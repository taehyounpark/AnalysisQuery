#pragma once

#include "AnaQuery/ObjectCalibration.h"

// external tools include(s):
#include "ElectronPhotonFourMomentumCorrection/EgammaCalibrationAndSmearingTool.h"
#include "IsolationCorrections/IsolationCorrectionTool.h"

#include "xAODEgamma/ElectronContainer.h"
#include "xAODEgamma/Electron.h"

#include "queryosity.h"


namespace HepQ {

class ElectronCalibration : public ObjectCalibration<xAOD::ElectronContainer>
{

public:
  struct Config {
    Config() = default;
    std::string inContainerName = "Electrons";
    std::string outContainerName = "ElectronsCalib";
    bool        applyIsolationCorrection = false;
    std::string esModel = "es2017_R21_v0";
    bool        useFastSim = false;
    std::string decorrelationModel = "";
    std::string systMode = "All";
    float       systVal = 1.0;
    bool        sortByPt = true;
  };

public:
  ElectronCalibration(Config cfg);

  void initialize(unsigned int, unsigned long long ,unsigned long long) final override;
  SystematicMap<ConstDataVector<xAOD::ElectronContainer>> evaluate(qty::column::observable<xAOD::ElectronContainer> elCont) const final override;
  void finalize(unsigned int) final override;

protected:
  Config m_cfg;

  std::unique_ptr<CP::EgammaCalibrationAndSmearingTool> m_EgammaCalibrationAndSmearingTool; //!
  std::unique_ptr<CP::IsolationCorrectionTool>          m_IsolationCorrectionTool;          //!

  // ClassDef(ElectronCalibration, 1);
};

}