#include "AnalysisQuery/ElectronCalibration.h"

#include <iostream>

// #include <EventLoop/Job.h>
// #include <EventLoop/StatusCode.h>
// #include <EventLoop/Worker.h>

#include "TDirectory.h"

ElectronCalibration::ElectronCalibration(
    ElectronCalibration::Configuration const &cfg,
    CP::SystematicVariation const &sysVar)
    : ObjectCalibration<xAOD::ElectronContainer>::ObjectCalibration(sysVar),
      m_cfg(cfg) {}

void ElectronCalibration::initialize(unsigned int slot, unsigned long long,
                                     unsigned long long) {

  // calibration & smearin
  m_p4CorrToolHandle = asg::AnaToolHandle<CP::EgammaCalibrationAndSmearingTool>(
      "CP::EgammaCalibrationAndSmearingTool/"
      "EgammaCalibrationAndSmearingTool_" +
      m_sysSet.name() + "_" + std::to_string(slot));
  if (!m_p4CorrToolHandle.isUserConfigured()) {
    m_p4CorrToolHandle.setProperty("ESModel", m_cfg.esModel).ignore();
    m_p4CorrToolHandle.setProperty("decorrelationModel", m_cfg.decorrelationModel)
        .ignore();
    m_p4CorrToolHandle.setProperty("useFastSim", (int)m_cfg.useFastSim).ignore();
    if (m_cfg.randomRunNumber >= 0)
      m_p4CorrToolHandle.setProperty("randomRunNumber", m_cfg.randomRunNumber).ignore();
  }
  m_p4CorrToolHandle.retrieve().ignore();
  m_p4CorrToolHandle->applySystematicVariation(m_sysSet).ignore();

  // isolation correction
  m_isoCorrToolHandle = asg::AnaToolHandle<CP::IsolationCorrectionTool>(
      "CP::IsolationCorrectionTool/IsolationCorrectionTool_" + m_sysSet.name() +
      "_" + std::to_string(slot));
  if (!m_isoCorrToolHandle.isUserConfigured()) {
    m_isoCorrToolHandle.setProperty("IsMC", true).ignore();
  }
  m_isoCorrToolHandle.retrieve().ignore();
}

ConstDataVector<xAOD::ElectronContainer> ElectronCalibration::evaluate(
    qty::column::observable<xAOD::ElectronContainer> elCont) const {
  ConstDataVector<xAOD::ElectronContainer> calibratedElectrons;

  // keep shallow copy for the event
  m_shallowCopy = ShallowCopy(xAOD::shallowCopyContainer(*elCont));

  // calibration each electron
  for (auto elecItr : *(m_shallowCopy.elements)) {

    // electron must have associated cluster & track
    if (elecItr->caloCluster() && elecItr->trackParticle()) {

      if (m_p4CorrToolHandle->applyCorrection(*elecItr) !=
          CP::CorrectionCode::Ok) {
        throw std::runtime_error("failed to apply electron calibration");
      }

      if (m_cfg.applyIsolationCorrection) {
        if (elecItr->pt() > 7e3 && m_isoCorrToolHandle->CorrectLeakage(
                                       *elecItr) != CP::CorrectionCode::Ok) {
          throw std::runtime_error(
              "failed to apply electron leakage correction");
        }
      }
    }

    if (!xAOD::setOriginalObjectLink(*elCont, *m_shallowCopy.elements)) {
      throw std::runtime_error("failed to make shallow copy");
    }
  }
  auto calibElCont =
      EventHelpers::getConstDataVector(m_shallowCopy.elements.get());

  if (m_cfg.sortByPt) {
    std::sort(calibElCont.begin(), calibElCont.end(), EventHelpers::sortByPt);
  }

  return calibElCont;
}

void ElectronCalibration::finalize(unsigned int) {
  // this->m_shallowCopy = {};
}