#pragma once

#include <AsgTools/AnaToolHandle.h>
#include <ElectronPhotonFourMomentumCorrection/EgammaCalibrationAndSmearingTool.h>
#include <IsolationCorrections/IsolationCorrectionTool.h>

#include <xAODEgamma/Electron.h>
#include <xAODEgamma/ElectronContainer.h>

#include <EventFlow/ObjectCalibration.h>

#include <queryosity.hpp>


struct ElectronCalibrationConfig {
  std::string esModel;
  std::string decorrelationModel;
  bool useFastSim;
  int randomRunNumber;
  bool applyIsolationCorrection;
  bool sortByPt;
};

class ElectronCalibration
    : public ObjectCalibration<xAOD::ElectronContainer,
                               ElectronCalibrationConfig> {

public:
  ElectronCalibration(ElectronCalibrationConfig const &cfg,
                      CP::SystematicVariation const &sysVar = {});
  ~ElectronCalibration() = default;

  void initialize(unsigned int, unsigned long long,
                  unsigned long long) final override;
  ConstDataVector<xAOD::ElectronContainer>
  evaluate(qty::column::observable<xAOD::ElectronContainer> elCont)
      const final override;
  void finalize(unsigned int) final override;

private:
  mutable asg::AnaToolHandle<CP::EgammaCalibrationAndSmearingTool>
      m_p4CorrToolHandle; //!
  mutable asg::AnaToolHandle<CP::IsolationCorrectionTool>
      m_isoCorrToolHandle; //!
};