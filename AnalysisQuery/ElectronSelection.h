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

class ElectronSelection
  : public Column<ConstDataVector<xAOD::ElectronContainer>(
      ConstDataVector<xAOD::ElectronContainer>)>
{

public:
  ElectronSelection(const Json& cfg);
  ~ElectronSelection() = default;

  void initialize(unsigned int,
                  unsigned long long,
                  unsigned long long) override;

  ConstDataVector<xAOD::ElectronContainer> evaluate(
    Observable<ConstDataVector<xAOD::ElectronContainer>>) const override;

  void finalize(unsigned int) override;

protected:
  double m_minPt;
  double m_maxEta;

  asg::AnaToolHandle<CP::IsolationSelectionTool> m_isolationTool_handle; //!
  std::string m_isoWorkingPoint;

  std::string m_idWorkingPoint;
  std::unique_ptr<SG::AuxElement::ConstAccessor<char>> m_passId;

};