#include "AnaQuery/RDS.h"

#include "TROOT.h"

HepQ::RDS::RDS(std::unique_ptr<RDataSource> rds) : m_rds(std::move(rds)) {
  ROOT::EnableThreadSafety();
}

void HepQ::RDS::parallelize(unsigned int nslots) { m_rds->SetNSlots(nslots); }

std::vector<std::pair<unsigned long long, unsigned long long>>
HepQ::RDS::partition() {
  return m_rds->GetEntryRanges();
}

void HepQ::RDS::initialize() { m_rds->Initialize(); }

void HepQ::RDS::finalize() { m_rds->Finalize(); }

void HepQ::RDS::initialize(unsigned int slot, unsigned long long begin,
                     unsigned long long /* end */) {
  m_rds->InitSlot(slot, begin);
}

void HepQ::RDS::execute(unsigned int slot, unsigned long long entry) {
  m_rds->SetEntry(slot, entry);
}

void HepQ::RDS::finalize(unsigned int slot) { m_rds->FinalizeSlot(slot); }