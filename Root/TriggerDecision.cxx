#include "AnalysisQuery/TriggerDecision.h"

TriggerDecision::TriggerDecision(const std::string &triggerSelection)
    : m_triggerSelection(triggerSelection) {}

void TriggerDecision::initialize(unsigned int slot, unsigned long long,
                                       unsigned long long) {
  if (!m_trigDecTool_handle.isUserConfigured()) {
    m_trigCfgTool_handle = asg::AnaToolHandle<TrigConf::ITrigConfigTool>{
        "TrigConf::xAODConfigTool/xAODConfigTool_" + std::to_string(slot)};
    m_trigCfgTool_handle.retrieve().ignore();
    m_trigDecTool_handle = asg::AnaToolHandle<Trig::TrigDecisionTool>{
        "Trig::TrigDecisionTool/TrigDecisionTool_" + std::to_string(slot)};
    m_trigDecTool_handle.setProperty("ConfigTool", m_trigCfgTool_handle).ignore();
    m_trigDecTool_handle.setProperty("TrigDecisionKey", "xTrigDecision").ignore();
    m_trigDecTool_handle.setProperty("AcceptMultipleInstance", true).ignore();
    m_trigDecTool_handle.retrieve().ignore();
  }
}

bool TriggerDecision::evaluate(
    qty::column::observable<xAOD::EventInfo>) const {
  if (m_trigDecTool_handle->beginEvent() != StatusCode::SUCCESS) {
    throw std::runtime_error("Failure in trigger decision tool");
  }
  return m_trigDecTool_handle->getChainGroup(m_triggerSelection)->isPassed();
}

void TriggerDecision::finalize(unsigned int) {
  // m_trigDecTool_handle->finalize();
}