#include "AnaQuery/ElectronGlobalScaleFactor.h"

double AnaQ::ElectronGlobalScaleFactor::evaluate(
    AnaQ::Observable<ROOT::RVec<double>> recoSFs,
    AnaQ::Observable<ROOT::RVec<double>> trigEffs,
    AnaQ::Observable<ROOT::RVec<double>> trigSFs,
    AnaQ::Observable<ROOT::RVec<double>> idSFs,
    AnaQ::Observable<ROOT::RVec<double>> isoSFs) const {

  if (trigSFs->size() != trigEffs->size()) {
    throw std::runtime_error("incompatible trigger efficiencies and SFs");
  }

  auto globalSF = 1.0;

  // reconstruction efficiencies
  globalSF *= ROOT::VecOps::Product(*recoSFs);

  // trigger efficiencies & SFs
  const auto globalTrigIneff = ROOT::VecOps::Product(1.0 - *trigEffs);
  const auto globalTrigIneffScaled = ROOT::VecOps::Product(1.0 - (*trigEffs)*(*trigSFs));
  globalSF *= (1.0 - globalTrigIneffScaled) / (1.0 - globalTrigIneff);

  globalSF *= ROOT::VecOps::Product(*idSFs);
  globalSF *= ROOT::VecOps::Product(*isoSFs);

  return globalSF;
}