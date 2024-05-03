#pragma once

// EDM
#include "xAODEgamma/ElectronContainer.h"
#include "xAODEventInfo/EventInfo.h"
#include "xAODMuon/MuonContainer.h"

// CP tools
#include "AsgTools/AnaToolHandle.h"
#include "EgammaAnalysisInterfaces/IAsgElectronEfficiencyCorrectionTool.h"
#include "MuonAnalysisInterfaces/IMuonTriggerScaleFactors.h"
#include "TriggerAnalysisInterfaces/ITrigGlobalEfficiencyCorrectionTool.h"

// CP interface
#include "PATCore/PATCoreEnums.h"
#include "PATInterfaces/SystematicRegistry.h"
#include "PATInterfaces/SystematicSet.h"
#include "PATInterfaces/SystematicVariation.h"
#include "PATInterfaces/SystematicsUtil.h"

#include "AnaQuery/EventHelpers.h"

namespace AnaQ {

class GlobalTriggerScaleFactor
  : public Column<double(ConstDataVector<xAOD::ElectronContainer>)>
{

private:
  static unsigned s_nInstances;

public:
  GlobalTriggerScaleFactor(nlohmann::json const& sfCfg,
                           CP::SystematicVariation const& sysVar = {});
  ~GlobalTriggerScaleFactor() = default;

  void initialize(unsigned int slot,
                  unsigned long long,
                  unsigned long long) override;
  double evaluate(Observable<ConstDataVector<xAOD::ElectronContainer>>) const override;
  void finalize(unsigned) override;

protected:
  bool applySystematicVariation(CP::SystematicSet const& sysSet);

protected:
  mutable asg::AnaToolHandle<ITrigGlobalEfficiencyCorrectionTool>
    m_trigGlobalTool_handle;
  int m_outputLevel;

  mutable std::vector<asg::AnaToolHandle<IAsgElectronEfficiencyCorrectionTool>>
    m_elecEffTools_handles;
  mutable std::vector<asg::AnaToolHandle<IAsgElectronEfficiencyCorrectionTool>>
    m_elecSFTools_handles;
  mutable ToolHandleArray<IAsgElectronEfficiencyCorrectionTool>
    m_elecEffTools_handleArray;
  mutable ToolHandleArray<IAsgElectronEfficiencyCorrectionTool>
    m_elecSFTools_handleArray;

  // std::map<string,string> m_legsPerTool;

  CP::SystematicSet m_sysSet;

  std::vector<std::string> m_elecSF_toolNames;
  std::vector<std::string> m_elecEff_toolNames;

  std::string m_elecCorrelationModel;
  int m_elecSimulationFlavour;
  std::vector<std::string> m_elecTriggers;
  std::map<std::string, std::vector<std::string>>
    m_elecEffCorrectionFileNameList;
  std::map<std::string, std::vector<std::string>>
    m_elecSFCorrectionFileNameList;

  unsigned int m_index;
};

} // namespace AnaQ

inline unsigned AnaQ::GlobalTriggerScaleFactor::s_nInstances = 0;