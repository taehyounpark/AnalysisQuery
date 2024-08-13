#pragma once

#include "EventHelpers.h"

#include "ElectronPhotonSelectorTools/AsgElectronIsEMSelector.h"
#include "ElectronPhotonSelectorTools/AsgElectronLikelihoodTool.h"
#include "ElectronPhotonSelectorTools/LikelihoodEnums.h"
#include "ElectronPhotonSelectorTools/egammaPIDdefs.h"

#include "AsgTools/AnaToolHandle.h"
#include "IsolationSelection/IsolationSelectionTool.h"

#include "AthContainers/ConstDataVector.h"
#include "AthContainers/DataVector.h"

#include "xAODEgamma/Electron.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODEventInfo/EventInfo.h"

#include <queryosity.hpp>

class ElectronSelection
    : public qty::column::definition<ConstDataVector<xAOD::ElectronContainer>(
          ConstDataVector<xAOD::ElectronContainer>)> {

  struct Configuration {
    double minPt;
    double maxEta;
    std::string isoWorkingPoint;
    std::string idWorkingPoint;
  };

public:
  ElectronSelection(Configuration const &cfg);
  ~ElectronSelection() = default;

  void initialize(unsigned int, unsigned long long,
                  unsigned long long) override;

  ConstDataVector<xAOD::ElectronContainer> evaluate(
      qty::column::observable<ConstDataVector<xAOD::ElectronContainer>>)
      const override;

  void finalize(unsigned int) override;

protected:
  Configuration m_cfg;

private:
  asg::AnaToolHandle<CP::IsolationSelectionTool> m_isolationToolHandle; //!
  std::unique_ptr<SG::AuxElement::ConstAccessor<char>> m_passId;        //!
};