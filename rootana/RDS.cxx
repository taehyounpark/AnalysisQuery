#include "rootana/RDS.h"

#include "TROOT.h"

#include "ana/concurrent.h"
#include "ana/strutils.h"
#include "ana/vecutils.h"

RDS::RDS(std::unique_ptr<RDataSource> rds) :
	m_rds(std::move(rds))
{}

ana::input::partition RDS::allocate()
{
  // force multithreading
  if (ana::multithread::status()) {
    ROOT::EnableImplicitMT(ana::multithread::concurrency());
  }
  m_rds->SetNSlots(ROOT::GetThreadPoolSize() ? ROOT::GetThreadPoolSize() : 1);

  // get allocated slots 
  auto slots = m_rds->GetEntryRanges();
  ana::input::partition partition;
  for (size_t islot=0 ; islot<slots.size() ; ++islot) {
    partition.add_part(islot, slots[islot].first, slots[islot].second);
  }

  // use whatever ROOT has decided
  partition.fixed = true;
  return partition;
}

void RDS::start()
{
  m_rds->Initialise();
}

void RDS::finish()
{
  m_rds->Finalise();
}

std::shared_ptr<RDS::Reader> RDS::read() const
{
	return std::make_shared<Reader>(*m_rds);
}

RDS::Reader::Reader(RDataSource& rds) :
  m_rds(&rds)
{}

void RDS::Reader::start(const ana::input::range& part)
{
  m_rds->InitSlot(part.slot, part.begin);
}

void RDS::Reader::next(const ana::input::range& part, unsigned long long entry)
{
  m_rds->SetEntry(part.slot, entry);
}

void RDS::Reader::finish(const ana::input::range& part)
{
  m_rds->FinaliseSlot(part.slot);
}