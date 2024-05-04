#include "AnaQuery/PileupReweight.h"

#include "TDirectory.h"

AnaQ::PileupReweight::PileupReweight(Json const &prwCfg,
                                     CP::SystematicVariation const &sysVar)
    : m_sysSet(std::vector<CP::SystematicVariation>{sysVar}) {
  m_outputLevel = prwCfg.value("outputLevel", 1);
  m_configFiles = prwCfg["configFiles"].get<std::vector<std::string>>();
  m_lumiCalcFiles = prwCfg["lumiCalcFiles"].get<std::vector<std::string>>();
  m_unrepresentedDataAction = prwCfg.value("unrepresentedDataAction", 3);
  m_usePeriodConfig = prwCfg.value("usePeriodConfig", "auto");
  m_periodAssignments = prwCfg.value("periodAssignments", "");
}

void AnaQ::PileupReweight::initialize(unsigned int slot, unsigned long long,
                                      unsigned long long) {
  // resolve paths
  for (auto &filePath : m_lumiCalcFiles) {
    filePath = PathResolverFindCalibFile(filePath);
  }
  for (auto &filePath : m_configFiles) {
    filePath = PathResolverFindCalibFile(filePath);
  }
  // setup tool
  if (!m_prwTool_handle.isUserConfigured()) {
    m_prwTool_handle = asg::AnaToolHandle<CP::IPileupReweightingTool>(
        "CP::PileupReweightingTool/PileupReweightingTool_" + m_sysSet.name() +
        "_" + std::to_string(slot));
    m_prwTool_handle.setProperty("OutputLevel", m_outputLevel).ignore();
    m_prwTool_handle.setProperty("LumiCalcFiles", m_lumiCalcFiles).ignore();
    m_prwTool_handle.setProperty("ConfigFiles", m_configFiles).ignore();
    m_prwTool_handle
        .setProperty("UnrepresentedDataAction", m_unrepresentedDataAction)
        .ignore();
    m_prwTool_handle.setProperty("UsePeriodConfig", m_usePeriodConfig).ignore();
    if (!m_periodAssignments.empty())
      m_prwTool_handle.setProperty("PeriodAssignments", m_periodAssignments)
          .ignore();
    m_prwTool_handle.retrieve().ignore();
    if (m_prwTool_handle->applySystematicVariation(m_sysSet) !=
        EL::StatusCode::SUCCESS) {
      throw std::runtime_error("Failed to apply systematic variation '" +
                              m_sysSet.name() + "' on pileup reweighting tool");
  }
  }
}

float AnaQ::PileupReweight::evaluate(
    AnaQ::Observable<xAOD::EventInfo> eventInfo) const {
  m_prwTool_handle->apply(*eventInfo).ignore();
  return m_prwTool_handle->getCombinedWeight(*eventInfo);
  // SG::AuxElement::Decorator<float>
  // correctedAndScaledAvgMu("correctedScaled_averageInteractionsPerCrossing");
  // SG::AuxElement::Decorator<float>
  // correctedMu("corrected_actualInteractionsPerCrossing");
  // SG::AuxElement::Decorator<float>
  // correctedAndScaledMu("correctedScaled_actualInteractionsPerCrossing");
  // correctedAndScaledAvgMu( *eventInfo ) =
  // m_prwTool_handle->getCorrectedAverageInteractionsPerCrossing( *eventInfo,
  // true ); correctedMu( *eventInfo ) =
  // m_prwTool_handle->getCorrectedActualInteractionsPerCrossing( *eventInfo );
  // correctedAndScaledMu( *eventInfo ) =
  // m_prwTool_handle->getCorrectedActualInteractionsPerCrossing( *eventInfo,
  // true );
}

void AnaQ::PileupReweight::finalize(unsigned int) {}