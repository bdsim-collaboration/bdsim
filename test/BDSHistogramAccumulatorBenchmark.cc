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
 * Benchmark of HistogramAccumulatorFast versus HistogramAccumulator as a
 * function of the number of bins in a 2D histogram.
 *
 * For each histogram size, nEvents 'event' histograms are each filled with
 * nFillsPerEvent random values. Each event is given to both accumulators so they
 * see identical data. Only the accumulation (per event) and termination (once)
 * calls are timed - filling and resetting the event histogram is common to both
 * and excluded. The two results are compared bin by bin to make sure the
 * comparison is a valid one.
 *
 * Usage: BDSHistogramAccumulatorBenchmark (<nEvents>) (<nFillsPerEvent>)
 * Defaults: 200 events, 100 fills per event.
 *
 * Returns 0 if the results of both accumulators agree, 1 otherwise.
 */
#include "HistogramAccumulator.hh"
#include "HistogramAccumulatorFast.hh"

#include "TH1.h"
#include "TH2D.h"

#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdio>
#include <iostream>
#include <random>
#include <set>
#include <string>
#include <vector>

namespace
{
  typedef std::chrono::steady_clock Clock;

  double SecondsSince(const Clock::time_point& start)
  {return std::chrono::duration<double>(Clock::now() - start).count();}

  /// Largest relative difference in bin content or error between two histograms of the same binning.
  double MaxRelativeDifference(const TH1* a, const TH1* b)
  {
    double result = 0;
    for (Int_t i = 0; i < a->GetNcells(); i++)
      {
        const double pairs[2][2] = {{a->GetBinContent(i), b->GetBinContent(i)},
                                    {a->GetBinError(i),   b->GetBinError(i)}};
        for (const auto& p : pairs)
          {
            double scale = std::max(std::abs(p[0]), std::abs(p[1]));
            if (scale > 0)
              {result = std::max(result, std::abs(p[0] - p[1]) / scale);}
          }
      }
    return result;
  }
}

int main(int argc, char** argv)
{
  const int nEvents        = argc > 1 ? std::stoi(argv[1]) : 1000;
  const int nFillsPerEvent = argc > 2 ? std::stoi(argv[2]) : 10;
  if (nEvents < 2 || nFillsPerEvent < 1)
    {
      std::cerr << "usage: " << argv[0] << " (<nEvents> >= 2) (<nFillsPerEvent> >= 1)" << std::endl;
      return 1;
    }

  TH1::AddDirectory(false); // keep the clones made by the accumulators out of gDirectory

  // number of bins per axis -> nBins^2 bins in total
  const std::vector<int> nBinsPerAxis = {10, 32, 100, 316, 1000, 2000};
  const double relTolerance = 1e-10; // the order of floating point operations differs between them

  std::mt19937 rng(12345);
  // a narrow gaussian in a wider histogram so most bins are typically empty in an event
  std::normal_distribution<double> gaus(0, 1);

  std::cout << "2D histogram accumulation: " << nEvents << " events, "
            << nFillsPerEvent << " fills per event (times in seconds)\n" << std::endl;
  std::printf("%10s %12s | %12s %12s | %12s %12s | %10s %12s\n",
              "nBins", "nCells", "std accum", "std term", "fast accum", "fast term", "speed up", "max rel diff");

  bool allAgree = true;
  for (int nb : nBinsPerAxis)
    {
      TH2D eventHist("benchEvent", "benchEvent", nb, -5, 5, nb, -5, 5);
      HistogramAccumulator     standard(&eventHist, "benchStandard", "benchStandard");
      HistogramAccumulatorFast fast(&eventHist, "benchFast", "benchFast");

      double tStandardAccumulate = 0;
      double tFastAccumulate     = 0;
      std::set<Int_t> binsFilled;
      for (int e = 0; e < nEvents; e++)
        {
          eventHist.Reset();
          binsFilled.clear();
          for (int f = 0; f < nFillsPerEvent; f++)
            {
              double x = gaus(rng);
              double y = gaus(rng);
              Int_t bin = eventHist.Fill(x, y);
              if (bin < 0) // under / overflow can be reported as -1 by ROOT
                {bin = eventHist.FindBin(x, y);}
              binsFilled.insert(bin);
            }

          auto start = Clock::now();
          standard.Accumulate(&eventHist);
          tStandardAccumulate += SecondsSince(start);

          start = Clock::now();
          fast.AccumulateBinsThatWereFilledOnly(&eventHist, binsFilled);
          tFastAccumulate += SecondsSince(start);
        }

      auto start = Clock::now();
      TH1* standardResult = standard.Terminate();
      double tStandardTerminate = SecondsSince(start);

      start = Clock::now();
      TH1* fastResult = fast.Terminate();
      double tFastTerminate = SecondsSince(start);

      double maxRelDiff = MaxRelativeDifference(standardResult, fastResult);
      bool agree = maxRelDiff < relTolerance;
      allAgree = allAgree && agree;

      double tStandard = tStandardAccumulate + tStandardTerminate;
      double tFast     = tFastAccumulate + tFastTerminate;
      std::printf("%4d x %-4d %12d | %12.4g %12.4g | %12.4g %12.4g | %9.1fx %12.3g%s\n",
                  nb, nb, eventHist.GetNcells(),
                  tStandardAccumulate, tStandardTerminate,
                  tFastAccumulate, tFastTerminate,
                  tFast > 0 ? tStandard / tFast : 0,
                  maxRelDiff, agree ? "" : "  <- MISMATCH");

      // the result histograms are intentionally leaked by the accumulators
      delete standardResult;
      delete fastResult;
    }

  std::cout << "\nnCells includes the under and overflow bins. Speed up is the total (accumulate + terminate)." << std::endl;
  if (!allAgree)
    {std::cout << "ERROR: results of the two accumulators do not agree" << std::endl;}
  return allAgree ? 0 : 1;
}
