#pragma once

#include "AnaQuery/EventHelpers.h"

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

#include "queryosity.h"

namespace AnaQ {


class TriggerDecision : public EventDecision<bool> {

public:
  TriggerDecision(const std::string& triggerSelection);
  virtual ~TriggerDecision() = default;

  void initialize(unsigned int, unsigned long long ,unsigned long long) override;
  bool evaluate(qty::column::observable<xAOD::EventInfo> evntInfo) const override;
  void finalize(unsigned int) override;

protected:
  mutable asg::AnaToolHandle<TrigConf::ITrigConfigTool>  m_trigCfgTool_handle; //!
  mutable asg::AnaToolHandle<Trig::TrigDecisionTool> m_trigDecTool_handle; //!

  std::string m_triggerSelection;

};

}