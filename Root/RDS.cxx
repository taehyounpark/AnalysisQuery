#include "AnaQuery/RDS.h"

#include "TROOT.h"

AnaQ::RDS::RDS(std::unique_ptr<RDataSource> rds) : m_rds(std::move(rds)) {
  ROOT::EnableThreadSafety();
}

void AnaQ::RDS::parallelize(unsigned int nslots) { m_rds->SetNSlots(nslots); }

std::vector<std::pair<unsigned long long, unsigned long long>>
AnaQ::RDS::partition() {
  return m_rds->GetEntryRanges();
}

void AnaQ::RDS::initialize() {
#if ROOT_VERSION_CODE >= ROOT_VERSION(6, 28, 00)
  m_rds->Initialize();
#else
  m_rds->Initialise();
#endif
}

void AnaQ::RDS::initialize(unsigned int slot, unsigned long long begin,
                           unsigned long long /* end */) {
  m_rds->InitSlot(slot, begin);
}

void AnaQ::RDS::execute(unsigned int slot, unsigned long long entry) {
  m_rds->SetEntry(slot, entry);
}

void AnaQ::RDS::finalize(unsigned int slot) {
#if ROOT_VERSION_CODE >= ROOT_VERSION(6, 28, 00)
  m_rds->FinalizeSlot(slot);
#else
  m_rds->FinaliseSlot(slot);
#endif
}

void AnaQ::RDS::finalize() {
#if ROOT_VERSION_CODE >= ROOT_VERSION(6, 28, 00)
  m_rds->Finalize();
#else
  m_rds->Finalise();
#endif
}