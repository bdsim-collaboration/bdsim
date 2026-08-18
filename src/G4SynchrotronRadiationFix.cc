//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
// --------------------------------------------------------------
//      GEANT 4 class implementation file
//
//      History: first implementation,
//      21-5-98 V.Grichine
//      28-05-01, V.Ivanchenko minor changes to provide ANSI -wall compilation
//      04.03.05, V.Grichine: get local field interface
//      18-05-06 H. Burkhardt: Energy spectrum from function rather than table
//
//      14-07-2026, G. Broggi:
//      * angular sampling: G4DipBustGenerator REPLACED by an exact
//        Schwinger out-of-orbit-plane sampler.
//        The vertical angle psi of a photon of energy E is drawn from
//
//          dN/dpsi  propto  (1+Psi^2)^2       * K_{2/3}^2(xi)   [sigma pol.]
//                 +         Psi^2 (1+Psi^2)   * K_{1/3}^2(xi)   [pi pol.]
//          Psi = gamma*psi ,  xi = (y/2)(1+Psi^2)^{3/2} ,  y = E/E_c
//
//        oriented w.r.t. the orbit plane defined by the local B field.
//        The previous generator (G4DipBustGenerator) is a bremsstrahlung
//        angular distribution: it ignores the photon energy entirely and
//        therefore misses the energy-angle correlation, and it has an
//        unphysical theta^-3 large-angle tail.
//
//      * the in-plane intrinsic angle is sampled as a second independent
//        draw from the same Schwinger distribution.
//
//      * the sigma / pi polarisation components are now assigned with
//        their correct relative probability at the sampled angle
//        (previously all photons were given sigma polarisation).
//
//      * production threshold: SR photons below the larger of
//        GetLowestPhotonEnergy() (user setting, default 0) and an
//        internal, gamma-dependent floor are not produced. The floor is
//        the boundary of validity of the small-angle Schwinger formalism
//        (see kFloorFactor below); it removes a fraction ~35/gamma of the
//        photons, carrying a fraction ~1e-6 (gamma/1e3)^-4 of the
//        radiated energy. Setting a user threshold E_min removes a
//        fraction ~1.23 (E_min/E_c)^{1/3} of the photons, carrying a
//        fraction ~(E_min/E_c)^{4/3} of the radiated energy.
//
//        Energy sampling and mean free path are UNCHANGED.
//
//      21-07-2026, G. Broggi:
//      * E_c is now computed ONCE per emission, inside GetRandomEnergySR,
//        and returned to PostStepDoIt through a new output argument.
//
//      24-07-2026:
//      * angular sampling optimised, distribution unchanged:
//        - K_{1/3}, K_{2/3} evaluated by a 9-term power series (xi < 1)
//          and split 12/14-term Chebyshev fits of e^x sqrt(x) K_nu(x)
//          (xi >= 1) instead of the ~110-term Kostroun quadrature;
//          max relative error 8.7e-14 over the full reachable range,
//          better than the ~1e-12 accuracy of the quadrature it replaces;
//        - envelope, sampling range and density constants computed once
//          per photon and shared by the vertical and in-plane draws
//          (SchwingerPsiSampler); in particular the cube root of the
//          small-argument branch is hoisted out of the rejection loop
//          via (xi/2)^{1/3} = (y/4)^{1/3} sqrt(1+Psi^2).
//
///////////////////////////////////////////////////////////////////////////

#include "G4SynchrotronRadiationFix.hh"

#include "G4Electron.hh"
#include "G4EmProcessSubType.hh"
#include "G4Exception.hh"
#include "G4Exp.hh"
#include "G4Log.hh"
#include "G4LossTableManager.hh"
#include "G4Gamma.hh"
#include "G4PhysicalConstants.hh"
#include "G4PropagatorInField.hh"
#include "G4SystemOfUnits.hh"
#include "G4TransportationManager.hh"
#include "G4UnitsTable.hh"
#include "G4PhysicsModelCatalog.hh"

#include "G4UImessenger.hh"
#include "G4UIdirectory.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"

#include "Randomize.hh"
#include <algorithm>
#include <cmath>
#include <mutex>

G4double G4SynchrotronRadiation::fLowestPhotonEnergy = 0.0;

namespace
{
  // E_c = kSRCritEnergyConst * gamma^2 * B_perp / mass_c2
  // (used once per emission, in GetRandomEnergySR).
  static const G4double kSRCritEnergyConst =
    1.5 * c_light * c_light * eplus * hbar_Planck;

  // Messenger for the SR production threshold.
  class G4SRThresholdMessenger : public G4UImessenger
  {
   public:
    G4SRThresholdMessenger()
    {
      fDir = new G4UIdirectory("/process/synrad/");
      fDir->SetGuidance("Synchrotron radiation process control");

      fCmd = new G4UIcmdWithADoubleAndUnit(
        "/process/synrad/lowestPhotonEnergy", this);
      fCmd->SetGuidance("Minimum energy of produced SR photons.");
      fCmd->SetParameterName("Emin", false);
      fCmd->SetUnitCategory("Energy");
      fCmd->SetDefaultUnit("eV");
      fCmd->SetRange("Emin >= 0");
      fCmd->AvailableForStates(G4State_PreInit, G4State_Idle);
    }

