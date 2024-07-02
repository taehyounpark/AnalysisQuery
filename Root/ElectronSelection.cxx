#include "AnalysisQuery/ElectronSelection.h"

ElectronSelection::ElectronSelection(const Json& cfg) {
  m_minPt = cfg.value("minPt",0.0);
  m_maxEta = cfg.value("maxEta",10.0);
  m_isoWorkingPoint = cfg["isoWorkingPoint"].get<std::string>();
  m_idWorkingPoint = cfg["idWorkingPoint"].get<std::string>();
}

void ElectronSelection::initialize(unsigned int slot, unsigned long long, unsigned long long) {
  m_passId = std::make_unique<SG::AuxElement::ConstAccessor<char>>("DFCommonElectrons"+m_idWorkingPoint);
  if (!m_isolationTool_handle.isUserConfigured()) {
    m_isolationTool_handle.setTypeAndName("CP::IsolationSelectionTool/IsolationSelectionTool_"+m_isoWorkingPoint+"_"+std::to_string(slot));
    m_isolationTool_handle.setProperty("ElectronWP", m_isoWorkingPoint).ignore();
    m_isolationTool_handle.retrieve().ignore();
  }
}

ConstDataVector<xAOD::ElectronContainer> ElectronSelection::evaluate(Observable<ConstDataVector<xAOD::ElectronContainer>> allElectrons) const
{
  ConstDataVector<xAOD::ElectronContainer> selectedElectrons(SG::VIEW_ELEMENTS);
  for (auto elecItr : *allElectrons) {
    if (elecItr->pt() < m_minPt)
      continue;
    if (TMath::Abs(elecItr->eta()) > m_maxEta)
      continue;
    if (!(*m_passId)(*elecItr))
      continue;
    if (!m_isolationTool_handle->accept(*elecItr))
      continue;
    selectedElectrons.push_back(elecItr);
  }
  return selectedElectrons;
}

void ElectronSelection::finalize(unsigned int) {
  // m_isolationTool_handle.reset();
}