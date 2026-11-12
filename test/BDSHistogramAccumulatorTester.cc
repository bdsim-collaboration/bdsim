/*
Beam Delivery Simulation (BDSIM) Copyright (C) BDSIM Collaboration, 2001 - 2026.

This file is part of BDSIM.

BDSIM is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published
by the Free Software Foundation version 3 of the License.

BDSIM is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with BDSIM.  If not, see <http://www.gnu.org/licenses/>.
*/
/**
 * Test program for the histogram accumulators in analysis/.
 *
 * Randomly filled 'event' histograms are accumulated and the results compared
 * bin by bin (including under and overflow bins) against a brute force two-pass
 * calculation of the mean and standard error on the mean from a full record of
 * every event.
 *
 * This file is built twice as the accumulators are split across two libraries
 * that must not be linked together:
 *  - linked to librebdsim with TEST_REBDSIM_ACCUMULATORS defined: tests
 *    HistogramAccumulator, HistogramAccumulatorFast, HistogramAccumulatorSum
 *    and HistogramAccumulatorMerge in 1, 2 and 3D.
 *  - linked to libbdsim: tests HistogramAccumulator and HistogramAccumulatorFast
 *    in 1, 2 and 3D, and also 4D if USE_BOOST. The 4D histogram global bin index
 *    functions only work in libbdsim (not libbdsimRootEvent with __ROOTBUILD__).
 *
 * Returns 0 if all checks pass, 1 otherwise.
 */
#include "HistogramAccumulator.hh"
#include "HistogramAccumulatorFast.hh"
#ifdef TEST_REBDSIM_ACCUMULATORS
#include "HistogramAccumulatorMerge.hh"
#include "HistogramAccumulatorSum.hh"
#endif

#if defined(USE_BOOST) && !defined(TEST_REBDSIM_ACCUMULATORS)
#define TEST_4D
#include "BDSBH4D.hh"
#include "BDSBH4DBase.hh"
#include "BDSBH4DTypeDefs.hh"
#include "BDSHistBinMapper.hh"
#endif

#include "TH1.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TH3D.h"

#include <algorithm>
#include <cmath>
#include <functional>
#include <iostream>
#include <random>
#include <set>
#include <sstream>
#include <string>
#include <vector>

namespace
{
  int nChecks   = 0;
  int nFailures = 0;

  /// Maximum number of individual bin failures printed per comparison.
  const int maxBinFailuresPrinted = 5;

  /// The accumulators are mathematically exact but the order of floating point operations
  /// differs between them and the reference, so results agree to ~1e-14 relative, not bitwise.
  bool Close(double a, double b, double relTol = 1e-12, double absTol = 1e-14)
  {
    double scale = std::max(std::abs(a), std::abs(b));
    return std::abs(a - b) <= std::max(absTol, relTol * scale);
  }

  void Check(bool ok, const std::string& testName, const std::string& what)
  {
    nChecks++;
    if (!ok)
      {
        nFailures++;
        std::cout << "  FAIL [" << testName << "] " << what << std::endl;
      }
  }

  /// Contents and errors of every cell of a histogram (including under/overflow)
  /// flattened into one vector in a fixed order.
  struct Cells
  {
    std::vector<double> content;
    std::vector<double> error;
  };

  /// Read all cells of a 1,2 or 3D histogram by ROOT global bin index.
  Cells ReadCells(TH1* h)
  {
    Cells c;
    for (Int_t i = 0; i < h->GetNcells(); i++)
      {
        c.content.push_back(h->GetBinContent(i));
        c.error.push_back(h->GetBinError(i));
      }
    return c;
  }

#ifdef TEST_4D
  /// Read all cells of a 4D histogram, including the under and overflow bins of
  /// each axis (boost index -1 to nBins inclusive).
  Cells ReadCells4D(TH1* hIn)
  {
    BDSBH4DBase* h = static_cast<BDSBH4DBase*>(hIn);
    Cells c;
    for (int i = -1; i <= h->GetNbinsX(); i++)
      {
        for (int j = -1; j <= h->GetNbinsY(); j++)
          {
            for (int k = -1; k <= h->GetNbinsZ(); k++)
              {
                for (int l = -1; l <= h->GetNbinsE(); l++)
                  {
                    c.content.push_back(h->At(i,j,k,l));
                    c.error.push_back(h->AtError(i,j,k,l));
                  }
              }
          }
      }
    return c;
  }
#endif

