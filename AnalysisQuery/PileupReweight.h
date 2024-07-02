#pragma once

#include "EventHelpers.h"

#include "AsgAnalysisInterfaces/IPileupReweightingTool.h"

#include "AsgTools/AnaToolHandle.h"

class PileupReweight : public EventDecision<float> {

public:
  PileupReweight(Json const& prwCfg, CP::SystematicVariation const& sysVar = {});
  ~PileupReweight() = default;

  void initialize(unsigned int, unsigned long long ,unsigned long long) override;
  float evaluate(Observable<xAOD::EventInfo> eventInfo) const override;
  void finalize(unsigned int) override;

protected:
  int m_outputLevel;
  std::vector<std::string> m_configFiles;
  std::vector<std::string> m_lumiCalcFiles;
  std::string m_usePeriodConfig;
  std::string m_periodAssignments;
  int m_unrepresentedDataAction;

  mutable asg::AnaToolHandle<CP::IPileupReweightingTool> m_prwTool_handle; //!

  CP::SystematicSet m_sysSet;

};