#pragma once

#include "EventFlow/EventHelpers.h"

// EDM
#include "xAODEgamma/ElectronContainer.h"
#include "xAODEventInfo/EventInfo.h"

// CP tools
#include "AsgTools/AnaToolHandle.h"
#include "EgammaAnalysisInterfaces/IAsgElectronEfficiencyCorrectionTool.h"

// CP interface
#include "PATInterfaces/SystematicRegistry.h"
#include "PATInterfaces/SystematicSet.h"
#include "PATInterfaces/SystematicVariation.h"
#include "PATInterfaces/SystematicsUtil.h"

#include "Action.h"

#include <queryosity.hpp>

#include <ROOT/RVec.hxx>

struct ElectronScaleFactorConfig {
  int simulationFlavour;
  std::string correlationModel;
  std::string idWorkingPoint;
  std::string isoWorkingPoint;
  std::string recoWorkingPoint;
  std::string trigWorkingPoint;
  std::vector<std::string> correctionFileNameList;
};

class ElectronScaleFactor
    : public EventFlow::Column<ROOT::RVec<double>(
                           ConstDataVector<xAOD::ElectronContainer>),
                       ElectronScaleFactorConfig> {

public:
  ElectronScaleFactor(ElectronScaleFactorConfig const &sfCfg);
  ElectronScaleFactor(ElectronScaleFactorConfig const &sfCfg,
                               CP::SystematicVariation const &sysVar);
  ~ElectronScaleFactor() = default;

  void initialize(unsigned int, unsigned long long,
                  unsigned long long) override;
  ROOT::RVec<double> evaluate(
      qty::column::observable<ConstDataVector<xAOD::ElectronContainer>>)
      const override;
  void finalize(unsigned int) override;

protected:
  std::string m_toolName;
  mutable asg::AnaToolHandle<IAsgElectronEfficiencyCorrectionTool>
      m_elEffCorrTool_handle; //!

};