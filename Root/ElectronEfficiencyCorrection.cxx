#include "AnaQuery/ElectronEfficiencyCorrection.h"

AnaQ::ElectronEfficiencyCorrection::ElectronEfficiencyCorrection(Settings sfConfig, SystematicMode const& systMode) :
  m_systMode(systMode) {
  m_toolName = sfConfig.value("toolName","ElectronEfficiencyCorrectionTool");
  m_simulationFlavour = sfConfig.value("simulationFlavour", 1);
  m_workingPointPID = sfConfig.value("workingPointPID", "");
  m_workingPointIso = sfConfig.value("workingPointIso", "");
  m_workingPointReco = sfConfig.value("workingPointReco", "");
  m_workingPointTrig = sfConfig.value("workingPointTrig", "");
  if (!m_systMode.name.empty()) {
    m_toolName += ("_"+m_systMode.name+"_"+std::to_string(m_systMode.value));
  }
  if (!m_workingPointPID.empty()) {
    m_toolName += ("_pid_" + m_workingPointPID);
  }
  if (!m_workingPointIso.empty()) {
    m_toolName += ("_iso_" + m_workingPointIso);
  }
  if (!m_workingPointReco.empty()) {
    m_toolName += ("_reco_" + m_workingPointTrig);
  }
  if (!m_workingPointTrig.empty()) {
    m_toolName += ("_trig_" + m_workingPointTrig);
  }
}

void AnaQ::ElectronEfficiencyCorrection::initialize(unsigned int slot, unsigned long long, unsigned long long) {
  m_toolName += ("_"+std::to_string(slot));
  if (!m_elEffCorrTool) {
    m_elEffCorrTool = std::make_unique<AsgElectronEfficiencyCorrectionTool>(m_toolName+"_slot"+std::to_string(slot));
    m_elEffCorrTool->setProperty("ForceDataType",m_simulationFlavour).ignore();
    m_elEffCorrTool->setProperty("CorrelationModel",m_correlationModel).ignore();
    m_elEffCorrTool->setProperty("IdKey", m_workingPointPID).ignore();
    m_elEffCorrTool->setProperty("IsoKey", m_workingPointIso).ignore();
    m_elEffCorrTool->setProperty("RecoKey", m_workingPointReco).ignore();
    m_elEffCorrTool->setProperty("TriggerKey", m_workingPointTrig).ignore();
  }
  m_elEffCorrTool->initialize().ignore();
}

ROOT::RVec<double> AnaQ::ElectronEfficiencyCorrection::evaluate(AnaQ::Observable<ConstDataVector<xAOD::ElectronContainer>> electrons) const {
  // the the affected systematic variation (if applicable)
  auto systVar = m_systMode.from(m_elEffCorrTool->recommendedSystematics());
  auto status = m_elEffCorrTool->applySystematicVariation(systVar);
  if ( status != EL::StatusCode::SUCCESS ) {
    throw std::runtime_error("Failed to apply systematic variation '"+systVar.name()+"' on electron efficiency SF tool '"+m_toolName+"'");
  }
  ROOT::RVec<double> effSFs;
  effSFs.reserve(electrons->size());
  for ( auto elItr : *(electrons) ) {
    double effSF = -1.0;
    status = m_elEffCorrTool->getEfficiencyScaleFactor( *elItr, effSF );
    if ( status == CP::CorrectionCode::Error ) {
      // ANA_MSG_ERROR( "Problem in PID getEfficiencyScaleFactor Tool");
      // return EL::StatusCode::FAILURE;
      throw std::runtime_error("Problem in electron efficiency SF tool '"+m_toolName+"'");
    } else if ( status == CP::CorrectionCode::OutOfValidityRange ) {
      // ANA_MSG_DEBUG( "Electron of of PID efficiency validity range");
    }
    effSFs.push_back(effSF);
  }
  return effSFs;
}

void AnaQ::ElectronEfficiencyCorrection::finalize(unsigned int) {
  m_elEffCorrTool.reset();
}