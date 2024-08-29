#include "EventFlow/ElectronScaleFactor.h"

#include "TDirectory.h"

ElectronScaleFactor::ElectronScaleFactor(
    ElectronScaleFactorConfig const &sfCfg)
    : EventFlow::Column<ROOT::RVec<double>(ConstDataVector<xAOD::ElectronContainer>),
                ElectronScaleFactorConfig>(sfCfg) {}

ElectronScaleFactor::ElectronScaleFactor(
        ElectronScaleFactorConfig const &sfCfg, CP::SystematicVariation const &sysVar)
    : EventFlow::Column<ROOT::RVec<double>(ConstDataVector<xAOD::ElectronContainer>),
                ElectronScaleFactorConfig>(sfCfg, sysVar) {}

void ElectronScaleFactor::initialize(unsigned int slot,
                                              unsigned long long,
                                              unsigned long long) {
  if (!m_elEffCorrTool_handle.isUserConfigured()) {
    m_elEffCorrTool_handle.setTypeAndName(
        "AsgElectronEfficiencyCorrectionTool/" + m_toolName + "_" +
        std::to_string(slot));
    m_elEffCorrTool_handle.setProperty("ForceDataType", cfg()->simulationFlavour)
        .ignore();
    m_elEffCorrTool_handle.setProperty("CorrelationModel", cfg()->correlationModel)
        .ignore();
    // m_elEffCorrTool_handle->setProperty("IdKey", m_idWorkingPoint).ignore();
    // m_elEffCorrTool_handle->setProperty("IsoKey",
    // m_isoWorkingPoint).ignore();
    // m_elEffCorrTool_handle->setProperty("RecoKey",
    // m_recoWorkingPoint).ignore();
    // m_elEffCorrTool_handle->setProperty("TriggerKey",
    // m_trigWorkingPoint).ignore();
    m_elEffCorrTool_handle
        .setProperty("CorrectionFileNameList", cfg()->correctionFileNameList)
        .ignore();
    m_elEffCorrTool_handle.retrieve().ignore();
  }
}

ROOT::RVec<double> ElectronScaleFactor::evaluate(
    qty::column::observable<ConstDataVector<xAOD::ElectronContainer>> electrons)
    const {
  if (m_elEffCorrTool_handle->applySystematicVariation(m_sysSet) !=
      StatusCode::SUCCESS) {
    throw std::runtime_error(
        "Failed to apply systematic variation '" + m_sysSet.name() +
        "' on electron efficiency correction tool");
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
          "Problem in electron efficiency correction tool");
    } else if (status == CP::CorrectionCode::OutOfValidityRange) {
      // ANA_MSG_DEBUG( "Electron of of PID efficiency validity range");
    }
    effSFs.push_back(effSF);
  }
  return effSFs;
}

void ElectronScaleFactor::finalize(unsigned int) {}