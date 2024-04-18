#pragma once

#include "AnaQuery/EventHelpers.h"

// EDM include(s):
#include "xAODEgamma/ElectronContainer.h"

// CP interface includes
#include "PATInterfaces/SystematicsUtil.h"
#include "PATInterfaces/SystematicRegistry.h"
#include "PATInterfaces/SystematicSet.h"
#include "PATInterfaces/SystematicVariation.h"

// external tools include(s):
#include "ElectronEfficiencyCorrection/AsgElectronEfficiencyCorrectionTool.h"

#include <ROOT/RVec.hxx>

#include "queryosity.h"

namespace AnaQ {

class ElectronEfficiencyCorrection: public Column<ROOT::RVec<double>(ConstDataVector<xAOD::ElectronContainer>)> {

public:
  ElectronEfficiencyCorrection(Settings sfConfig, SystematicMode const& systMode = SystematicMode("",0.0));
  ~ElectronEfficiencyCorrection() = default;

  void initialize(unsigned int, unsigned long long, unsigned long long) override;
  ROOT::RVec<double> evaluate(Observable<ConstDataVector<xAOD::ElectronContainer>>) const override;
  void finalize(unsigned int) override;

protected:
  std::string m_toolName;
  int m_simulationFlavour;
  std::string m_correlationModel;
  std::string m_workingPointPID;
  std::string m_workingPointIso;
  std::string m_workingPointReco;
  std::string m_workingPointId;
  std::string m_workingPointTrig;

  SystematicMode m_systMode;

  std::unique_ptr<AsgElectronEfficiencyCorrectionTool> m_elEffCorrTool;  //!
};

}
