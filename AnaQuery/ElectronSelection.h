#pragma once

#include "EventHelpers.h"

#include <EventLoop/StatusCode.h>

#include "AsgTools/AnaToolHandle.h"
#include "IsolationSelection/IIsolationSelectionTool.h"
#include "TrigDecisionTool/TrigDecisionTool.h"
#include "TriggerMatchingTool/IMatchingTool.h"
#include "TriggerMatchingTool/IMatchScoringTool.h"

#include "AthContainers/ConstDataVector.h"
#include "AthContainers/DataVector.h"

#include "xAODEventInfo/EventInfo.h"
#include "xAODEgamma/Electron.h"
#include "xAODEgamma/ElectronContainer.h"

namespace AnaQ {

class ElectronSelection
    : public Column<ConstDataVector<xAOD::ElectronContainer>(
          xAOD::ElectronContainer)> {
public:
  ElectronSelection(double pT_min, double eta_max)
      : m_pT_min(pT_min), m_eta_max(eta_max) {}
  virtual ~ElectronSelection() = default;
  virtual ConstDataVector<xAOD::ElectronContainer>
  evaluate(Observable<xAOD::ElectronContainer> els) const override;
protected:
  double m_pT_min;
  double m_eta_max;

  asg::AnaToolHandle<CP::IIsolationSelectionTool> m_isolationSelectionTool_handle{"CP::IsolationSelectionTool/IsolationSelectionTool"}; //!
  // this only exists because the interface needs to be updated, complain on pathelp, remove forward declaration for this when fixed
  // CP::IsolationSelectionTool*                     m_isolationSelectionTool{nullptr};                                                                 //!
  asg::AnaToolHandle<Trig::TrigDecisionTool>      m_trigDecTool_handle           {"Trig::TrigDecisionTool/TrigDecisionTool"                       }; //!
  asg::AnaToolHandle<Trig::IMatchingTool>         m_trigElectronMatchTool_handle; //!
  asg::AnaToolHandle<Trig::IMatchScoringTool>     m_scoreTool                    {"Trig::DRScoringTool/DRScoringTool"                             }; //!
};

}