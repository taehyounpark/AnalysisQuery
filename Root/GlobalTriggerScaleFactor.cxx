#include "AnalysisQuery/GlobalTriggerScaleFactor.h"

GlobalTriggerScaleFactor::GlobalTriggerScaleFactor(
    nlohmann::json const &sfCfg, CP::SystematicVariation const &sysVar)
    : m_sysSet(std::vector<CP::SystematicVariation>{sysVar}) {
  auto const &elecCfg = sfCfg["ElectronEfficiencyCorrection"];
  m_elecCorrelationModel = elecCfg.value("correlationModel", "TOTAL");
  m_elecSimulationFlavour = elecCfg.value("simulationFlavour", 1);
  for (const auto &[elecTrig, elecTrigCfg] : elecCfg["triggers"].items()) {
    m_elecTriggers.push_back(elecTrig);
    m_elecEffCorrectionFileNameList[elecTrig] =
        elecTrigCfg["efficiency"].get<std::vector<std::string>>();
    m_elecSFCorrectionFileNameList[elecTrig] =
        elecTrigCfg["efficiencySF"].get<std::vector<std::string>>();
  }
  m_outputLevel =
      sfCfg["TrigGlobalEfficiencyCorrection"].value("outputLevel", 1);
}

void GlobalTriggerScaleFactor::initialize(unsigned int slot,
                                                unsigned long long,
                                                unsigned long long) {

  // electron efficiency correction tools
  const auto nElecTriggers = m_elecTriggers.size();
  // m_elecEffTools_handles.resize(nElecTriggers);
  // m_elecSFTools_handles.resize(nElecTriggers);
  // m_elecEffTools_handleArray.resize(nElecTriggers);
  // m_elecSFTools_handleArray.resize(nElecTriggers);
  for (unsigned itrig = 0; itrig < nElecTriggers; ++itrig) {
    auto const &elecTrig = m_elecTriggers[itrig];

    auto &elecEffTool_handle = m_elecEffTools_handles[itrig];
    if (!elecEffTool_handle.isUserConfigured()) {
      elecEffTool_handle.setTypeAndName(
          "AsgElectronEfficiencyCorrectionTool/"
          "ElectronEfficiencyCorrection_efficiency_" +
          elecTrig + "_" + m_sysSet.name() + "_" + std::to_string(slot));
      elecEffTool_handle
          .setProperty("CorrectionFileNameList",
                       m_elecEffCorrectionFileNameList[elecTrig])
          .ignore();
      elecEffTool_handle.setProperty("CorrelationModel", m_elecCorrelationModel)
          .ignore();
      elecEffTool_handle.retrieve().ignore();
    }

    auto &elecSFTool_handle = m_elecSFTools_handles[itrig];
    if (!elecSFTool_handle.isUserConfigured()) {
      elecSFTool_handle.setTypeAndName(
          "AsgElectronEfficiencyCorrectionTool/"
          "ElectronEfficiencyCorrection_efficiencySF_" +
          elecTrig + "_" + m_sysSet.name() + "_" + std::to_string(slot));
      elecSFTool_handle
          .setProperty("CorrectionFileNameList",
                       m_elecSFCorrectionFileNameList[elecTrig])
          .ignore();
      elecSFTool_handle.setProperty("CorrelationModel", m_elecCorrelationModel)
          .ignore();
      elecSFTool_handle.retrieve().ignore();
    }
    m_elecEffTools_handleArray[itrig] = elecEffTool_handle.getHandle();
    m_elecSFTools_handleArray[itrig] = elecSFTool_handle.getHandle();
  }

  // apply systematic variation on any of the electron/muon/photon eff/SF tools
  if (!this->applySystematicVariation(m_sysSet)) {
    throw std::runtime_error("Failed to apply systematic variation '" +
                             m_sysSet.name() + "'");
  }

  if (!m_trigGlobalTool_handle.isUserConfigured()) {
    m_trigGlobalTool_handle.setTypeAndName(
        "TrigGlobalEfficiencyCorrectionTool/TrigGlobalEfficiencyCorrection_" +
        m_sysSet.name() + "_" + std::to_string(slot));
    // HARDCODED FOR NOW
    m_trigGlobalTool_handle.setProperty("TriggerCombination2022",
                                        m_elecTriggers[0]).ignore();
    m_trigGlobalTool_handle
        .setProperty("ElectronEfficiencyTools", m_elecEffTools_handleArray)
        .ignore();
    m_trigGlobalTool_handle
        .setProperty("ElectronScaleFactorTools", m_elecSFTools_handleArray)
        .ignore();
    m_trigGlobalTool_handle.setProperty("OutputLevel", m_outputLevel).ignore();
    m_trigGlobalTool_handle.retrieve().ignore();
  }
}

double GlobalTriggerScaleFactor::evaluate(
    Observable<ConstDataVector<xAOD::ElectronContainer>> electrons) const {
  // run electron trigger efficiency & SF tools
  for (auto elItr : *(electrons)) {
    for (auto &elecEffTool_handle : m_elecEffTools_handles) {
      double effSF = -1.0;
      auto status = elecEffTool_handle->getEfficiencyScaleFactor(*elItr, effSF);
      if (status == CP::CorrectionCode::Error) {
        throw std::runtime_error(
            "problem in electron efficiency correction tool");
      } else if (status == CP::CorrectionCode::OutOfValidityRange) {
        // std::cout << "electron out of validity range for efficiency
        // calculation" << std::endl;
      }
    }
    for (auto &elecSFTool_handle : m_elecSFTools_handles) {
      double effSF = -1.0;
      auto status = elecSFTool_handle->getEfficiencyScaleFactor(*elItr, effSF);
      if (status == CP::CorrectionCode::Error) {
        throw std::runtime_error(
            "problem in electron efficiency correction tool");
      } else if (status == CP::CorrectionCode::OutOfValidityRange) {
        // std::cout << "electron out of validity range for efficiency
        // calculation" << std::endl;
      }
    }
  }

  std::vector<xAOD::Electron const *> triggeringElectrons(electrons->begin(),
                                                          electrons->end());
  double globTrigSF = 1.0;
  if (m_trigGlobalTool_handle->getEfficiencyScaleFactor(
      triggeringElectrons, globTrigSF) != CP::CorrectionCode::Ok) {
        throw std::runtime_error("cannot calculate global trigger scale factor");
  }
  return globTrigSF;
}

void GlobalTriggerScaleFactor::finalize(unsigned int) {}

StatusCode GlobalTriggerScaleFactor::applySystematicVariation(
    CP::SystematicSet const &sysSet) {
  for (auto &elecEffTool_handle : m_elecEffTools_handleArray) {
    auto statusCode = elecEffTool_handle->applySystematicVariation(sysSet);
    if (statusCode != StatusCode::SUCCESS) return statusCode;
  }
  for (auto &elecSFTool_handle : m_elecSFTools_handleArray) {
    auto statusCode = elecSFTool_handle->applySystematicVariation(sysSet);
    if (statusCode != StatusCode::SUCCESS) return statusCode;
  }
  return StatusCode::SUCCESS;
}