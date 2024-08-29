#pragma once

#include "EventData.h"

#include <queryosity.hpp>

class TFile;

namespace EventFlow {

template <typename Data>
class Analysis {

public:
  Analysis() = default;
  virtual ~Analysis() = default;

  virtual void analyze(qty::dataflow& df, qty::dataflow::input<Data>& ds) = 0;
  virtual void output(TFile* file) const;

};

}

#include "TFile.h"

template <typename Data>
void EventFlow::Analysis<Data>::output(TFile*) const {}

