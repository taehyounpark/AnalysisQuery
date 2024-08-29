#pragma once

#include <memory>
#include <string>
#include <vector>

#include <ROOT/RVec.hxx>

#include "TChain.h"
#include "TDirectory.h"
#include "TFile.h"
#include "TROOT.h"
#include "TTree.h"
#include "TTreeReader.h"
#include "TTreeReaderArray.h"
#include "TTreeReaderValue.h"

#include <queryosity.hpp>

class TreeData : public queryosity::dataset::reader<TreeData> {

public:
  class Reader;

  template <typename T> class Branch;

public:
  TreeData(const std::vector<std::string> &filePaths, const std::string &treeName);
  TreeData(std::initializer_list<std::string> filePaths,
       const std::string &treeName);
  virtual ~TreeData() = default;

  virtual void parallelize(unsigned int nslots) final override;

  virtual std::vector<std::pair<unsigned long long, unsigned long long>>
  partition() final override;

  template <typename U>
  std::unique_ptr<Branch<U>> read(unsigned int slot,
                                  const std::string &branchName);

  virtual void initialize(unsigned int slot, unsigned long long begin,
                          unsigned long long end) final override;
  virtual void execute(unsigned int slot, unsigned long long entry) final override;
  virtual void finalize(unsigned int slot) final override;
  using queryosity::dataset::source::initialize;
  using queryosity::dataset::source::finalize;

protected:
  std::vector<std::string> m_inputFiles;
  std::string m_treeName;

  std::vector<std::unique_ptr<TTree>> m_trees;             //!
  std::vector<std::unique_ptr<TTreeReader>> m_treeReaders; //!
};

template <typename T>
class TreeData::Branch : public queryosity::column::reader<T> {

public:
  Branch(const std::string &branchName, TTreeReader &treeReader)
      : m_branchName(branchName) {
    m_treeReaderValue = std::make_unique<TTreeReaderValue<T>>(
        treeReader, this->m_branchName.c_str());
  }
  ~Branch() = default;

  virtual T const &read(unsigned int, unsigned long long) const final override {
    return **m_treeReaderValue;
  }

protected:
  std::string m_branchName;
  std::unique_ptr<TTreeReaderValue<T>> m_treeReaderValue;
};

template <typename T>
class TreeData::Branch<ROOT::RVec<T>>
    : public queryosity::column::reader<ROOT::RVec<T>> {

public:
  Branch(const std::string &branchName, TTreeReader &treeReader)
      : m_branchName(branchName) {
    m_treeReaderArray = std::make_unique<TTreeReaderArray<T>>(
        treeReader, this->m_branchName.c_str());
  }
  ~Branch() = default;

  virtual void initialize(unsigned int, unsigned long long,
                          unsigned long long) final override {}

  virtual ROOT::RVec<T> const &read(unsigned int,
                                    unsigned long long) const final override {
    if (auto arraySize = m_treeReaderArray->GetSize()) {
      ROOT::RVec<T> readArray(&m_treeReaderArray->At(0), arraySize);
      std::swap(m_readArray, readArray);
    } else {
      ROOT::RVec<T> emptyVector{};
      std::swap(m_readArray, emptyVector);
    }
    return m_readArray;
  }

protected:
  std::string m_branchName;
  std::unique_ptr<TTreeReaderArray<T>> m_treeReaderArray;
  mutable ROOT::RVec<T> m_readArray;
};

template <>
class TreeData::Branch<ROOT::RVec<bool>>
    : public queryosity::column::reader<ROOT::RVec<bool>> {

public:
  Branch(const std::string &branchName, TTreeReader &treeReader)
      : m_branchName(branchName) {
    m_treeReaderArray = std::make_unique<TTreeReaderArray<bool>>(
        treeReader, this->m_branchName.c_str());
  }
  ~Branch() = default;

  virtual ROOT::RVec<bool> const &read(unsigned int,
                                       unsigned long long) const final override {
    if (m_treeReaderArray->GetSize()) {
      ROOT::RVec<bool> readArray(m_treeReaderArray->begin(),
                                 m_treeReaderArray->end());
      std::swap(m_readArray, readArray);
    } else {
      ROOT::RVec<bool> emptyVector{};
      std::swap(m_readArray, emptyVector);
    }
    return m_readArray;
  }

protected:
  std::string m_branchName;
  std::unique_ptr<TTreeReaderArray<bool>> m_treeReaderArray;
  mutable ROOT::RVec<bool> m_readArray;
};

template <typename U>
std::unique_ptr<TreeData::Branch<U>> TreeData::read(unsigned int slot,
                                            const std::string &branchName) {
  return std::make_unique<Branch<U>>(branchName, *m_treeReaders[slot]);
}
