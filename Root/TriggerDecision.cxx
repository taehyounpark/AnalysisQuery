#include "EventFlow/TriggerDecision.h"

TriggerDecision::TriggerDecision(TriggerDecisionConfig const &trigDecCfg)
    : EventDecision<bool, TriggerDecisionConfig>(trigDecCfg) {}

void TriggerDecision::initialize(unsigned int slot, unsigned long long,
                                 unsigned long long) {
  if (!m_trigDecTool_handle.isUserConfigured()) {
    m_trigDecCfgTool_handle = asg::AnaToolHandle<TrigConf::ITrigConfigTool>{
        "TrigConf::xAODConfigTool/xAODConfigTool_" + std::to_string(slot)};
    m_trigDecCfgTool_handle.retrieve().ignore();
    m_trigDecTool_handle = asg::AnaToolHandle<Trig::TrigDecisionTool>{
        "Trig::TrigDecisionTool/TrigDecisionTool_" + std::to_string(slot)};
    m_trigDecTool_handle.setProperty("ConfigTool", m_trigDecCfgTool_handle)
        .ignore();
    m_trigDecTool_handle.setProperty("TrigDecisionKey", "xTrigDecision")
        .ignore();
    m_trigDecTool_handle.setProperty("AcceptMultipleInstance", true).ignore();
    m_trigDecTool_handle.retrieve().ignore();
  }
}

bool TriggerDecision::evaluate(qty::column::observable<xAOD::EventInfo>) const {
  if (m_trigDecTool_handle->beginEvent() != StatusCode::SUCCESS) {
    throw std::runtime_error("Failure in trigger decision tool");
  }
  for (auto const &trig : cfg()->triggerList) {
    if (m_trigDecTool_handle->getChainGroup(trig)->isPassed()) {
      return true;
    }
  }
  return false;
}

void TriggerDecision::finalize(unsigned int) {
  // m_trigDecTool_handle->finalize();
}