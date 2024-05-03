#include "AnaQuery/GlobalTriggerScaleFactor.h"

#include "TDirectory.h"

AnaQ::GlobalTriggerScaleFactor::GlobalTriggerScaleFactor(
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
  m_index = s_nInstances++;
}

void AnaQ::GlobalTriggerScaleFactor::initialize(unsigned int slot,
                                                unsigned long long,
                                                unsigned long long) {
  if (m_trigGlobalTool_handle.empty()) {
    for (auto const &elecTrig : m_elecTriggers) {
      TDirectory::TContext c;
      m_elecEffTools_handles.emplace_back(
          "AsgElectronEfficiencyCorrectionTool/"
          "ElectronEfficiencyCorrection_efficiency_" +
          elecTrig + "_" + m_sysSet.name() + "_" + std::to_string(slot) + "_" +
          std::to_string(m_index));
      auto &elecEffTool_handle = m_elecEffTools_handles.back();
      elecEffTool_handle
          .setProperty("CorrectionFileNameList",
                       m_elecEffCorrectionFileNameList[elecTrig])
          .ignore();
      elecEffTool_handle.setProperty("CorrelationModel", m_elecCorrelationModel)
          .ignore();
      elecEffTool_handle.retrieve().ignore();
      m_elecEffTools_handleArray.emplace_back(elecEffTool_handle.getHandle());
    }

    for (auto const &elecTrig : m_elecTriggers) {
      TDirectory::TContext c;
      m_elecSFTools_handles.emplace_back(
          "AsgElectronEfficiencyCorrectionTool/"
          "ElectronEfficiencyCorrection_efficiencySF_" +
          elecTrig + "_" + m_sysSet.name() + "_" + std::to_string(slot) + "_" +
          std::to_string(m_index));
      auto &elecSFTool_handle = m_elecSFTools_handles.back();
      elecSFTool_handle
          .setProperty("CorrectionFileNameList",
                       m_elecSFCorrectionFileNameList[elecTrig])
          .ignore();
      elecSFTool_handle.setProperty("CorrelationModel", m_elecCorrelationModel)
          .ignore();
      elecSFTool_handle.retrieve().ignore();
      m_elecSFTools_handleArray.emplace_back(elecSFTool_handle.getHandle());
    }

    if (!applySystematicVariation(m_sysSet)) {
      throw std::runtime_error("Failed to apply systematic variation '" +
                               m_sysSet.name() + "'");
    }

    TDirectory::TContext c;
    m_trigGlobalTool_handle = asg::AnaToolHandle<
        ITrigGlobalEfficiencyCorrectionTool>(
        "TrigGlobalEfficiencyCorrectionTool/TrigGlobalEfficiencyCorrection_" +
        std::to_string(slot) + "_" + std::to_string(s_nInstances));
    // HARDCODED FOR NOW
    m_trigGlobalTool_handle.setProperty("TriggerCombination2022",
                                        m_elecTriggers[0]);
    m_trigGlobalTool_handle.setProperty("TriggerCombination2023",
                                        m_elecTriggers[0]);
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

double AnaQ::GlobalTriggerScaleFactor::evaluate(
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
  auto status = m_trigGlobalTool_handle->getEfficiencyScaleFactor(
      triggeringElectrons, globTrigSF);
  return globTrigSF;
}

void AnaQ::GlobalTriggerScaleFactor::finalize(unsigned int) {}

bool AnaQ::GlobalTriggerScaleFactor::applySystematicVariation(
    CP::SystematicSet const &sysSet) {
  bool sysCode;
  for (auto &elecEffTool_handle : m_elecEffTools_handleArray) {
    sysCode = sysCode || (elecEffTool_handle->applySystematicVariation(
                              sysSet) == EL::StatusCode::SUCCESS);
  }
  for (auto &elecSFTool_handle : m_elecSFTools_handleArray) {
    sysCode = sysCode || (elecSFTool_handle->applySystematicVariation(sysSet) ==
                          EL::StatusCode::SUCCESS);
  }
  return sysCode;
}