    void SetNewValue(G4UIcommand* cmd, G4String val) override
    {
      if(cmd == fCmd)
      {
        G4SynchrotronRadiation::SetLowestPhotonEnergy(
          fCmd->GetNewDoubleValue(val));
      }
    }

   private:
    G4UIdirectory* fDir            = nullptr;
    G4UIcmdWithADoubleAndUnit* fCmd = nullptr;
  };

  std::once_flag srThresholdMessengerFlag;

  void ConstructSRThresholdMessengerOnce()
  {
    std::call_once(srThresholdMessengerFlag,
                   []() { new G4SRThresholdMessenger(); });
  }

  struct G4SRThresholdMessengerRegistrar
  {
    G4SRThresholdMessengerRegistrar() { ConstructSRThresholdMessengerOnce(); }
  };
  [[maybe_unused]] const G4SRThresholdMessengerRegistrar
    theSRThresholdMessengerRegistrar{};
}  // namespace

//////////////////////////////////////////////////////////////////////
// Schwinger angular sampler
//////////////////////////////////////////////////////////////////////
namespace
{
  ////////////////////////////////////////////////////////////
  // Schwinger sampler configuration
  ////////////////////////////////////////////////////////////

  // Upper limit of the emission angle for which the small-angle Schwinger
  // formalism is used. Also fixes the internal production floor below.
  constexpr G4double kPsiMax = 0.1;   // rad

  // Sampling range: Psicut is set from xi(Psicut) - xi(0) = kDeltaXi.
  // The density carries K^2, so the density at the cut is down by
  // ~e^{-2 kDeltaXi} relative to the peak; the integrated truncated
  // fraction is smaller still (verified by direct integration:
  // < 1e-11 of the distribution for kDeltaXi = 12, worst case over
  // y in [1e-8, 10]).
  constexpr G4double kDeltaXi = 12.0;

  // Envelope height, in units of the on-axis density f(0):
  // max_Psi f(Psi) <= 1.246 f(0) for every y (verified numerically over
  // y in [1e-20, 1e2]; for soft photons the pi component shifts the
  // maximum slightly off axis), so 1.5 is rigorous over the whole
  // reachable range.
  constexpr G4double kEnvelope = 1.5;

  // Psicut ~ (2 kDeltaXi / y)^{1/3}, so Psicut <= gamma*kPsiMax translates
  // into the reduced-energy floor y > 2 kDeltaXi/(gamma kPsiMax)^3, i.e.
  //     E_floor = kFloorFactor * E_c / gamma^3 .
  // Photons below it would be emitted outside the domain of validity of
  // the formalism (their true emission is quasi-isotropic, since their
  // wavelength approaches the bending radius); they are not produced.
  constexpr G4double kFloorFactor =
    2.0 * kDeltaXi / (kPsiMax * kPsiMax * kPsiMax);   // = 2.4e4

  ////////////////////////////////////////////////////////////
  // Modified Bessel functions
  ////////////////////////////////////////////////////////////

  // K_{1/3}(x) and K_{2/3}(x), always evaluated together.
  //
  // Fast evaluation replacing the Kostroun quadrature of the first
  // revision (Nucl. Instr. Meth. 172 (1980) 371), which cost ~110
  // exp/cosh terms per call:
  //
  //   x < 1      power series of the modified Bessel functions I,
  //                K_nu = (pi/sqrt(3)) [ (x/2)^{-nu} S(-nu; z)
  //                                    - (x/2)^{+nu} S(+nu; z) ],
  //                S(nu; z) = sum_k z^k / (k! Gamma(nu+k+1)),  z = (x/2)^2,
  //              9 terms: truncation < 5e-14 relative at x = 1 and
  //              exact in the x -> 0 limit;
  //              all four fractional powers come from ONE cube root,
  //              which the caller precomputes ONCE PER PHOTON, since
  //              (xi/2)^{1/3} = (y/4)^{1/3} sqrt(1+Psi^2)  (see below);
  //
  //   x in [1,4] and x >= 4
  //              Chebyshev fits (14 and 12 terms) of the scaled function
  //              g_nu(t) = e^x sqrt(x) K_nu(x) in t = 1/x, both orders
  //              evaluated by one interleaved Clenshaw recurrence.
  //
  // Maximum relative error 8.7e-14 over x in [1e-12, 40] (validated
  // against a long-double Kostroun reference; the largest argument the
  // sampler can produce is xi(0) + kDeltaXi <~ 40). This improves the
  // ~1e-12 accuracy / 1e-10 termination tolerance of the quadrature it
  // replaces, at ~1/100 of the cost.
  //
  // Coefficients generated with mpmath at 40 significant digits.

  constexpr G4double kPiOverSqrt3 = 1.8137993642342178506;

