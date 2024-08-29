#include "EventFlow/PileupReweighting.h"

#include "TDirectory.h"

PileupReweighting::PileupReweighting(PileupReweightingConfig const &prwCfg,
                                     CP::SystematicVariation const &sysVar)
    : EventDecision<float, PileupReweightingConfig>(prwCfg, sysVar) {}

void PileupReweighting::initialize(unsigned int slot, unsigned long long,
                                   unsigned long long) {
  // setup tool
  if (!m_prwTool_handle.isUserConfigured()) {
    m_prwTool_handle = asg::AnaToolHandle<CP::IPileupReweightingTool>(
        "CP::PileupReweightingTool/PileupReweightingTool_" +
        this->m_sysSet.name() + "_" + std::to_string(slot));
    m_prwTool_handle.setProperty("OutputLevel", this->cfg()->outputLevel).ignore();
    m_prwTool_handle.setProperty("LumiCalcFiles", this->cfg()->lumiCalcFiles)
        .ignore();
    m_prwTool_handle.setProperty("ConfigFiles", this->cfg()->configFiles).ignore();
    m_prwTool_handle
        .setProperty("UnrepresentedDataAction", this->cfg()->unrepresentedDataAction)
        .ignore();
    m_prwTool_handle.setProperty("UsePeriodConfig", this->cfg()->usePeriodConfig)
        .ignore();
    if (!this->cfg()->periodAssignments.empty())
      m_prwTool_handle
          .setProperty("PeriodAssignments", this->cfg()->periodAssignments)
          .ignore();
    m_prwTool_handle.retrieve().ignore();
    if (m_prwTool_handle->applySystematicVariation(this->m_sysSet) !=
        StatusCode::SUCCESS) {
      throw std::runtime_error("Failed to apply systematic variation '" +
                               this->m_sysSet.name() +
                               "' on pileup reweighting tool");
    }
  }
}

float PileupReweighting::evaluate(
    qty::column::observable<xAOD::EventInfo> eventInfo) const {
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

void PileupReweighting::finalize(unsigned int) {}