  /// Brute force two-pass mean and standard error on the mean per cell.
  Cells ReferenceMean(const std::vector<std::vector<double>>& events)
  {
    Cells r;
    if (events.empty())
      {return r;}
    std::size_t nCells = events[0].size();
    double n = (double)events.size();
    r.content.assign(nCells, 0);
    r.error.assign(nCells, 0);
    for (const auto& ev : events)
      {
        for (std::size_t i = 0; i < nCells; i++)
          {r.content[i] += ev[i];}
      }
    for (auto& v : r.content)
      {v /= n;}
    if (events.size() < 2)
      {return r;} // error defined as 0 for 1 entry
    for (const auto& ev : events)
      {
        for (std::size_t i = 0; i < nCells; i++)
          {r.error[i] += std::pow(ev[i] - r.content[i], 2);}
      }
    for (auto& v : r.error)
      {v = std::sqrt(v / (n * (n - 1)));} // sqrt(variance / n)
    return r;
  }

  /// Compare cells bin by bin. Returns true if all match.
  bool CompareCells(const Cells& result,
                    const Cells& expected,
                    const std::string& testName,
                    bool compareErrors = true)
  {
    nChecks++;
    if (result.content.size() != expected.content.size())
      {
        nFailures++;
        std::cout << "  FAIL [" << testName << "] number of cells differs: " << result.content.size()
                  << " vs expected " << expected.content.size() << std::endl;
        return false;
      }
    int nBad = 0;
    for (std::size_t i = 0; i < result.content.size(); i++)
      {
        bool contentOK = Close(result.content[i], expected.content[i]);
        bool errorOK   = !compareErrors || Close(result.error[i], expected.error[i]);
        if (!contentOK || !errorOK)
          {
            if (nBad < maxBinFailuresPrinted)
              {
                std::cout << "  FAIL [" << testName << "] cell " << i
                          << " content " << result.content[i] << " expected " << expected.content[i];
                if (compareErrors)
                  {std::cout << " error " << result.error[i] << " expected " << expected.error[i];}
                std::cout << std::endl;
              }
            nBad++;
          }
      }
    if (nBad > maxBinFailuresPrinted)
      {std::cout << "  FAIL [" << testName << "] ... " << nBad << " cells differ in total" << std::endl;}
    if (nBad > 0)
      {nFailures++;}
    return nBad == 0;
  }

  /// A source of randomly filled event histograms of a given dimension. The same
  /// seed gives the same sequence of events.
  class EventGenerator
  {
  public:
    EventGenerator(int nDimensionsIn, unsigned int seed):
      nDimensions(nDimensionsIn),
      rng(seed),
      coord(-0.2, 1.2), // axes are [0,1] so this also fills under and overflow bins
      weight(0.5, 2.0),
      nFills(0, 6),
      uniform(0, 1)
    {
      std::string name = "eventHist" + std::to_string(nDimensions) + "D_" + std::to_string(seed);
      switch (nDimensions)
        {
        case 1:
          {hist = new TH1D(name.c_str(), name.c_str(), 10, 0, 1); break;}
        case 2:
          {hist = new TH2D(name.c_str(), name.c_str(), 5, 0, 1, 4, 0, 1); break;}
        case 3:
          {hist = new TH3D(name.c_str(), name.c_str(), 4, 0, 1, 3, 0, 1, 3, 0, 1); break;}
#ifdef TEST_4D
        case 4:
          {
            std::string title = name;
            hist = new BDSBH4D<boost_histogram_linear>(name, title, "linear",
                                                       3, 0, 1,
                                                       3, 0, 1,
                                                       2, 0, 1,
                                                       4, 0, 1);
            break;
          }
#endif
        default:
          {hist = nullptr; break;}
        }
      if (nDimensions < 4 && hist->GetSumw2N() == 0)
        {hist->Sumw2();}
    }
    ~EventGenerator(){delete hist;}

