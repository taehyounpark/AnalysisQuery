#pragma once

#include "AnaQuery/EventHelpers.h"

namespace AnaQ {

class ElectronGlobalScaleFactor
    : public Column<double(ROOT::RVec<double>, ROOT::RVec<double>,
                           ROOT::RVec<double>, ROOT::RVec<double>,
                           ROOT::RVec<double>)> {

public:
  ElectronGlobalScaleFactor() = default;
  ~ElectronGlobalScaleFactor() = default;

  double evaluate(Observable<ROOT::RVec<double>> recoSFs,
                  Observable<ROOT::RVec<double>> trigEffs,
                  Observable<ROOT::RVec<double>> trigSFs,
                  Observable<ROOT::RVec<double>> idSFs,
                  Observable<ROOT::RVec<double>> isoSFs) const override;

protected:
};

} // namespace AnaQ
