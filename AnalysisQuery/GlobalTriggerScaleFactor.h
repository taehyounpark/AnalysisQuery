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

#include "AnalysisQuery/EventHelpers.h"

class GlobalTriggerScaleFactor
  : public Column<double(ConstDataVector<xAOD::ElectronContainer>)>
{

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
  StatusCode applySystematicVariation(CP::SystematicSet const& sysSet);

protected:
  int m_outputLevel;

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

  // std::map<string,string> m_legsPerTool;

protected:
  mutable asg::AnaToolHandle<ITrigGlobalEfficiencyCorrectionTool>
    m_trigGlobalTool_handle; //!

  mutable std::vector<asg::AnaToolHandle<IAsgElectronEfficiencyCorrectionTool>>
    m_elecEffTools_handles; //!
  mutable std::vector<asg::AnaToolHandle<IAsgElectronEfficiencyCorrectionTool>>
    m_elecSFTools_handles; //!
  mutable ToolHandleArray<IAsgElectronEfficiencyCorrectionTool>
    m_elecEffTools_handleArray; //!
  mutable ToolHandleArray<IAsgElectronEfficiencyCorrectionTool>
    m_elecSFTools_handleArray; //!

};