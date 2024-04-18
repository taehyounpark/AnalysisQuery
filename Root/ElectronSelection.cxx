#include "AnaQuery/ElectronSelection.h"

ConstDataVector<xAOD::ElectronContainer> AnaQ::ElectronSelection::evaluate(AnaQ::Observable<xAOD::ElectronContainer> els) const
{
  ConstDataVector<xAOD::ElectronContainer> els_sel(SG::VIEW_ELEMENTS);
  for (const xAOD::Electron *el : *els) {
    if (el->pt() < m_pT_min)
      continue;
    if (TMath::Abs(el->eta()) > m_eta_max)
      continue;
    els_sel.push_back(el);
  }
  return els_sel;
}