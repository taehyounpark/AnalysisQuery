#include "AnaQuery/ElectronCalibration.h"

#include <iostream>

#include <EventLoop/Job.h>
#include <EventLoop/StatusCode.h>
#include <EventLoop/Worker.h>

AnaQ::ElectronCalibration::ElectronCalibration(Settings calibCfg, std::pair<std::string,float> systVar)
    : m_calibCfg(std::move(calibCfg)), m_systName(systVar.first), m_systVal(systVar.second) {}

void AnaQ::ElectronCalibration::initialize(unsigned int, unsigned long long,
                                           unsigned long long) {
  // CP::EgammaCalibrationAndSmearingTool
  m_EgammaCalibrationAndSmearingTool =
      std::make_unique<CP::EgammaCalibrationAndSmearingTool>(
          "EgammaCalibrationAndSmearingTool");
  m_EgammaCalibrationAndSmearingTool->msg().setLevel(
      MSG::ERROR); // DEBUG, VERBOSE, INFO
  m_EgammaCalibrationAndSmearingTool->setProperty("ESModel", m_calibCfg["esModel"].get<std::string>()).ignore();
  m_EgammaCalibrationAndSmearingTool->setProperty("decorrelationModel",
                                                  m_calibCfg["decorrelationModel"].get<std::string>()).ignore();
  if (m_calibCfg.value("useFastSim",false)) {
    m_EgammaCalibrationAndSmearingTool->setProperty("useFastSim", 1).ignore();
  } else {
    m_EgammaCalibrationAndSmearingTool->setProperty("useFastSim", 0).ignore();
  }
  if (int randomRunNumber = m_calibCfg.value("randomRunNumber",-1) >= 0) {
    m_EgammaCalibrationAndSmearingTool->setProperty("randomRunNumber", randomRunNumber).ignore();
  }
  m_EgammaCalibrationAndSmearingTool->initialize().ignore();
  // Get a list of recommended systematics for this tool
  const CP::SystematicSet &recSyst =
      m_EgammaCalibrationAndSmearingTool->recommendedSystematics();
  // ANA_MSG_INFO(" Initializing Electron Calibrator Systematics :");
  m_systVar = EventHelpers::getSystematicVariation(recSyst, m_systName, m_systVal);

  m_IsolationCorrectionTool =
      std::make_unique<CP::IsolationCorrectionTool>("IsolationCorrectionTool");
  m_IsolationCorrectionTool->msg().setLevel(MSG::INFO); // DEBUG, VERBOSE, INFO
  m_IsolationCorrectionTool->setProperty("IsMC", true).ignore();
  m_IsolationCorrectionTool->initialize().ignore();
}

ConstDataVector<xAOD::ElectronContainer> AnaQ::ElectronCalibration::evaluate(qty::column::observable<xAOD::ElectronContainer> elCont) const {
  ConstDataVector<xAOD::ElectronContainer> calibratedElectrons;

  m_shallowCopy = ShallowCopy(xAOD::shallowCopyContainer(*elCont));
  for (auto elItr : *(m_shallowCopy.elements)) {
    if (elItr->caloCluster() &&
        elItr->trackParticle()) { // NB: derivations might remove CC and
                                  // tracks for low pt electrons
      // std::cout << "electron pt correction (before) = " << elItr->pt();
      if (m_EgammaCalibrationAndSmearingTool->applyCorrection(*elItr) !=
          CP::CorrectionCode::Ok) {
        // ANA_MSG_WARNING( "Problem in
        // CP::EgammaCalibrationAndSmearingTool::applyCorrection()");
      }
      // std::cout << ", (after) = " << elItr->pt();
      // std::cout << std::endl;
      if (m_calibCfg.value("applyIsolationCorrection",false)) {
        if (elItr->pt() > 7e3 && m_IsolationCorrectionTool->CorrectLeakage(
                                      *elItr) != CP::CorrectionCode::Ok) {
          // ANA_MSG_WARNING( "Problem in
          // CP::IsolationCorrectionTool::CorrectLeakage()");
        }
      }
    }

    if (!xAOD::setOriginalObjectLink(*elCont, *m_shallowCopy.elements)) {
      throw std::runtime_error("Failed to set original object links -- MET "
                               "rebuilding cannot proceed.");
      // ANA_MSG_ERROR( "Failed to set original object links -- MET rebuilding
      // cannot proceed.");
    }

  }
  return EventHelpers::makeConstDataVector(m_shallowCopy.elements.get());
}

void AnaQ::ElectronCalibration::finalize(unsigned int) {
  this->m_shallowCopy = {};
}