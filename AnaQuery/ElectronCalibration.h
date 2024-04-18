#pragma once

#include "AnaQuery/ObjectCalibration.h"

// external tools include(s):
#include "ElectronPhotonFourMomentumCorrection/EgammaCalibrationAndSmearingTool.h"
#include "IsolationCorrections/IsolationCorrectionTool.h"

#include "xAODEgamma/Electron.h"
#include "xAODEgamma/ElectronContainer.h"

#include "queryosity.h"

namespace AnaQ {

class ElectronCalibration : public ObjectCalibration<xAOD::ElectronContainer> {

public:
  ElectronCalibration(Settings calibCfg, std::pair<std::string,float> systVar={"",0.0});

  void initialize(unsigned int, unsigned long long,
                  unsigned long long) final override;
  ConstDataVector<xAOD::ElectronContainer>
  evaluate(qty::column::observable<xAOD::ElectronContainer> elCont)
      const final override;
  void finalize(unsigned int) final override;

protected:
  Settings m_calibCfg;
  std::string m_systName;
  float       m_systVal;

  std::unique_ptr<CP::EgammaCalibrationAndSmearingTool>
      m_EgammaCalibrationAndSmearingTool;                                 //!
  std::unique_ptr<CP::IsolationCorrectionTool> m_IsolationCorrectionTool; //!

  // ClassDef(ElectronCalibration, 1);
};

} // namespace AnaQ