    /// Reset the event histogram and fill it with a random number of entries
    /// (sometimes none). Record the global bins filled as BDSOutputStructures does.
    void Next(bool emptyEventsAllowed = true, bool inRangeOnly = false)
    {
      Reset();
      binsFilled.clear();
      int nToFill = nFills(rng);
      if (!emptyEventsAllowed)
        {nToFill = std::max(nToFill, 1);}
      for (int i = 0; i < nToFill; i++)
        {
          double x = inRangeOnly ? uniform(rng) : coord(rng);
          double y = inRangeOnly ? uniform(rng) : coord(rng);
          double z = inRangeOnly ? uniform(rng) : coord(rng);
#ifdef TEST_4D
          double e = inRangeOnly ? uniform(rng) : coord(rng);
#endif
          double w = weight(rng);
          Int_t globalBin = -1;
          switch (nDimensions)
            {
            case 1:
              {
                globalBin = hist->Fill(x, w);
                if (globalBin < 0)
                  {globalBin = hist->FindBin(x);}
                break;
              }
            case 2:
              {
                globalBin = static_cast<TH2D*>(hist)->Fill(x, y, w);
                if (globalBin < 0)
                  {globalBin = hist->FindBin(x, y);}
                break;
              }
            case 3:
              {
                globalBin = static_cast<TH3D*>(hist)->Fill(x, y, z, w);
                if (globalBin < 0)
                  {globalBin = hist->FindBin(x, y, z);}
                break;
              }
#ifdef TEST_4D
            case 4:
              {globalBin = static_cast<BDSBH4DBase*>(hist)->Fill_BDSBH4D(x, y, z, e); break;}
#endif
            default:
              {break;}
            }
          binsFilled.insert(globalBin);
        }
    }

    /// Full record of the current event histogram.
    Cells Read() const
    {
#ifdef TEST_4D
      if (nDimensions == 4)
        {return ReadCells4D(hist);}
#endif
      return ReadCells(hist);
    }

    void Reset()
    {
#ifdef TEST_4D
      if (nDimensions == 4)
        {static_cast<BDSBH4DBase*>(hist)->Reset_BDSBH4D(); return;}
#endif
      hist->Reset();
    }

    int                nDimensions;
    TH1*               hist;
    std::set<Int_t>    binsFilled;
    std::mt19937       rng;
    std::uniform_real_distribution<double> coord;
    std::uniform_real_distribution<double> weight;
    std::uniform_int_distribution<int>     nFills;
    std::uniform_real_distribution<double> uniform;
  };

  Cells ReadResult(TH1* h, int nDimensions)
  {
#ifdef TEST_4D
    if (nDimensions == 4)
      {return ReadCells4D(h);}
#else
    (void)nDimensions;
#endif
    return ReadCells(h);
  }

  double ResultEntries(TH1* h, int nDimensions)
  {
#ifdef TEST_4D
    if (nDimensions == 4)
      {return (double)static_cast<BDSBH4DBase*>(h)->GetEntries_BDSBH4D();}
#else
    (void)nDimensions;
#endif
    return h->GetEntries();
  }

  /// Unique result histogram names so ROOT doesn't complain about replacing objects.
  std::string UniqueName(const std::string& base)
  {
    static int counter = 0;
    return base + "_" + std::to_string(counter++);
  }

  std::string DimName(const std::string& testName, int nDimensions)
  {
    std::ostringstream ss;
    ss << testName << " " << nDimensions << "D";
    return ss.str();
  }

  //---------------------------------------------------------------------------
  // Tests
  //---------------------------------------------------------------------------

