#include "AnaQuery/TriggerDecision.h"

AnaQ::TriggerDecision::TriggerDecision(const std::string& triggerSelection) : m_triggerSelection(triggerSelection) {}

void AnaQ::TriggerDecision::initialize(unsigned int slot, unsigned long long, unsigned long long) {
  m_trigCfgTool_handle = asg::AnaToolHandle<TrigConf::ITrigConfigTool>{"TrigConf::xAODConfigTool/xAODConfigTool_slot"+std::to_string(slot)};
  m_trigCfgTool_handle.retrieve().ignore();
  m_trigDecTool_handle = asg::AnaToolHandle<Trig::TrigDecisionTool>{"Trig::TrigDecisionTool/TrigDecisionTool_slot"+std::to_string(slot)};
  m_trigDecTool_handle.setProperty("ConfigTool", m_trigCfgTool_handle).ignore();
  m_trigDecTool_handle.setProperty( "TrigDecisionKey", "xTrigDecision" ).ignore();
  m_trigDecTool_handle.retrieve().ignore();
}

bool AnaQ::TriggerDecision::evaluate(qty::column::observable<xAOD::EventInfo>) const {
  // eventInfo.value();
  // if (m_trigCfgTool_handle->beginEvent() != EL::StatusCode::SUCCESS) {
  //   // std::cout << "failed" << std::endl;
  // }
  // if (m_trigDecTool_handle->beginEvent() != EL::StatusCode::SUCCESS) {
  //   // std::cout << "failed" << std::endl;
  // };
  // std::cout << m_triggerSelection << std::endl;
  auto printingTriggerChainGroup = m_trigDecTool_handle->getChainGroup(m_triggerSelection);
  std::vector<std::string> triggersUsed = printingTriggerChainGroup->getListOfTriggers();
  // for ( unsigned int iTrigger = 0; iTrigger < triggersUsed.size(); ++iTrigger ) {
  //   std::cout << triggersUsed.at(iTrigger).c_str();
  // }
  return m_trigDecTool_handle->getChainGroup(m_triggerSelection)->isPassed();
  // return m_trigDecTool_handle->isPassed(m_triggerSelection);
}

void AnaQ::TriggerDecision::finalize(unsigned int) {
  // m_trigDecTool_handle->finalize();
}