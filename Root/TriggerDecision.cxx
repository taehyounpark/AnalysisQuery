#include "AnaQuery/TriggerDecision.h"

AnaQ::TriggerDecision::TriggerDecision(const std::string& triggerSelection) : m_triggerSelection(triggerSelection) {}

void AnaQ::TriggerDecision::initialize(unsigned int, unsigned long long, unsigned long long) {
  m_configTool = std::make_unique<xAODConfigTool>("xAODConfigTool");
  m_configTool->initialize().ignore();
  m_decisionTool = std::make_unique<Trig::TrigDecisionTool>("TrigDecisionTool");
  m_decisionTool->setProperty("ConfigTool", m_configTool->name()).ignore();
  m_decisionTool->setProperty( "TrigDecisionKey", "xTrigDecision" ).ignore();
  // m_decisionTool->setProperty( "NavigationFormat", "TriggerElement" ).ignore();
  // m_decisionTool->setProperty( "HLTSummary", "HLTNav_Summary_AODSlimmed" ).ignore();
  m_decisionTool->initialize().ignore();
}

bool AnaQ::TriggerDecision::evaluate(qty::column::observable<xAOD::EventInfo> eventInfo) const {
  eventInfo.value();
  if (m_configTool->beginEvent() != EL::StatusCode::SUCCESS) {
    std::cout << "failed" << std::endl;
  }
  if (m_decisionTool->beginEvent() != EL::StatusCode::SUCCESS) {
    std::cout << "failed" << std::endl;
  };
  // std::cout << m_triggerSelection << std::endl;
  auto printingTriggerChainGroup = m_decisionTool->getChainGroup(m_triggerSelection);
  std::vector<std::string> triggersUsed = printingTriggerChainGroup->getListOfTriggers();
  for ( unsigned int iTrigger = 0; iTrigger < triggersUsed.size(); ++iTrigger ) {
    std::cout << triggersUsed.at(iTrigger).c_str();
  }
  // return m_decisionTool->getChainGroup(m_triggerSelection)->isPassed();
  return m_decisionTool->isPassed(m_triggerSelection);
}

void AnaQ::TriggerDecision::finalize(unsigned int) {
  // m_decisionTool->finalize();
}