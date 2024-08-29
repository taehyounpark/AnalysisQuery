#include "EventFlow/TreeData.h"

#include "TROOT.h"
#include "TTreeReader.h"
#include "TTreeReaderArray.h"
#include "TTreeReaderValue.h"

TreeData::TreeData(const std::vector<std::string> &inputFiles,
           const std::string &treeName)
    : m_inputFiles(inputFiles), m_treeName(treeName) {}

TreeData::TreeData(std::initializer_list<std::string> inputFiles,
           const std::string &treeName)
    : m_inputFiles(inputFiles), m_treeName(treeName) {}

void TreeData::parallelize(unsigned int nslots) {
  m_trees.resize(nslots);
  m_treeReaders.resize(nslots);
  for (unsigned int islot = 0; islot < nslots; ++islot) {
    auto tree =
        std::make_unique<TChain>(m_treeName.c_str(), m_treeName.c_str());
    tree->ResetBit(kMustCleanup);
    for (auto const &filePath : m_inputFiles) {
      tree->Add(filePath.c_str());
    }
    auto treeReader = std::make_unique<TTreeReader>(tree.get());
    m_trees[islot] = std::move(tree);
    m_treeReaders[islot] = std::move(treeReader);
  }
}

std::vector<std::pair<unsigned long long, unsigned long long>>
TreeData::partition() {
  ROOT::EnableThreadSafety();
  // ROOT::EnableImplicitMT(m_nslots);

  TDirectory::TContext c;
  std::vector<std::pair<unsigned long long, unsigned long long>> parts;

  // offset to account for global entry position
  long long offset = 0ll;
  for (auto const &filePath : m_inputFiles) {
    // check file
    std::unique_ptr<TFile> file(TFile::Open(filePath.c_str()));
    if (!file) {
      continue;
    } else if (file->IsZombie()) {
      continue;
    }

    // check tree
    auto tree = file->Get<TTree>(m_treeName.c_str());
    if (!tree) {
      continue;
    }

    // add tree clusters
    auto fileEntries = tree->GetEntries();
    auto clusterIterator = tree->GetClusterIterator(0);
    long long start = 0ll, end = 0ll;
    while ((start = clusterIterator.Next()) < fileEntries) {
      end = clusterIterator.GetNextEntry();
      parts.emplace_back(offset + start, offset + end);
    }
    // remember offset for next file
    offset += fileEntries;
  }

  return parts;
}

void TreeData::initialize(unsigned int slot, unsigned long long begin,
                      unsigned long long end) {
  m_treeReaders[slot]->SetEntriesRange(begin, end);
}

void TreeData::execute(unsigned int slot, unsigned long long entry) {
  m_treeReaders[slot]->SetEntry(entry);
}

void TreeData::finalize(unsigned int) {
  // m_treeReaders[slot].reset(nullptr);
}