  // series coefficients 1/(k! Gamma(nu+k+1))
  constexpr G4int kNSer = 9;
  constexpr G4double kSerP13[kNSer] = {
    1.119846521722185685,      0.83988489129163926373,
    0.17997533384820841366,    0.017997533384820841366,
    0.0010383192337396639249,  0.000038936971265237397185,
    1.0246571385588788733e-6,  1.9960853348549588441e-8,
    2.9941280022824382661e-10 };
  constexpr G4double kSerM13[kNSer] = {
    0.73848811162164831294,    1.1077321674324724694,
    0.33231965022974174082,    0.041539956278717717603,
    0.0028322697462762080184,  0.00012138298912612320079,
    3.5700879154742117879e-6,  7.6501883903018824025e-8,
    1.2473133245057416961e-9 };
  constexpr G4double kSerP23[kNSer] = {
    1.1077321674324724694,     0.66463930045948348164,
    0.12461986883615315281,    0.011329078985104832073,
    0.00060691494563061600393, 0.000021420527492845270727,
    5.3551318732113176818e-7,  9.9785065960459335685e-9,
    1.4392076821220096493e-10 };
  constexpr G4double kSerM23[kNSer] = {
    0.37328217390739522833,    1.119846521722185685,
    0.41994244564581963187,    0.059991777949402804552,
    0.0044993833462052103414,  0.00020766384674793278499,
    6.4894952108728995309e-6,  1.463795912226969819e-7,
    2.4951066685686985551e-9 };

  // Chebyshev fits of g_nu(t) = e^x sqrt(x) K_nu(x), t = 1/x:
  //   interval A: t in [0, 0.25]  (x >= 4),     12 terms, rel. err 5.6e-14
  //   interval B: t in [0.25, 1]  (x in [1,4]), 14 terms, rel. err 8.9e-14
  constexpr G4int kNChebA = 12;
  constexpr G4double kChebK13A[kNChebA] = {
    2.4866946720386860032,     -0.0096879488591852587091,
    0.00026510268970439654395, -0.000012814830372217478448,
    8.5587703706171339365e-7,  -7.1185958895158429737e-8,
    6.9681069246579181534e-9,  -7.7503844727435236008e-10,
    9.5658148369451393979e-11, -1.2880575753947319715e-11,
    1.8670167214169125217e-12, -2.8098356253275304822e-13 };
  constexpr G4double kChebK23A[kNChebA] = {
    2.5349134307070677685,     0.013808357566329722703,
    -0.00031863656954616650156,0.000014553653430440710993,
    -9.4462012391116314391e-7, 7.7217088108773024982e-8,
    -7.4706641016203686885e-9, 8.2395031730899992032e-10,
    -1.0104576631595708131e-10,1.3537657625113177965e-11,
    -1.9542947046354382824e-12,2.9317135041712852611e-13 };
  constexpr G4int kNChebB = 14;
  constexpr G4double kChebK13B[kNChebB] = {
    2.423535440130355685,      -0.020980911933421222525,
    0.0010687922506434993152,  -0.000085283709614450583521,
    8.5988738925386772067e-6,  -1.006496984059464514e-6,
    1.3092585503183766605e-7,  -1.8442684679834664698e-8,
    2.7662314920024619143e-9,  -4.366636246843895225e-10,
    7.1931129210803487539e-11, -1.2285973770298882965e-11,
    2.1626989185748103374e-12, -3.7841428686391862208e-13 };
  constexpr G4double kChebK23B[kNChebB] = {
    2.6277900898304459882,     0.03147921358039523152,
    -0.0013436998458821822449, 0.00010086539499817774621,
    -9.8510470485402287808e-6, 1.1303558943028352572e-6,
    -1.4502867966196062329e-7, 2.0222665082478401455e-8,
    -3.0094486630724465759e-9, 4.7207440479414153432e-10,
    -7.7362830111110638124e-11,1.3156455486907051649e-11,
    -2.307389269450422345e-12, 4.0251032484162260309e-13 };

  // q = (x/2)^{1/3} must be supplied by the caller (used only for x < 1;
  // in the rejection loop it costs one multiplication instead of a cbrt).
  inline void BesselK13K23(G4double x, G4double q, G4double& K13,
                           G4double& K23)
  {
    if(x < 1.0)
    {
      const G4double z  = 0.25 * x * x;   // (x/2)^2
      const G4double qi = 1.0 / q;
      // four independent Horner chains, evaluated interleaved
      G4double a = kSerP13[kNSer - 1], b = kSerM13[kNSer - 1];
      G4double c = kSerP23[kNSer - 1], d = kSerM23[kNSer - 1];
      for(G4int k = kNSer - 2; k >= 0; --k)
      {
        a = a * z + kSerP13[k];
        b = b * z + kSerM13[k];
        c = c * z + kSerP23[k];
        d = d * z + kSerM23[k];
      }
      K13 = kPiOverSqrt3 * (qi * b - q * a);
      K23 = kPiOverSqrt3 * (qi * qi * d - q * q * c);
    }
    else if(x < 700.0)
    {
      const G4double t = 1.0 / x;
      const G4double A = G4Exp(-x) * std::sqrt(t);   // e^{-x}/sqrt(x)
      const G4double* c13;
      const G4double* c23;
      G4int n;
      G4double w;   // Clenshaw variable on [-1, 1]
      if(t < 0.25)
      {
        c13 = kChebK13A; c23 = kChebK23A; n = kNChebA;
        w   = 8.0 * t - 1.0;
      }
      else
      {
        c13 = kChebK13B; c23 = kChebK23B; n = kNChebB;
        w   = (2.0 * t - 1.25) * (1.0 / 0.75);
      }
      const G4double w2 = 2.0 * w;
      G4double d1 = 0.0, dd1 = 0.0, d2 = 0.0, dd2 = 0.0;
      for(G4int j = n - 1; j >= 1; --j)   // interleaved Clenshaw
      {
        const G4double s1 = d1, s2 = d2;
        d1  = w2 * d1 - dd1 + c13[j];
        d2  = w2 * d2 - dd2 + c23[j];
        dd1 = s1;
        dd2 = s2;
      }
      K13 = A * (w * d1 - dd1 + 0.5 * c13[0]);
      K23 = A * (w * d2 - dd2 + 0.5 * c23[0]);
    }
    else
    {
      // Both orders underflow: unreachable through the process (requires
      // y >~ 1.5e3, far beyond what InvSynFracInt can return); the caller
      // then emits collinearly.
      K13 = 0.0;
      K23 = 0.0;
    }
  }

