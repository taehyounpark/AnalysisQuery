#include "EventFlow/EventData.h"

#include "TROOT.h"

#include "xAODCutFlow/CutBookkeeper.h"
#include "xAODCutFlow/CutBookkeeperAuxContainer.h"
#include "xAODCutFlow/CutBookkeeperContainer.h"

EventData::EventData(const std::vector<std::string> &inputFiles,
                   const std::string &collection, const std::string &metadata)
    : m_inputFiles(inputFiles), m_treeName(collection), m_metaName(metadata)
     {}

void EventData::parallelize(unsigned int nslots) {
  ROOT::EnableThreadSafety();
  ROOT::EnableImplicitMT(nslots);
  xAOD::Init().ignore();
  TDirectory::TContext c;
  m_event_per_slot.resize(nslots);
  m_store_per_slot.resize(nslots);
  for (unsigned int islot = 0; islot < nslots; ++islot) {
    auto tree =
        std::make_unique<TChain>(m_treeName.c_str(), m_treeName.c_str());
    for (auto const &filePath : m_inputFiles) {
      tree->Add(filePath.c_str());
    }
    tree->ResetBit(kMustCleanup);
    auto event = std::make_unique<xAOD::TEvent>(xAOD::TEvent::kUndefinedAccess);
    if (event->readFrom(tree.release()).isFailure()) {
      throw std::runtime_error("failed to read event");
    };
    m_event_per_slot[islot] = std::move(event);
    m_store_per_slot[islot] = std::make_unique<xAOD::TStore>();
  }
}

std::vector<std::pair<unsigned long long, unsigned long long>>
EventData::partition() {
  TDirectory::TContext c;

  std::vector<std::pair<unsigned long long, unsigned long long>> parts;

  // check all files for tree clusters
  long long offset = 0;
  for (auto const &filePath : m_inputFiles) {

    // check file
    auto file = std::unique_ptr<TFile>(TFile::Open(filePath.c_str()));
    if (!file) {
      continue;
    } else if (file->IsZombie()) {
      continue;
    }

    // check tree
    auto tree = file->Get<TTree>(m_treeName.c_str());
    auto event = std::make_unique<xAOD::TEvent>();
    if (!tree || event->readFrom(tree).isFailure()) {
      continue;
      std::cout << "noo" << std::endl;
    }

    // get file entries
    long long fileEntries = event->getEntries();
    // add part to parts
    parts.emplace_back(offset, offset + fileEntries);
    offset += fileEntries;
  }

  return parts;
}

void EventData::initialize(unsigned int slot, unsigned long long,
                             unsigned long long) {
  m_event_per_slot[slot]->setActive();
  m_store_per_slot[slot]->setActive();
}

void EventData::execute(unsigned int slot, unsigned long long entry) {
  m_store_per_slot[slot]->clear();
  if (m_event_per_slot[slot]->getEntry(entry) < 0) {
    throw std::runtime_error("failed to get entry");
  }
}

void EventData::finalize(unsigned int) {
  // nothing to do
}