  /// HistogramAccumulator::Accumulate looping over all bins vs reference.
  void TestMean(int nDimensions, int nEvents)
  {
    std::string testName = DimName("HistogramAccumulator mean " + std::to_string(nEvents) + " events", nDimensions);
    EventGenerator gen(nDimensions, 1234 + nDimensions);
    HistogramAccumulator acc(gen.hist, nDimensions, UniqueName("mean"), "mean");
    std::vector<std::vector<double>> record;
    for (int i = 0; i < nEvents; i++)
      {
        gen.Next();
        record.push_back(gen.Read().content);
        acc.Accumulate(gen.hist);
      }
    Check(acc.N() == (unsigned long)nEvents, testName, "N() == number of events");
    TH1* result = acc.Terminate();
    CompareCells(ReadResult(result, nDimensions), ReferenceMean(record), testName);
    Check(Close(ResultEntries(result, nDimensions), (double)nEvents), testName,
          "entries of result == number of events");
  }

  /// HistogramAccumulatorFast accumulating only filled bins vs reference and
  /// vs HistogramAccumulator for the same events.
  void TestFast(int nDimensions, int nEvents, bool inRangeOnly = false)
  {
    std::string testName = DimName("HistogramAccumulatorFast " + std::to_string(nEvents) + " events"
                                   + (inRangeOnly ? " in range" : ""), nDimensions);
    EventGenerator gen(nDimensions, 4321 + nDimensions);
    HistogramAccumulatorFast acc(gen.hist, nDimensions, UniqueName("fast"), "fast");
    HistogramAccumulator     accSlow(gen.hist, nDimensions, UniqueName("slow"), "slow");
    std::vector<std::vector<double>> record;
    for (int i = 0; i < nEvents; i++)
      {
        gen.Next(true, inRangeOnly);
        record.push_back(gen.Read().content);
        acc.AccumulateBinsThatWereFilledOnly(gen.hist, gen.binsFilled);
        accSlow.Accumulate(gen.hist);
      }
    Check(acc.N() == (unsigned long)nEvents, testName, "N() == number of events");
    TH1* result = acc.Terminate();
    Cells fastCells = ReadResult(result, nDimensions);
    CompareCells(fastCells, ReferenceMean(record), testName + " vs reference");
    CompareCells(fastCells, ReadResult(accSlow.Terminate(), nDimensions), testName + " vs HistogramAccumulator");
    Check(Close(ResultEntries(result, nDimensions), (double)nEvents), testName,
          "entries of result == number of events");
  }

  /// A single entry should give the event contents and zero error (not nan).
  void TestSingleEntry(int nDimensions, bool inRangeOnly = false)
  {
    std::string testName = DimName("single entry", nDimensions);
    EventGenerator gen(nDimensions, 99 + nDimensions);
    gen.Next(false, inRangeOnly);
    Cells expected = gen.Read();
    std::fill(expected.error.begin(), expected.error.end(), 0);

    HistogramAccumulator acc(gen.hist, nDimensions, UniqueName("single"), "single");
    acc.Accumulate(gen.hist);
    CompareCells(ReadResult(acc.Terminate(), nDimensions), expected, testName + " HistogramAccumulator");

    HistogramAccumulatorFast accFast(gen.hist, nDimensions, UniqueName("singleFast"), "singleFast");
    accFast.AccumulateBinsThatWereFilledOnly(gen.hist, gen.binsFilled);
    CompareCells(ReadResult(accFast.Terminate(), nDimensions), expected, testName + " HistogramAccumulatorFast");
  }

  /// Bin indices outside the histogram must be ignored safely (no memory corruption).
  void TestFastOutOfRangeIndices(int nDimensions)
  {
    std::string testName = DimName("HistogramAccumulatorFast out of range indices", nDimensions);
    EventGenerator gen(nDimensions, 555 + nDimensions);
    HistogramAccumulatorFast acc(gen.hist, nDimensions, UniqueName("oor"), "oor");
    HistogramAccumulatorFast accRef(gen.hist, nDimensions, UniqueName("oorRef"), "oorRef");
    const int bigIndex = 1000000;
    for (int i = 0; i < 20; i++)
      {
        gen.Next();
        std::set<Int_t> withBad = gen.binsFilled;
        withBad.insert(-1);
        withBad.insert(-bigIndex);
        withBad.insert(bigIndex);
        acc.AccumulateBinsThatWereFilledOnly(gen.hist, withBad);
        accRef.AccumulateBinsThatWereFilledOnly(gen.hist, gen.binsFilled);
      }
    CompareCells(ReadResult(acc.Terminate(), nDimensions), ReadResult(accRef.Terminate(), nDimensions), testName);
  }