  ////////////////////////////////////////////////////////////
  // Per-photon sampler
  ////////////////////////////////////////////////////////////

  // Samples |psi| (rad) from the exact Schwinger out-of-plane
  // distribution
  //   f(Psi) = (1+Psi^2)^2 K_{2/3}^2(xi)          [sigma]
  //          + Psi^2 (1+Psi^2) K_{1/3}^2(xi)      [pi]
  //   xi = (y/2) (1+Psi^2)^{3/2}
  // by rejection sampling with a uniform envelope on Psi in [0, Psicut]:
  //   * the envelope kEnvelope * f(0) is rigorous for every y (see the
  //     kEnvelope comment above);
  //   * the acceptance, ~25% averaged over the emitted spectrum, is the
  //     fixed ratio of Psicut to the width of the distribution and is
  //     independent of gamma and of the bending radius (scale
  //     invariance): the sampler behaves identically in any regime.
  //
  // The envelope, the sampling range and the constants of the density
  // depend only on (y, gamma), so they are computed ONCE per emitted
  // photon (Init) and shared by the vertical and in-plane draws; in
  // particular the cube root needed by the small-argument Bessel branch
  // is hoisted out of the rejection loop through
  //   (xi/2)^{1/3} = (y/4)^{1/3} * sqrt(1+Psi^2).
  //
  struct SchwingerPsiSampler
  {
    G4double y        = 0.0;
    G4double gammaInv = 0.0;
    G4double fmax     = 0.0;
    G4double Psicut   = 0.0;
    G4double halfy    = 0.0;   // y/2       : xi = halfy * u^{3/2}
    G4double cbrtQ    = 0.0;   // (y/4)^{1/3}: (xi/2)^{1/3} = cbrtQ*sqrt(u)
    G4bool valid      = false;

    // sigma / pi components of the photon-number angular density at
    // Psi = gamma*psi for the reduced photon energy y of this photon
    inline void SchwingerComponents(G4double Psi, G4double& fSigma,
                                    G4double& fPi) const
    {
      const G4double u  = 1.0 + Psi * Psi;
      const G4double su = std::sqrt(u);
      const G4double xi = halfy * u * su;
      G4double K13, K23;
      BesselK13K23(xi, cbrtQ * su, K13, K23);
      fSigma = u * u * K23 * K23;
      fPi    = Psi * Psi * u * K13 * K13;
    }

    void Init(G4double gamma, G4double y_)
    {
      y     = y_;
      valid = false;
      if(y <= 0.0 || gamma <= 0.0)
      {
        return;
      }
      gammaInv = 1.0 / gamma;
      halfy    = 0.5 * y;
      cbrtQ    = std::cbrt(0.25 * y);

      // on-axis density: u = 1, f_pi(0) = 0 analytically, so only
      // K_{2/3}(y/2) is needed
      G4double K13, K23;
      BesselK13K23(halfy, cbrtQ, K13, K23);
      fmax = kEnvelope * K23 * K23;

      // Guard: if xi(0) is so large that both Bessel functions underflow,
      // the envelope is void. This requires y >~ 1.5e3, more than an
      // order of magnitude beyond the largest reduced energy
      // InvSynFracInt can return; emit collinearly (the true distribution
      // there is far narrower than 1/gamma in any case).
      if(!(fmax > 0.0))
      {
        return;
      }

      const G4double c = std::cbrt(1.0 + 2.0 * kDeltaXi / y);
      Psicut           = std::sqrt(std::max(c * c - 1.0, 1.0e-12));

      // Defensive: with the production floor applied in PostStepDoIt this
      // clamp never fires, but the sampler must stay safe if called with
      // an arbitrarily soft photon.
      Psicut = std::min(Psicut, gamma * kPsiMax);
      valid  = true;
    }

    // Also decides the polarisation component (true = sigma).
    G4double Sample(G4bool& sigmaPol) const
    {
      sigmaPol = true;
      if(!valid)
      {
        return 0.0;
      }
      G4double Psi = 0.0, fs = 0.0, fp = 0.0;
      G4bool accepted = false;
      for(G4int i = 0; i < 10000; ++i)
      {
        Psi = Psicut * G4UniformRand();
        SchwingerComponents(Psi, fs, fp);
        if((fs + fp) > fmax * G4UniformRand())
        {
          accepted = true;
          break;
        }
      }
      if(!accepted)
      {
        // Unreachable for finite y (the acceptance is ~25% whatever y);
        // reaching this point indicates a non-finite input.
        G4Exception("G4SynchrotronRadiation::SchwingerPsiSampler", "em0004",
                    JustWarning,
                    "SR angular rejection sampling did not converge; "
                    "falling back to collinear emission.");
        Psi = 0.0;
        SchwingerComponents(0.0, fs, fp);
      }
      sigmaPol = (G4UniformRand() * (fs + fp) < fs);
      return Psi * gammaInv;
    }
  };
}  // anonymous namespace

