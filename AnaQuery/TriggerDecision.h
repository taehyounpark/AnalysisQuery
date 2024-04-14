#pragma once

#include "AnaQuery/EventHelpers.h"

#include "AsgTools/AnaToolHandle.h"

#include "TrigConfInterfaces/ITrigConfigTool.h"
#include "TrigDecisionTool/TrigDecisionTool.h"

#include "PATInterfaces/SystematicRegistry.h"
#include "PATInterfaces/SystematicSet.h"
#include "PATInterfaces/SystematicsUtil.h"
#include "PATInterfaces/SystematicVariation.h"

#include "xAODCore/ShallowCopy.h"
#include "xAODEventInfo/EventInfo.h"

#include "queryosity.h"

class TriggerDecision : public EventDecision<bool> {

public:
  TriggerDecision(const std::string& triggerSelection);
  ~TriggerDecision() = default;

  bool evaluate(qty::column::observable<xAOD::EventInfo> evntInfo) const override;

protected:
  asg::AnaToolHandle<TrigConf::ITrigConfigTool> m_trigConfTool_handle{"TrigConf::xAODConfigTool/xAODConfigTool",nullptr}; //!
  asg::AnaToolHandle<Trig::TrigDecisionTool>    m_trigDecTool_handle{"Trig::TrigDecisionTool/TrigDecisionTool"}; //!

};