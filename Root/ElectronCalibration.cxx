#include "HepQuery/ElectronCalibration.h"

#include <iostream>

#include <EventLoop/Job.h>
#include <EventLoop/StatusCode.h>
#include <EventLoop/Worker.h>

HepQ::ElectronCalibration::ElectronCalibration(ElectronCalibration::Config cfg) : m_cfg(cfg) {}

void HepQ::ElectronCalibration::initialize(unsigned int, unsigned long long ,unsigned long long) {
  m_outAuxContainerName     = m_cfg.outContainerName + "Aux."; // the period is very important!
  m_outSCContainerName      = m_cfg.outContainerName + "ShallowCopy";
  m_outSCAuxContainerName   = m_outSCContainerName + "Aux."; // the period is very important!

  // CP::EgammaCalibrationAndSmearingTool
  m_EgammaCalibrationAndSmearingTool = std::make_unique<CP::EgammaCalibrationAndSmearingTool>("EgammaCalibrationAndSmearingTool");
  m_EgammaCalibrationAndSmearingTool->msg().setLevel( MSG::ERROR ); // DEBUG, VERBOSE, INFO
  m_EgammaCalibrationAndSmearingTool->setProperty("ESModel", m_cfg.esModel);
  m_EgammaCalibrationAndSmearingTool->setProperty("decorrelationModel", m_cfg.decorrelationModel);
  if ( m_cfg.useFastSim ){
    // ANA_CHECK( m_EgammaCalibrationAndSmearingTool->setProperty("useFastSim", 1));
    m_EgammaCalibrationAndSmearingTool->setProperty("useFastSim", 1);
  }
  else {
    m_EgammaCalibrationAndSmearingTool->setProperty("useFastSim", 0);
  }
  // HACK for now
  m_EgammaCalibrationAndSmearingTool->setProperty("randomRunNumber", 123456);
  m_EgammaCalibrationAndSmearingTool->initialize();

  // Get a list of recommended systematics for this tool
  const CP::SystematicSet& recSyst = m_EgammaCalibrationAndSmearingTool->recommendedSystematics();

  // ANA_MSG_INFO(" Initializing Electron Calibrator Systematics :");
  m_systList = EventHelpers::getListofSystematics( recSyst, m_cfg.systMode, m_cfg.systVal );

  // ANA_MSG_INFO("Will be using EgammaCalibrationAndSmearingTool systematic:");
  m_systNames.clear();
  m_systNames.reserve(m_systList.size());
  for ( const auto& syst_it : m_systList ) {
    m_systNames.push_back(syst_it.name());
  }

  m_IsolationCorrectionTool = std::make_unique<CP::IsolationCorrectionTool>("IsolationCorrectionTool");
  m_IsolationCorrectionTool->msg().setLevel( MSG::INFO ); // DEBUG, VERBOSE, INFO
  m_IsolationCorrectionTool->setProperty("IsMC", true);
  m_IsolationCorrectionTool->initialize();
}

SystematicMap<ConstDataVector<xAOD::ElectronContainer>> HepQ::ElectronCalibration::evaluate(qty::column::observable<xAOD::ElectronContainer> elCont) const {
  SystematicMap<ConstDataVector<xAOD::ElectronContainer>> systElecContCDV;

  for ( const auto& syst_it : m_systList ) {
    std::string outSCContainerName(m_outSCContainerName);
    std::string outSCAuxContainerName(m_outSCAuxContainerName);
    std::string outContainerName(m_cfg.outContainerName);

    outSCContainerName    += syst_it.name();
    outSCAuxContainerName += syst_it.name();
    outContainerName      += syst_it.name();

    // apply syst
    //
    if ( m_EgammaCalibrationAndSmearingTool->applySystematicVariation(syst_it) != EL::StatusCode::SUCCESS ) {
      throw std::runtime_error("Failed to configure EgammaCalibrationAndSmearingTool for systematic ");
      // ANA_MSG_ERROR( "Failed to configure EgammaCalibrationAndSmearingTool for systematic " << syst_it.name());
      // return EL::StatusCode::FAILURE;
    }

    auto shallowCopy = ShallowCopy(xAOD::shallowCopyContainer(*elCont));

    for ( auto elItr : *(shallowCopy.elements) ) {
      if ( elItr->caloCluster() && elItr->trackParticle() ) {  // NB: derivations might remove CC and tracks for low pt electrons
        if ( m_EgammaCalibrationAndSmearingTool->applyCorrection( *elItr ) != CP::CorrectionCode::Ok ) {
          // ANA_MSG_WARNING( "Problem in CP::EgammaCalibrationAndSmearingTool::applyCorrection()");
        }
        if ( m_cfg.applyIsolationCorrection ) {
          if ( elItr->pt() > 7e3 && m_IsolationCorrectionTool->CorrectLeakage( *elItr ) != CP::CorrectionCode::Ok ) {
            // ANA_MSG_WARNING( "Problem in CP::IsolationCorrectionTool::CorrectLeakage()");
          }
        }
      }
    }

    if ( !xAOD::setOriginalObjectLink(*elCont, *shallowCopy.elements) ) {
      throw std::runtime_error("Failed to set original object links -- MET rebuilding cannot proceed.");
      // ANA_MSG_ERROR( "Failed to set original object links -- MET rebuilding cannot proceed.");
    }

    auto calibEls = EventHelpers::makeConstDataVector(shallowCopy.elements.get());
    this->m_shallowCopies[outSCContainerName] = std::move(shallowCopy);
    systElecContCDV[outContainerName] = std::move(calibEls);
  }

  return systElecContCDV;
}

void HepQ::ElectronCalibration::finalize(unsigned int) {
  this->m_shallowCopies.clear();
}