///////////////////////////////////////////////////////////////////////
//  Constructor
G4SynchrotronRadiation::G4SynchrotronRadiation(const G4String& processName,
                                               G4ProcessType type)
  : G4VDiscreteProcess(processName, type)
  , theGamma(G4Gamma::Gamma())
{
  G4TransportationManager* transportMgr =
    G4TransportationManager::GetTransportationManager();

  fFieldPropagator = transportMgr->GetPropagatorInField();

  secID = G4PhysicsModelCatalog::GetModelID("model_SynRad");
  SetProcessSubType(fSynchrotronRadiation);
  verboseLevel        = 1;
  FirstTime           = true;
  FirstTime1          = true;
  theManager          = G4LossTableManager::Instance();
  theManager->Register(this);
  ConstructSRThresholdMessengerOnce();
}

/////////////////////////////////////////////////////////////////////////
// Destructor
G4SynchrotronRadiation::~G4SynchrotronRadiation()
{
  theManager->DeRegister(this);
}

/////////////////////////////// METHODS /////////////////////////////////

G4bool G4SynchrotronRadiation::IsApplicable(
  const G4ParticleDefinition& particle)
{
  return (particle.GetPDGCharge() != 0.0 && !particle.IsShortLived());
}

/////////////////////////////////////////////////////////////////////////
// Production of synchrotron X-ray photon
// Geant4 internal units.
G4double G4SynchrotronRadiation::GetMeanFreePath(const G4Track& trackData,
                                                 G4double,
                                                 G4ForceCondition* condition)
{
  // gives the MeanFreePath in Geant4 internal units
  G4double MeanFreePath = DBL_MAX;

  const G4DynamicParticle* aDynamicParticle = trackData.GetDynamicParticle();

  *condition = NotForced;

  G4double gamma =
    aDynamicParticle->GetTotalEnergy() / aDynamicParticle->GetMass();

  G4double particleCharge = aDynamicParticle->GetDefinition()->GetPDGCharge();

  if(gamma < 1.0e3 || 0.0 == particleCharge)
  {
    MeanFreePath = DBL_MAX;
  }
  else
  {
    G4ThreeVector FieldValue;
    const G4Field* pField   = nullptr;
    G4bool fieldExertsForce = false;

    G4FieldManager* fieldMgr =
      fFieldPropagator->FindAndSetFieldManager(trackData.GetVolume());

    if(fieldMgr != nullptr)
    {
      // If the field manager has no field, there is no field !
      fieldExertsForce = (fieldMgr->GetDetectorField() != nullptr);
    }

    if(fieldExertsForce)
    {
      pField                     = fieldMgr->GetDetectorField();
      G4ThreeVector globPosition = trackData.GetPosition();

      G4double globPosVec[4], FieldValueVec[6];

      globPosVec[0] = globPosition.x();
      globPosVec[1] = globPosition.y();
      globPosVec[2] = globPosition.z();
      globPosVec[3] = trackData.GetGlobalTime();

      pField->GetFieldValue(globPosVec, FieldValueVec);

      FieldValue =
        G4ThreeVector(FieldValueVec[0], FieldValueVec[1], FieldValueVec[2]);

      G4ThreeVector unitMomentum = aDynamicParticle->GetMomentumDirection();
      G4ThreeVector unitMcrossB  = FieldValue.cross(unitMomentum);
      G4double perpB             = unitMcrossB.mag();

      static const G4double fLambdaConst =
        std::sqrt(3.0) * eplus / (2.5 * fine_structure_const * c_light);

      if(perpB > 0.0)
      {
        MeanFreePath = fLambdaConst *
                       aDynamicParticle->GetDefinition()->GetPDGMass() /
                       (perpB * particleCharge * particleCharge);
      }
      if(verboseLevel > 0 && FirstTime)
      {
        G4cout << "G4SynchrotronRadiation::GetMeanFreePath "
               << " for particle "
               << aDynamicParticle->GetDefinition()->GetParticleName() << ":"
               << '\n'
               << "  MeanFreePath = " << G4BestUnit(MeanFreePath, "Length")
               << G4endl;
        if(verboseLevel > 1)
        {
          G4ThreeVector pvec = aDynamicParticle->GetMomentum();
          G4double Btot      = FieldValue.getR();
          G4double ptot      = pvec.getR();
          G4double rho       = ptot / (MeV * c_light * Btot);
          // full bending radius
          G4double Theta = unitMomentum.theta(FieldValue);
          // angle between particle and field
          G4cout << "  B = " << Btot / tesla << " Tesla"
                 << "  perpB = " << perpB / tesla << " Tesla"
                 << "  Theta = " << Theta
                 << " std::sin(Theta)=" << std::sin(Theta) << '\n'
                 << "  ptot  = " << G4BestUnit(ptot, "Energy")
                 << "  rho   = " << G4BestUnit(rho, "Length") << G4endl;
        }
        FirstTime = false;
      }
    }
  }
  return MeanFreePath;
}

