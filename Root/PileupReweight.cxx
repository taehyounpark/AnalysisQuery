#include "AnaQuery/PileupReweight.h"

AnaQ::PileupReweight::PileupReweight(Settings const& prwCfg, std::pair<std::string,float> systVar) : m_systName(systVar.first), m_systVal(systVar.second) {
  m_configFiles = prwCfg["configFiles"].get<std::vector<std::string>>();
  m_lumiCalcFiles = prwCfg["lumiCalcFiles"].get<std::vector<std::string>>();
  m_periodAssignments = prwCfg.value("periodAssignments","");
  m_usePeriodConfig = prwCfg.value("usePeriodConfig","MC16");
  m_unrepresentedDataAction = prwCfg.value("unrepresentedDataAction",3);
}

void AnaQ::PileupReweight::initialize(unsigned int, unsigned long long, unsigned long long) {
  // resolve paths
  for (auto& filePath : m_lumiCalcFiles) {
    filePath = PathResolverFindCalibFile(filePath);
  }
  for (auto& filePath : m_configFiles) {
    filePath = PathResolverFindCalibFile(filePath);
  }
  // setup tool
  m_prwTool_handle = asg::AnaToolHandle<CP::IPileupReweightingTool>("CP::PileupReweightingTool/tool");
  m_prwTool_handle.setProperty("UsePeriodConfig",m_usePeriodConfig).ignore();
  if (!m_periodAssignments.empty()) m_prwTool_handle.setProperty("PeriodAssignments",m_periodAssignments).ignore();
  m_prwTool_handle.setProperty("LumiCalcFiles", m_lumiCalcFiles).ignore();
  m_prwTool_handle.setProperty("ConfigFiles", m_configFiles).ignore();
  m_prwTool_handle.setProperty("UnrepresentedDataAction", m_unrepresentedDataAction).ignore();
  // m_prwTool_handle.setProperty("IgnoreBadChannels", true).ignore();
  m_prwTool_handle.retrieve().ignore();
}

float AnaQ::PileupReweight::evaluate(AnaQ::Observable<xAOD::EventInfo> eventInfo) const {
  m_prwTool_handle->apply(*eventInfo).ignore();
  SG::AuxElement::Decorator<float> correctedAndScaledAvgMu("correctedScaled_averageInteractionsPerCrossing");
  SG::AuxElement::Decorator<float> correctedMu("corrected_actualInteractionsPerCrossing");
  SG::AuxElement::Decorator<float> correctedAndScaledMu("correctedScaled_actualInteractionsPerCrossing");
  correctedAndScaledAvgMu( *eventInfo ) = m_prwTool_handle->getCorrectedAverageInteractionsPerCrossing( *eventInfo, true );
  correctedMu( *eventInfo ) = m_prwTool_handle->getCorrectedActualInteractionsPerCrossing( *eventInfo );
  correctedAndScaledMu( *eventInfo ) = m_prwTool_handle->getCorrectedActualInteractionsPerCrossing( *eventInfo, true );
  return m_prwTool_handle->getCombinedWeight(*eventInfo);
}

void AnaQ::PileupReweight::finalize(unsigned int) {
}