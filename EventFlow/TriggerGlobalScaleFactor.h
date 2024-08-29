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

#include "EventFlow/EventHelpers.h"
#include "EventFlow/Action.h"

#include <string>
#include <vector>
#include <map>

struct TriggerGlobalScaleFactorConfig {
  int simulationFlavour;
  std::vector<std::string> triggerList;
  std::string correlationModel;
  std::vector<std::string> m_trigList_2015;
  std::vector<std::string> m_trigList_2016;
  std::vector<std::string> m_trigList_2017;
  std::vector<std::string> m_trigList_2018;
  std::vector<std::string> m_trigList_2022;
  std::vector<std::string> m_trigList_2023;
  std::vector<std::string> m_trigList_2024;
  std::vector<std::string> m_trigList_2025;
  std::vector<std::vector<std::string>> effCorrectionFileNameLists;
  std::vector<std::vector<std::string>> sfCorrectionFileNameLists;
};

class TriggerGlobalScaleFactor
  : public EventFlow::Column<double(ConstDataVector<xAOD::ElectronContainer>), TriggerGlobalScaleFactorConfig>
{

public:
  TriggerGlobalScaleFactor(TriggerGlobalScaleFactorConfig const& sfCfg,
                           CP::SystematicVariation const& sysVar = {});
  ~TriggerGlobalScaleFactor() = default;

  void initialize(unsigned int slot,
                  unsigned long long,
                  unsigned long long) override;
  double evaluate(qty::column::observable<ConstDataVector<xAOD::ElectronContainer>>) const override;
  void finalize(unsigned) override;

protected:
  StatusCode applySystematicVariation(CP::SystematicSet const& sysSet);

protected:
  std::vector<std::string> m_elecSF_toolNames;
  std::vector<std::string> m_elecEff_toolNames;

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