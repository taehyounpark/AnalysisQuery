#include "EventFlow/JetCalibration.h"

#include "JetCalibTools/JetCalibrationTool.h"
#include "JetUncertainties/JetUncertaintiesTool.h"
#include "JetSelectorTools/JetCleaningTool.h"

#include <EventLoop/StatusCode.h>

JetCalibration::JetCalibration(JetCalibrationConfig const &cfg)
    : ObjectCalibration<xAOD::JetContainer, JetCalibrationConfig>(cfg) {}

void JetCalibration::initialize(unsigned int, unsigned long long,
                                unsigned long long) {
  m_JetCalibrationTool_handle.setTypeAndName("JetCalibrationTool/jetCalibrationTool");
  m_JetCalibrationTool_handle.setProperty("JetCollection",cfg()->jetCollection).ignore();
  m_JetCalibrationTool_handle.setProperty("CalibArea",cfg()->calibArea).ignore();
  m_JetCalibrationTool_handle.setProperty("ConfigFile",cfg()->calibConfig).ignore();
  m_JetCalibrationTool_handle.setProperty("CalibSequence",cfg()->calibSequence).ignore();
  m_JetCalibrationTool_handle.setProperty("IsData",!cfg()->isMC).ignore();
  m_JetCalibrationTool_handle.retrieve().ignore(); 
}

ConstDataVector<xAOD::JetContainer> JetCalibration::evaluate(
    qty::column::observable<xAOD::JetContainer> jets) const {

  // keep shallow copy for the event
  this->keepShallowCopy(xAOD::shallowCopyContainer(*jets));

  if (m_JetCalibrationTool_handle->applyCalibration(
          *this->m_shallowCopy.elementContainer) == EL::StatusCode::FAILURE) {
    throw std::runtime_error("failed to apply jet calibration");
  }

  if (!xAOD::setOriginalObjectLink(*jets,
                                   *this->m_shallowCopy.elementContainer)) {
    throw std::runtime_error("failed to make shallow copy");
  }

  auto calibJets = EventHelpers::makeConstDataVector(
      this->m_shallowCopy.elementContainer.get());

  return calibJets;
}