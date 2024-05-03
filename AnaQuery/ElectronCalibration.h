#pragma once

#include "AnaQuery/ObjectCalibration.h"

// external tools include(s):
#include "ElectronPhotonFourMomentumCorrection/EgammaCalibrationAndSmearingTool.h"
#include "IsolationCorrections/IsolationCorrectionTool.h"

#include "xAODEgamma/Electron.h"
#include "xAODEgamma/ElectronContainer.h"

#include "queryosity.h"

namespace AnaQ {

class ElectronCalibration : public ObjectCalibration<xAOD::ElectronContainer>
{

private:
  static unsigned s_nInstances;

public:
  ElectronCalibration(Json const& calibCfg,
                      const CP::SystematicVariation& sysVar = {});
  ~ElectronCalibration() = default;

  void initialize(unsigned int,
                  unsigned long long,
                  unsigned long long) final override;
  ConstDataVector<xAOD::ElectronContainer> evaluate(
    Observable<xAOD::ElectronContainer> elCont) const final override;
  void finalize(unsigned int) final override;

protected:
  std::string m_esModel;
  std::string m_decorrelationModel;
  bool m_useFastSim;
  int m_randomRunNumber;
  bool m_applyIsolationCorrection;
  bool m_sortByPt;
  unsigned int m_index;

  std::unique_ptr<CP::EgammaCalibrationAndSmearingTool> m_p4CorrTool; //!
  std::unique_ptr<CP::IsolationCorrectionTool> m_isoCorrTool;         //!
};

} // namespace AnaQ