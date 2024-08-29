#pragma once

#include "EventFlow/EventHelpers.h"

// Trigger include(s).
#include "TrigConfxAOD/xAODConfigTool.h"
#include "TrigDecisionTool/TrigDecisionTool.h"
#include "AsgTools/AnaToolHandle.h"

#include "PATInterfaces/SystematicRegistry.h"
#include "PATInterfaces/SystematicSet.h"
#include "PATInterfaces/SystematicsUtil.h"
#include "PATInterfaces/SystematicVariation.h"

#include "xAODCore/ShallowCopy.h"
#include "xAODEventInfo/EventInfo.h"


#include <queryosity.hpp>

struct TriggerDecisionConfig {
  std::vector<std::string> triggerList;
};

class TriggerDecision : public EventDecision<bool, TriggerDecisionConfig> {

public:
  TriggerDecision(TriggerDecisionConfig const& trigDecCfg);
  virtual ~TriggerDecision() = default;

  void initialize(unsigned int, unsigned long long ,unsigned long long) override;
  bool evaluate(qty::column::observable<xAOD::EventInfo> evntInfo) const override;
  void finalize(unsigned int) override;

protected:
  mutable asg::AnaToolHandle<TrigConf::ITrigConfigTool>  m_trigDecCfgTool_handle; //!
  mutable asg::AnaToolHandle<Trig::TrigDecisionTool> m_trigDecTool_handle; //!
};