  /// Flush() then a second run must give the result of the second run only, as
  /// done for multiple runs in one bdsim execution.
  void TestFastMultipleRuns(int nDimensions, bool inRangeOnly = false)
  {
    std::string testName = DimName("HistogramAccumulatorFast Flush and reuse", nDimensions);
    EventGenerator gen(nDimensions, 777 + nDimensions);
    HistogramAccumulatorFast acc(gen.hist, nDimensions, UniqueName("runs"), "runs");
    std::vector<int> eventsPerRun = {13, 40, 7};
    for (std::size_t run = 0; run < eventsPerRun.size(); run++)
      {
        if (run > 0)
          {acc.Flush();}
        std::vector<std::vector<double>> record;
        for (int i = 0; i < eventsPerRun[run]; i++)
          {
            gen.Next(true, inRangeOnly);
            record.push_back(gen.Read().content);
            acc.AccumulateBinsThatWereFilledOnly(gen.hist, gen.binsFilled);
          }
        TH1* result = acc.Terminate();
        std::string runName = testName + " run " + std::to_string(run);
        CompareCells(ReadResult(result, nDimensions), ReferenceMean(record), runName);
        Check(Close(ResultEntries(result, nDimensions), (double)eventsPerRun[run]), runName,
              "entries of result == number of events in this run");
      }
  }

#ifdef TEST_4D
  /// Emulate the 4D scoring mesh path in BDSOutput::FillScorerHitsIndividual, where the
  /// hits are indexed by the scorer's mapper (which includes energy under/overflow bins),
  /// the 4D histogram is set with Set_BDSBH4D(x,y,z,e-1) and the bin filled is recorded
  /// with the 4D histogram's own global index.
  void TestFast4DScorerIndexing(int nEvents)
  {
    std::string testName = "HistogramAccumulatorFast 4D scorer indexing";
    const int nx = 3, ny = 3, nz = 2, ne = 4;
    EventGenerator gen(4, 888);
    BDSBH4DBase* h4 = static_cast<BDSBH4DBase*>(gen.hist);
    boost_histogram_linear_axis eAxis(ne, 0, 1);
    BDSHistBinMapper scorerMapper(nx, ny, nz, ne, &eAxis);

    HistogramAccumulatorFast acc(gen.hist, 4, UniqueName("scorer4D"), "scorer4D");
    std::mt19937 rng(888);
    std::uniform_int_distribution<int> ix(0, nx-1), iy(0, ny-1), iz(0, nz-1), ie(-1, ne); // energy incl. flow
    std::uniform_int_distribution<int> nHits(0, 6);
    std::uniform_real_distribution<double> value(0.1, 5);
    std::vector<std::vector<double>> record;
    for (int i = 0; i < nEvents; i++)
      {
        gen.Reset();
        std::set<Int_t> binsFilled;
        std::set<int>   scorerHits;
        int n = nHits(rng);
        for (int j = 0; j < n; j++)
          {
            int x = ix(rng), y = iy(rng), z = iz(rng), e = ie(rng);
            int scorerGlobal = scorerMapper.GlobalFromIJKLIndex(x, y, z, e + 1);
            if (scorerHits.count(scorerGlobal))
              {continue;} // a scorer has one entry per cell per event
            scorerHits.insert(scorerGlobal);
            h4->Set_BDSBH4D(x, y, z, e, value(rng));
            binsFilled.insert(h4->GlobalBin_BDSBH4D(x, y, z, e));
          }
        record.push_back(gen.Read().content);
        acc.AccumulateBinsThatWereFilledOnly(gen.hist, binsFilled);
      }
    CompareCells(ReadResult(acc.Terminate(), 4), ReferenceMean(record), testName);
  }
#endif

#ifdef TEST_REBDSIM_ACCUMULATORS
  /// HistogramAccumulatorSum should give the sum of contents and the errors
  /// combined in quadrature.
  void TestSum(int nDimensions, int nEvents)
  {
    std::string testName = DimName("HistogramAccumulatorSum", nDimensions);
    EventGenerator gen(nDimensions, 2468 + nDimensions);
    HistogramAccumulatorSum acc(gen.hist, nDimensions, UniqueName("sum"), "sum");
    Cells expected;
    for (int i = 0; i < nEvents; i++)
      {
        gen.Next();
        Cells ev = gen.Read();
        if (expected.content.empty())
          {
            expected.content.assign(ev.content.size(), 0);
            expected.error.assign(ev.error.size(), 0);
          }
        for (std::size_t c = 0; c < ev.content.size(); c++)
          {
            expected.content[c] += ev.content[c];
            expected.error[c]   += std::pow(ev.error[c], 2);
          }
        acc.Accumulate(gen.hist);
      }
    for (auto& v : expected.error)
      {v = std::sqrt(v);}
    CompareCells(ReadResult(acc.Terminate(), nDimensions), expected, testName);
  }

