#include "AnalysisQuery/ElectronSelection.h"

ElectronSelection::ElectronSelection(
    ElectronSelection::Configuration const &cfg)
    : m_cfg(cfg) {}

void ElectronSelection::initialize(unsigned int slot, unsigned long long,
                                   unsigned long long) {
  m_passId = std::make_unique<SG::AuxElement::ConstAccessor<char>>(
      "DFCommonElectrons" + m_cfg.idWorkingPoint);

  m_isolationToolHandle.setTypeAndName(
      "CP::IsolationSelectionTool/IsolationSelectionTool_" +
      m_cfg.isoWorkingPoint);
  if (!m_isolationToolHandle.isUserConfigured()) {
    m_isolationToolHandle.setProperty("ElectronWP", m_cfg.isoWorkingPoint)
        .ignore();
  }
  m_isolationToolHandle.retrieve().ignore();
}

ConstDataVector<xAOD::ElectronContainer> ElectronSelection::evaluate(
    qty::column::observable<ConstDataVector<xAOD::ElectronContainer>>
        allElectrons) const {
  ConstDataVector<xAOD::ElectronContainer> selectedElectrons(SG::VIEW_ELEMENTS);
  for (auto elecItr : *allElectrons) {
    if (elecItr->pt() < m_cfg.minPt)
      continue;
    if (TMath::Abs(elecItr->eta()) > m_cfg.maxEta)
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