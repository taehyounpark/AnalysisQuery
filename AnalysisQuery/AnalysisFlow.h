#pragma once

#include "TFile.h"

#include <queryosity.hpp>

using dataflow = qty::dataflow;
namespace multithread = qty::multithread;
namespace dataset = qty::dataset;
namespace column = qty::column;
namespace selection = qty::selection;
namespace query = qty::query;
namespace systematic = qty::systematic;

template <typename Data>
class AnalysisFlow {

public:
  AnalysisFlow() = default;
  ~AnalysisFlow() = default;

  virtual void analyze(dataflow& df, dataflow::input<Data>& ds) = 0;
  virtual void output(TFile& file) const = 0;

protected:

};