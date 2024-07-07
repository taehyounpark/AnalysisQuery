#pragma once

#include "TFile.h"

#include <queryosity.hpp>

template <typename Data>
class DataAnalysis {

public:
  DataAnalysis() = default;
  virtual ~DataAnalysis() = default;

  virtual void analyze(qty::dataflow& df, qty::dataflow::input<Data>& ds) = 0;
  virtual void output(TFile* file) const = 0;

};