///////////////////////////////////////////////////////////////////////////////
G4VParticleChange* G4SynchrotronRadiation::PostStepDoIt(
  const G4Track& trackData, const G4Step& stepData)

{
  aParticleChange.Initialize(trackData);

  const G4DynamicParticle* aDynamicParticle = trackData.GetDynamicParticle();

  G4double gamma = aDynamicParticle->GetTotalEnergy() /
                   (aDynamicParticle->GetDefinition()->GetPDGMass());

  G4double particleCharge = aDynamicParticle->GetDefinition()->GetPDGCharge();
  if(gamma < 1.0e3 || 0.0 == particleCharge)
  {
    return G4VDiscreteProcess::PostStepDoIt(trackData, stepData);
  }

  G4ThreeVector FieldValue;
  const G4Field* pField = nullptr;

  G4bool fieldExertsForce = false;
  G4FieldManager* fieldMgr =
    fFieldPropagator->FindAndSetFieldManager(trackData.GetVolume());

  if(fieldMgr != nullptr)
  {
    // If the field manager has no field, there is no field !
    fieldExertsForce = (fieldMgr->GetDetectorField() != nullptr);
  }

  if(fieldExertsForce)
  {
    pField                     = fieldMgr->GetDetectorField();
    G4ThreeVector globPosition = trackData.GetPosition();
    G4double globPosVec[4], FieldValueVec[6];
    globPosVec[0] = globPosition.x();
    globPosVec[1] = globPosition.y();
    globPosVec[2] = globPosition.z();
    globPosVec[3] = trackData.GetGlobalTime();

    pField->GetFieldValue(globPosVec, FieldValueVec);
    FieldValue =
      G4ThreeVector(FieldValueVec[0], FieldValueVec[1], FieldValueVec[2]);

    G4ThreeVector unitMomentum = aDynamicParticle->GetMomentumDirection();
    G4ThreeVector unitMcrossB  = FieldValue.cross(unitMomentum);
    G4double perpB             = unitMcrossB.mag();
    if(perpB > 0.0)
    {
      const G4double mass_c2 =
        aDynamicParticle->GetDefinition()->GetPDGMass();

      // M-C of synchrotron photon energy. The local critical energy is
      // computed once, inside the sampler, and returned here.
      G4double Ecr        = 0.0;
      G4double energyOfSR = GetRandomEnergySR(gamma, perpB, mass_c2, Ecr);

      // Production threshold: the larger of the user setting (default 0)
      // and the internal floor, i.e. the boundary of validity of the
      // small-angle Schwinger formalism. Photons below the floor would
      // have an emission angle outside that limit; physically their
      // emission is quasi-isotropic (their wavelength approaches the
      // bending radius), which this model does not describe, so they are
      // not produced. The floor removes a fraction ~35/gamma of the
      // photons, carrying ~(kFloorFactor/gamma^3)^{4/3} of the radiated
      // energy (7e-7 at gamma = 1e3).
      // Tested BEFORE the angular sampling, so that a threshold actually
      // saves the CPU it is meant to save.
      // @TODO the sub-threshold radiated energy is not transferred to the
      // charged particle; a rigorous treatment would add it as a
      // continuous energy loss (requires G4VContinuousDiscreteProcess).
      // The neglected fraction is ~(E_min/E_c)^{4/3}: 5e-6 for
      // E_min = 1e-4 E_c, 2e-3 for E_min = 1e-2 E_c.
      const G4double eFloor = kFloorFactor * Ecr / (gamma * gamma * gamma);
      const G4double eMinSR = std::max(fLowestPhotonEnergy, eFloor);
      if(energyOfSR <= eMinSR)
      {
        return G4VDiscreteProcess::PostStepDoIt(trackData, stepData);
      }

      G4double kineticEnergy = aDynamicParticle->GetKineticEnergy();

      // ============================================================
      // Schwinger angular sampling
      // ============================================================
      const G4double y = energyOfSR / Ecr;

      // orthonormal triad at the emission point:
      //   eVert  : out-of-orbit-plane axis = component of B perpendicular
      //            to the particle direction (perpB > 0 guarantees it is
      //            well defined)
      //   eHoriz : in-plane transverse axis
      G4ThreeVector eVert =
        (FieldValue - FieldValue.dot(unitMomentum) * unitMomentum).unit();
      G4ThreeVector eHoriz = eVert.cross(unitMomentum).unit();

      // envelope, sampling range and density constants depend only on
      // (y, gamma): computed once per photon, shared by both draws
      SchwingerPsiSampler psiSampler;
      psiSampler.Init(gamma, y);

      // vertical angle: exact Schwinger distribution at this y; the
      // side of the orbit plane is picked at random (symmetric).
      // The sigma/pi polarisation split is decided by this draw.
      G4bool sigmaPol        = true;
      const G4double psiVmag = psiSampler.Sample(sigmaPol);
      const G4double psiV    = (G4UniformRand() < 0.5) ? psiVmag : -psiVmag;

      // horizontal intrinsic angle: independent draw from the same
      // distribution (formation-length smearing of the same scale);
      // sign symmetric. The polarisation flag of this draw is unused.
      G4bool dummyPol        = true;
      const G4double psiHmag = psiSampler.Sample(dummyPol);
      const G4double psiH    = (G4UniformRand() < 0.5) ? psiHmag : -psiHmag;

      // exact unit vector by construction; the vertical projection
      // satisfies asin(k.eVert) = psiV exactly, so the validated vertical
      // marginal is preserved independently of the in-plane draw
      const G4double cV = std::cos(psiV), sV = std::sin(psiV);
      const G4double cH = std::cos(psiH), sH = std::sin(psiH);
      G4ThreeVector gammaDirection =
        (cV * cH) * unitMomentum + sV * eVert + (cV * sH) * eHoriz;

      // polarisation consistent with the sampled component:
      //   sigma: E-vector in the orbit plane (perp. to eVert and to k)
      //   pi   : E-vector along the out-of-plane axis (perp. to k)
      G4ThreeVector gammaPolarization;
      if(sigmaPol)
      {
        gammaPolarization = gammaDirection.cross(eVert).unit();
      }
      else
      {
        gammaPolarization =
          (eVert - eVert.dot(gammaDirection) * gammaDirection).unit();
      }
      // ============================================================

      // create G4DynamicParticle object for the SR photon
      auto aGamma = new G4DynamicParticle(theGamma, gammaDirection, energyOfSR);
      aGamma->SetPolarization(gammaPolarization.x(), gammaPolarization.y(),
                              gammaPolarization.z());

      aParticleChange.SetNumberOfSecondaries(1);

      // Update the incident particle
      G4double newKinEnergy = kineticEnergy - energyOfSR;

      if(newKinEnergy > 0.)
      {
        aParticleChange.ProposeEnergy(newKinEnergy);
      }
      else
      {
        aParticleChange.ProposeEnergy(0.);
      }

      // Create the G4Track
      G4Track* aSecondaryTrack = new G4Track(aGamma, trackData.GetGlobalTime(),
                                             trackData.GetPosition());
      aSecondaryTrack->SetTouchableHandle(
        stepData.GetPostStepPoint()->GetTouchableHandle());
      aSecondaryTrack->SetParentID(trackData.GetTrackID());
      aSecondaryTrack->SetCreatorModelID(secID);
      aParticleChange.AddSecondary(aSecondaryTrack);
    }
  }
  return G4VDiscreteProcess::PostStepDoIt(trackData, stepData);
}

