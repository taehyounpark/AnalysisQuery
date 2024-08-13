#pragma once

#include "AnalysisQuery/ObjectCalibration.h"

#include "AsgTools/AnaToolHandle.h"
#include "ElectronPhotonFourMomentumCorrection/EgammaCalibrationAndSmearingTool.h"
#include "IsolationCorrections/IsolationCorrectionTool.h"

#include "xAODEgamma/Electron.h"
#include "xAODEgamma/ElectronContainer.h"

#include <queryosity.hpp>

class ElectronCalibration : public ObjectCalibration<xAOD::ElectronContainer> {

public:
  struct Configuration {
    std::string esModel;
    std::string decorrelationModel;
    bool useFastSim;
    int randomRunNumber;
    bool applyIsolationCorrection;
    bool sortByPt;
  };

public:
  ElectronCalibration(Configuration const &cfg,
                      CP::SystematicVariation const &sysVar = {});
  ~ElectronCalibration() = default;

  void initialize(unsigned int, unsigned long long,
                  unsigned long long) final override;
  ConstDataVector<xAOD::ElectronContainer>
  evaluate(qty::column::observable<xAOD::ElectronContainer> elCont)
      const final override;
  void finalize(unsigned int) final override;

private:
  Configuration m_cfg;
  mutable asg::AnaToolHandle<CP::EgammaCalibrationAndSmearingTool>
      m_p4CorrToolHandle;                                                      //!
  mutable asg::AnaToolHandle<CP::IsolationCorrectionTool> m_isoCorrToolHandle; //!
};