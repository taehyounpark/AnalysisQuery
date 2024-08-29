#pragma once

#include "AsgAnalysisInterfaces/IPileupReweightingTool.h"
#include "AsgTools/AnaToolHandle.h"

#include "EventHelpers.h"


struct PileupReweightingConfig {
  int outputLevel;
  std::vector<std::string> configFiles;
  std::vector<std::string> lumiCalcFiles;
  std::string usePeriodConfig = "auto";
  std::string periodAssignments;
  int unrepresentedDataAction;
};

class PileupReweighting : public EventDecision<float, PileupReweightingConfig> {

public:
  PileupReweighting( PileupReweightingConfig const& prwCfg, CP::SystematicVariation const& sysVar = {});
  ~PileupReweighting() = default;

  void initialize(unsigned int, unsigned long long ,unsigned long long) override;
  float evaluate(qty::column::observable<xAOD::EventInfo> eventInfo) const override;
  void finalize(unsigned int) override;

protected:
  mutable asg::AnaToolHandle<CP::IPileupReweightingTool> m_prwTool_handle; //!
  CP::SystematicSet m_sysSet;

};