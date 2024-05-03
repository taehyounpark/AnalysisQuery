#pragma once

#include "AnaQuery/EventHelpers.h"

// EDM
#include "xAODEgamma/ElectronContainer.h"
#include "xAODEventInfo/EventInfo.h"

// CP tools
#include "AsgTools/AnaToolHandle.h"
#include "EgammaAnalysisInterfaces/IAsgElectronEfficiencyCorrectionTool.h"

// CP interface
#include "PATInterfaces/SystematicRegistry.h"
#include "PATInterfaces/SystematicSet.h"
#include "PATInterfaces/SystematicVariation.h"
#include "PATInterfaces/SystematicsUtil.h"

#include "queryosity.h"

namespace AnaQ {

class ElectronEfficiencyCorrection
  : public Column<ROOT::RVec<double>(ConstDataVector<xAOD::ElectronContainer>)>
{

public:
  ElectronEfficiencyCorrection(Json const& sfConfig);
  ElectronEfficiencyCorrection(Json const& sfConfig,
                               CP::SystematicVariation const& sysVar);
  ~ElectronEfficiencyCorrection() = default;

  void initialize(unsigned int,
                  unsigned long long,
                  unsigned long long) override;
  ROOT::RVec<double> evaluate(
    Observable<ConstDataVector<xAOD::ElectronContainer>>) const override;
  void finalize(unsigned int) override;

protected:
  std::string m_toolName;
  int m_simulationFlavour;
  std::string m_correlationModel;
  std::string m_idWorkingPoint;
  std::string m_isoWorkingPoint;
  std::string m_recoWorkingPoint;
  std::string m_workingPointId;
  std::string m_trigWorkingPoint;
  std::vector<std::string> m_correctionFileNameList;

  CP::SystematicSet m_sysSet;

  mutable asg::AnaToolHandle<IAsgElectronEfficiencyCorrectionTool>
    m_elEffCorrTool_handle; //!
};

}