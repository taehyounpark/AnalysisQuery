#include "AnaQuery/ElectronSelection.h"

AnaQ::ElectronSelection::ElectronSelection(const Json& cfg) {
  m_minPt = cfg.value("minPt",0.0);
  m_maxEta = cfg.value("maxEta",10.0);
  m_isoWorkingPoint = cfg["isoWorkingPoint"].get<std::string>();
  m_idWorkingPoint = cfg["idWorkingPoint"].get<std::string>();
  m_passId = std::make_unique<SG::AuxElement::ConstAccessor<char>>("DFCommonElectrons"+m_idWorkingPoint);
}

void AnaQ::ElectronSelection::initialize(unsigned int slot, unsigned long long, unsigned long long) {
  m_isolationTool = std::make_unique<CP::IsolationSelectionTool>("IsolationSelectionTool_"+m_isoWorkingPoint+"_slot"+std::to_string(slot));
  m_isolationTool->setProperty("ElectronWP", m_isoWorkingPoint).ignore();
  m_isolationTool->initialize().ignore();
}

ConstDataVector<xAOD::ElectronContainer> AnaQ::ElectronSelection::evaluate(AnaQ::Observable<ConstDataVector<xAOD::ElectronContainer>> allElectrons) const
{
  ConstDataVector<xAOD::ElectronContainer> selectedElectrons(SG::VIEW_ELEMENTS);
  for (auto elecItr : *allElectrons) {
    if (elecItr->pt() < m_minPt)
      continue;
    if (TMath::Abs(elecItr->eta()) > m_maxEta)
      continue;
    if (!(*m_passId)(*elecItr))
      continue;
    if (!m_isolationTool->accept(*elecItr))
      continue;
    selectedElectrons.push_back(elecItr);
  }
  return selectedElectrons;
}

void AnaQ::ElectronSelection::finalize(unsigned int) {
  m_isolationTool.reset();
}