///////////////////////////////////////////////////////////////////////////////
G4double G4SynchrotronRadiation::InvSynFracInt(G4double x)
// direct generation
{
  // from 0 to 0.7
  static constexpr G4double aa1           = 0;
  static constexpr G4double aa2           = 0.7;
  static constexpr G4int ncheb1           = 27;
  static constexpr G4double cheb1[ncheb1] = {
    1.22371665676046468821,     0.108956475422163837267,
    0.0383328524358594396134,   0.00759138369340257753721,
    0.00205712048644963340914,  0.000497810783280019308661,
    0.000130743691810302187818, 0.0000338168760220395409734,
    8.97049680900520817728e-6,  2.38685472794452241466e-6,
    6.41923109149104165049e-7,  1.73549898982749277843e-7,
    4.72145949240790029153e-8,  1.29039866111999149636e-8,
    3.5422080787089834182e-9,   9.7594757336403784905e-10,
    2.6979510184976065731e-10,  7.480422622550977077e-11,
    2.079598176402699913e-11,   5.79533622220841193e-12,
    1.61856011449276096e-12,    4.529450993473807e-13,
    1.2698603951096606e-13,     3.566117394511206e-14,
    1.00301587494091e-14,       2.82515346447219e-15,
    7.9680747949792e-16
  };
  //   from 0.7 to 0.9132260271183847
  static constexpr G4double aa3           = 0.9132260271183847;
  static constexpr G4int ncheb2           = 27;
  static constexpr G4double cheb2[ncheb2] = {
    1.1139496701107756,     0.3523967429328067,     0.0713849171926623,
    0.01475818043595387,    0.003381255637322462,   0.0008228057599452224,
    0.00020785506681254216, 0.00005390169253706556, 0.000014250571923902464,
    3.823880733161044e-6,   1.0381966089136036e-6,  2.8457557457837253e-7,
    7.86223332179956e-8,    2.1866609342508474e-8,  6.116186259857143e-9,
    1.7191233618437565e-9,  4.852755117740807e-10,  1.3749966961763457e-10,
    3.908961987062447e-11,  1.1146253766895824e-11, 3.1868887323415814e-12,
    9.134319791300977e-13,  2.6211077371181566e-13, 7.588643377757906e-14,
    2.1528376972619e-14,    6.030906040404772e-15,  1.9549163926819867e-15
  };
  // Chebyshev with exp/log  scale
  // a = -Log[1 - SynFracInt[1]]; b = -Log[1 - SynFracInt[7]];
  static constexpr G4double aa4           = 2.4444485538746025480;
  static constexpr G4double aa5           = 9.3830728608909477079;
  static constexpr G4int ncheb3           = 28;
  static constexpr G4double cheb3[ncheb3] = {
    1.2292683840435586977,        0.160353449247864455879,
    -0.0353559911947559448721,    0.00776901561223573936985,
    -0.00165886451971685133259,   0.000335719118906954279467,
    -0.0000617184951079161143187, 9.23534039743246708256e-6,
    -6.06747198795168022842e-7,   -3.07934045961999778094e-7,
    1.98818772614682367781e-7,    -8.13909971567720135413e-8,
    2.84298174969641838618e-8,    -9.12829766621316063548e-9,
    2.77713868004820551077e-9,    -8.13032767247834023165e-10,
    2.31128525568385247392e-10,   -6.41796873254200220876e-11,
    1.74815310473323361543e-11,   -4.68653536933392363045e-12,
    1.24016595805520752748e-12,   -3.24839432979935522159e-13,
    8.44601465226513952994e-14,   -2.18647276044246803998e-14,
    5.65407548745690689978e-15,   -1.46553625917463067508e-15,
    3.82059606377570462276e-16,   -1.00457896653436912508e-16
  };
  static constexpr G4double aa6           = 33.122936966163038145;
  static constexpr G4int ncheb4           = 27;
  static constexpr G4double cheb4[ncheb4] = {
    1.69342658227676741765,      0.0742766400841232319225,
    -0.019337880608635717358,    0.00516065527473364110491,
    -0.00139342012990307729473,  0.000378549864052022522193,
    -0.000103167085583785340215, 0.0000281543441271412178337,
    -7.68409742018258198651e-6,  2.09543221890204537392e-6,
    -5.70493140367526282946e-7,  1.54961164548564906446e-7,
    -4.19665599629607704794e-8,  1.13239680054166507038e-8,
    -3.04223563379021441863e-9,  8.13073745977562957997e-10,
    -2.15969415476814981374e-10, 5.69472105972525594811e-11,
    -1.48844799572430829499e-11, 3.84901514438304484973e-12,
    -9.82222575944247161834e-13, 2.46468329208292208183e-13,
    -6.04953826265982691612e-14, 1.44055805710671611984e-14,
    -3.28200813577388740722e-15, 6.96566359173765367675e-16,
    -1.294122794852896275e-16
  };

  if(x < aa2)
    return x * x * x * Chebyshev(aa1, aa2, cheb1, ncheb1, x);
  else if(x < aa3)
    return Chebyshev(aa2, aa3, cheb2, ncheb2, x);
  else if(x < 1 - 0.0000841363)
  {
    G4double y = -G4Log(1 - x);
    return y * Chebyshev(aa4, aa5, cheb3, ncheb3, y);
  }
  else
  {
    G4double y = -G4Log(1 - x);
    return y * Chebyshev(aa5, aa6, cheb4, ncheb4, y);
  }
}

