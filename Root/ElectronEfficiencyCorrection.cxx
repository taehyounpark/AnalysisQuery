#include "AnalysisQuery/ElectronEfficiencyCorrection.h"

#include "TDirectory.h"

ElectronEfficiencyCorrection::ElectronEfficiencyCorrection(Json const &sfConfig)
    : m_sysSet({}) {
  m_toolName = sfConfig.value("toolName", "ElectronEfficiencyCorrectionTool");
  m_simulationFlavour = sfConfig.value("simulationFlavour", 1);
  m_idWorkingPoint = sfConfig.value("idWorkingPoint", "");
  m_isoWorkingPoint = sfConfig.value("isoWorkingPoint", "");
  m_recoWorkingPoint = sfConfig.value("recoWorkingPoint", "");
  m_trigWorkingPoint = sfConfig.value("trigWorkingPoint", "");
  m_correlationModel = sfConfig.value("correlationModel", "TOTAL");
  m_correctionFileNameList =
      sfConfig.value("correctionFileNameList", std::vector<std::string>());
  for (auto &correctionFileName : m_correctionFileNameList) {
    correctionFileName = PathResolverFindCalibFile(correctionFileName);
  }
  if (!m_idWorkingPoint.empty()) {
    m_toolName += ("_ID_" + m_idWorkingPoint);
  }
  if (!m_isoWorkingPoint.empty()) {
    m_toolName += ("_Iso_" + m_isoWorkingPoint);
  }
  if (!m_recoWorkingPoint.empty()) {
    m_toolName += ("_Reco_" + m_trigWorkingPoint);
  }
  if (!m_trigWorkingPoint.empty()) {
    m_toolName += ("_Trigger_" + m_trigWorkingPoint);
  }
}

ElectronEfficiencyCorrection::ElectronEfficiencyCorrection(
    Json const &sfConfig, CP::SystematicVariation const &sysVar)
    : ElectronEfficiencyCorrection(sfConfig) {
  m_sysSet = std::vector<CP::SystematicVariation>{sysVar};
  m_toolName += ("_" + m_sysSet.name());
}

void ElectronEfficiencyCorrection::initialize(unsigned int slot,
                                              unsigned long long,
                                              unsigned long long) {
  if (!m_elEffCorrTool_handle.isUserConfigured()) {
    m_elEffCorrTool_handle.setTypeAndName(
        "AsgElectronEfficiencyCorrectionTool/" + m_toolName + "_" +
        std::to_string(slot));
    m_elEffCorrTool_handle.setProperty("ForceDataType", m_simulationFlavour)
        .ignore();
    m_elEffCorrTool_handle.setProperty("CorrelationModel", m_correlationModel)
        .ignore();
    // m_elEffCorrTool_handle->setProperty("IdKey", m_idWorkingPoint).ignore();
    // m_elEffCorrTool_handle->setProperty("IsoKey",
    // m_isoWorkingPoint).ignore();
    // m_elEffCorrTool_handle->setProperty("RecoKey",
    // m_recoWorkingPoint).ignore();
    // m_elEffCorrTool_handle->setProperty("TriggerKey",
    // m_trigWorkingPoint).ignore();
    m_elEffCorrTool_handle
        .setProperty("CorrectionFileNameList", m_correctionFileNameList)
        .ignore();
    m_elEffCorrTool_handle.retrieve().ignore();
  }
}

ROOT::RVec<double> ElectronEfficiencyCorrection::evaluate(
    qty::column::observable<ConstDataVector<xAOD::ElectronContainer>> electrons) const {
  if (m_elEffCorrTool_handle->applySystematicVariation(m_sysSet) !=
      StatusCode::SUCCESS) {
    throw std::runtime_error(
        "Failed to apply systematic variation '" + m_sysSet.name() +
        "' on electron efficiency correction tool '" + m_toolName + "'");
  }

  ROOT::RVec<double> effSFs;
  effSFs.reserve(electrons->size());
  for (auto elItr : *(electrons)) {
    double effSF = -1.0;
    auto status =
        m_elEffCorrTool_handle->getEfficiencyScaleFactor(*elItr, effSF);
    if (status == CP::CorrectionCode::Error) {
      // ANA_MSG_ERROR( "Problem in PID getEfficiencyScaleFactor Tool");
      // return StatusCode::FAILURE;
      throw std::runtime_error(
          "Problem in electron efficiency correction tool '" + m_toolName +
          "'");
    } else if (status == CP::CorrectionCode::OutOfValidityRange) {
      // ANA_MSG_DEBUG( "Electron of of PID efficiency validity range");
    }
    effSFs.push_back(effSF);
  }
  return effSFs;
}

void ElectronEfficiencyCorrection::finalize(unsigned int) {}