#include "EventFlow/ElectronSelection.h"

ElectronSelection::ElectronSelection(ElectronSelectionConfig const &cfg)
    : EventFlow::Column<ConstDataVector<xAOD::ElectronContainer>(
                            ConstDataVector<xAOD::ElectronContainer>),
                        ElectronSelectionConfig>(cfg) {}

void ElectronSelection::initialize(unsigned int, unsigned long long,
                                   unsigned long long) {
  m_passId = std::make_unique<SG::AuxElement::ConstAccessor<char>>(
      "DFCommonElectrons" + cfg()->idWorkingPoint);

  m_isolationToolHandle.setTypeAndName(
      "CP::IsolationSelectionTool/IsolationSelectionTool_" +
      cfg()->isoWorkingPoint);
  if (!m_isolationToolHandle.isUserConfigured()) {
    m_isolationToolHandle.setProperty("ElectronWP", cfg()->isoWorkingPoint)
        .ignore();
  }
  m_isolationToolHandle.retrieve().ignore();
}

ConstDataVector<xAOD::ElectronContainer> ElectronSelection::evaluate(
    qty::column::observable<ConstDataVector<xAOD::ElectronContainer>>
        allElectrons) const {
  ConstDataVector<xAOD::ElectronContainer> selectedElectrons(SG::VIEW_ELEMENTS);
  for (auto elecItr : *allElectrons) {
    if (elecItr->pt() < cfg()->minPt)
      continue;
    if (TMath::Abs(elecItr->eta()) > cfg()->maxEta)
      continue;
    if (!(*m_passId)(*elecItr))
      continue;
    if (!m_isolationToolHandle->accept(*elecItr))
      continue;
    selectedElectrons.push_back(elecItr);
  }
  return selectedElectrons;
}

void ElectronSelection::finalize(unsigned int) {
  // m_isolationToolHandle.reset();
}