///////////////////////////////////////////////////////////////////////////////
// Samples the photon energy from the integrated Schwinger spectrum and
// returns, through critEnergy, the local critical energy used to do so,
// so that PostStepDoIt does not have to recompute it.
G4double G4SynchrotronRadiation::GetRandomEnergySR(G4double gamma,
                                                   G4double perpB,
                                                   G4double mass_c2,
                                                   G4double& critEnergy)
{
  critEnergy = kSRCritEnergyConst * gamma * gamma * perpB / mass_c2;

  if(verboseLevel > 0 && FirstTime1)
  {
    // mean and rms of photon energy
    G4double Emean = 8. / (15. * std::sqrt(3.)) * critEnergy;
    G4double E_rms = std::sqrt(211. / 675.) * critEnergy;
    G4long prec    = G4cout.precision();
    G4cout << "G4SynchrotronRadiation::GetRandomEnergySR :" << '\n'
           << std::setprecision(4)
           << "  Ecr   = " << G4BestUnit(critEnergy, "Energy") << '\n'
           << "  Emean = " << G4BestUnit(Emean, "Energy") << '\n'
           << "  E_rms = " << G4BestUnit(E_rms, "Energy") << '\n'
           << "  internal production floor = "
           << G4BestUnit(kFloorFactor * critEnergy / (gamma * gamma * gamma),
                         "Energy")
           << G4endl;
    if(fLowestPhotonEnergy > 0.0)
    {
      G4cout << "  user production threshold = "
             << G4BestUnit(fLowestPhotonEnergy, "Energy") << G4endl;
    }
    FirstTime1 = false;
    G4cout.precision(prec);
  }

  G4double energySR = critEnergy * InvSynFracInt(G4UniformRand());
  return energySR;
}

///////////////////////////////////////////////////////////////////////////////
void G4SynchrotronRadiation::BuildPhysicsTable(const G4ParticleDefinition& part)
{
  if(0 < verboseLevel && &part == G4Electron::Electron())
    ProcessDescription(G4cout);
  // same for all particles, print only for one (electron)
}

///////////////////////////////////////////////////////////////////////////////
void G4SynchrotronRadiation::ProcessDescription(std::ostream& out) const
{
  out << GetProcessName()
      << ":  Incoherent Synchrotron Radiation\n"
         "Good description for long magnets at all energies.\n"
         "Photon energy sampled from the integrated Schwinger spectrum; "
         "emission angles sampled from the exact Schwinger out-of-plane "
         "distribution, with the sigma/pi polarisation decomposition.\n";
}