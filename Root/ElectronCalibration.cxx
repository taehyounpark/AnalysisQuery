#include "EventFlow/ElectronCalibration.h"

#include <iostream>

#include "TDirectory.h"

ElectronCalibration::ElectronCalibration(
    ElectronCalibrationConfig const &cfg,
    CP::SystematicVariation const &sysVar)
    : ObjectCalibration<
          xAOD::ElectronContainer,
          ElectronCalibrationConfig>::ObjectCalibration(cfg, sysVar) {}

void ElectronCalibration::initialize(unsigned int slot, unsigned long long,
                                     unsigned long long) {

  // calibration & smearin
  m_p4CorrToolHandle = asg::AnaToolHandle<CP::EgammaCalibrationAndSmearingTool>(
      "CP::EgammaCalibrationAndSmearingTool/"
      "EgammaCalibrationAndSmearingTool_" +
      this->m_sysSet.name() + "_" + std::to_string(slot));
  if (!m_p4CorrToolHandle.isUserConfigured()) {
    m_p4CorrToolHandle.setProperty("ESModel", cfg()->esModel).ignore();
    m_p4CorrToolHandle
        .setProperty("decorrelationModel", cfg()->decorrelationModel)
        .ignore();
    m_p4CorrToolHandle.setProperty("useFastSim", (int)cfg()->useFastSim)
        .ignore();
    if (cfg()->randomRunNumber >= 0)
      m_p4CorrToolHandle.setProperty("randomRunNumber", cfg()->randomRunNumber)
          .ignore();
  }
  m_p4CorrToolHandle.retrieve().ignore();
  m_p4CorrToolHandle->applySystematicVariation(this->m_sysSet).ignore();

  // isolation correction
  m_isoCorrToolHandle = asg::AnaToolHandle<CP::IsolationCorrectionTool>(
      "CP::IsolationCorrectionTool/IsolationCorrectionTool_" +
      this->m_sysSet.name() + "_" + std::to_string(slot));
  if (!m_isoCorrToolHandle.isUserConfigured()) {
    m_isoCorrToolHandle.setProperty("IsMC", true).ignore();
  }
  m_isoCorrToolHandle.retrieve().ignore();
}

ConstDataVector<xAOD::ElectronContainer> ElectronCalibration::evaluate(
    qty::column::observable<xAOD::ElectronContainer> elCont) const {
  ConstDataVector<xAOD::ElectronContainer> calibratedElectrons;

  // keep shallow copy for the event
  this->keepShallowCopy(xAOD::shallowCopyContainer(*elCont));

  // calibration each electron
  for (auto elItr : *(this->m_shallowCopy.elementContainer)) {

    // electron must have associated cluster & track
    if (elItr->caloCluster() && elItr->trackParticle()) {

      if (m_p4CorrToolHandle->applyCorrection(*elItr) !=
          CP::CorrectionCode::Ok) {
        throw std::runtime_error("failed to apply electron calibration");
      }

      if (cfg()->applyIsolationCorrection) {
        if (elItr->pt() > 7e3 && m_isoCorrToolHandle->CorrectLeakage(*elItr) !=
                                     CP::CorrectionCode::Ok) {
          throw std::runtime_error(
              "failed to apply electron leakage correction");
        }
      }
    }

    if (!xAOD::setOriginalObjectLink(*elCont,
                                     *this->m_shallowCopy.elementContainer)) {
      throw std::runtime_error("failed to make shallow copy");
    }
  }
  auto calibElCont = EventHelpers::makeConstDataVector(
      this->m_shallowCopy.elementContainer.get());

  if (cfg()->sortByPt) {
    std::sort(calibElCont.begin(), calibElCont.end(), EventHelpers::sortByPt);
  }

  return calibElCont;
}

void ElectronCalibration::finalize(unsigned int) { this->freeShallowCopy(); }