  /// Split events into samples, average each with HistogramAccumulator then combine
  /// those with HistogramAccumulatorMerge (as rebdsimHistoMerge / rebdsimCombine do).
  /// The result should be the same as averaging all events at once.
  void TestMerge(int nDimensions)
  {
    std::string testName = DimName("HistogramAccumulatorMerge", nDimensions);
    EventGenerator gen(nDimensions, 1357 + nDimensions);
    std::vector<int> eventsPerSample = {7, 1, 2, 13, 30, 47};
    HistogramAccumulatorMerge merge(gen.hist, nDimensions, UniqueName("merge"), "merge");
    std::vector<std::vector<double>> record;
    unsigned long nTotal = 0;
    for (int nInSample : eventsPerSample)
      {
        HistogramAccumulator sample(gen.hist, nDimensions, UniqueName("sample"), "sample");
        for (int i = 0; i < nInSample; i++)
          {
            gen.Next();
            record.push_back(gen.Read().content);
            sample.Accumulate(gen.hist);
          }
        merge.Accumulate(sample.Terminate());
        nTotal += (unsigned long)nInSample;
      }
    TH1* result = merge.Terminate();
    CompareCells(ReadResult(result, nDimensions), ReferenceMean(record), testName);
    Check(merge.N() == nTotal, testName, "N() == total number of events");
    Check(Close(ResultEntries(result, nDimensions), (double)nTotal), testName,
          "entries of result == total number of events");
  }
#endif
}

int main(int /*argc*/, char** /*argv*/)
{
  TH1::AddDirectory(kFALSE);

  for (int nDim = 1; nDim <= 3; nDim++)
    {
      TestMean(nDim, 1);
      TestMean(nDim, 2);
      TestMean(nDim, 200);
      TestFast(nDim, 2);
      TestFast(nDim, 200);
      TestSingleEntry(nDim);
      TestFastOutOfRangeIndices(nDim);
      TestFastMultipleRuns(nDim);
#ifdef TEST_REBDSIM_ACCUMULATORS
      TestSum(nDim, 50);
      TestMerge(nDim);
#endif
    }

#ifdef TEST_4D
  TestMean(4, 50);
  // only the dedicated tests fill the 4D under and overflow bins so other failures are distinct
  TestFast(4, 50, true);
  TestFast(4, 50, false);
  TestSingleEntry(4, true);
  TestFastOutOfRangeIndices(4);
  TestFastMultipleRuns(4, true);
  TestFast4DScorerIndexing(50);
#endif

  std::cout << (nChecks - nFailures) << " / " << nChecks << " checks passed" << std::endl;
  return nFailures == 0 ? 0 : 1;
}
