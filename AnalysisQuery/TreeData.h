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

  template <typename... ColumnTypes> class Snapshot;

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

template <typename... ColumnTypes>
class TreeData::Snapshot
    : public queryosity::query::definition<std::shared_ptr<TTree>(
          ColumnTypes...)> {

public:
  static constexpr size_t N = sizeof...(ColumnTypes);
  using BranchArray_t = std::array<TBranch *, N>;
  using ColumnTuple_t = std::tuple<ColumnTypes...>;

public:
  template <typename... Names>
  Snapshot(const std::string &treeName, Names const &...columnNames);
  virtual ~Snapshot() = default;

  virtual void fill(queryosity::column::observable<ColumnTypes>...,
                    double) final override;
  virtual std::shared_ptr<TTree> result() const final override;
  virtual std::shared_ptr<TTree>
  merge(std::vector<std::shared_ptr<TTree>> const &results) const final override;

private:
  // helper function to make branch of i-th data type with i-th column name
  template <typename T, std::size_t I>
  TBranch *makeBranch(const std::string &name) {
    return m_snapshot->Branch<T>(name.c_str(), &std::get<I>(m_columns));
  }

  template <std::size_t I, typename T>
  void fillBranch(const queryosity::column::observable<T> &column) {
    std::get<I>(m_columns) = column.value();
  }

  // expand the packs ColumnTypes, Indices, and columnNames simultaneously
  template <std::size_t... Is, typename... Names>
  BranchArray_t makeBranches(std::index_sequence<Is...>,
                             Names const &...columnNames) {
    return {{makeBranch<ColumnTypes, Is>(columnNames)...}};
  }

  template <std::size_t... Is, typename... Observables>
  void fillBranchs(std::index_sequence<Is...>, Observables... columns) {
    (fillBranch<Is>(columns), ...);
  }

protected:
  std::shared_ptr<TTree> m_snapshot; //!
  BranchArray_t m_branches;
  ColumnTuple_t m_columns;
};

template <typename U>
std::unique_ptr<TreeData::Branch<U>> TreeData::read(unsigned int slot,
                                            const std::string &branchName) {
  return std::make_unique<Branch<U>>(branchName, *m_treeReaders[slot]);
}

template <typename... ColumnTypes>
template <typename... Names>
TreeData::Snapshot<ColumnTypes...>::Snapshot(const std::string &treeName,
                                         Names const &...columnNames)
    : m_snapshot(std::make_shared<TTree>(treeName.c_str(), treeName.c_str())),
      m_branches(makeBranches(std::index_sequence_for<ColumnTypes...>(),
                              columnNames...)) {
  m_snapshot->SetDirectory(0);
}

template <typename... ColumnTypes>
void TreeData::Snapshot<ColumnTypes...>::fill(
    queryosity::column::observable<ColumnTypes>... columns, double) {
  this->fillBranchs(std::index_sequence_for<ColumnTypes...>(), columns...);
  m_snapshot->Fill();
}

template <typename... ColumnTypes>
std::shared_ptr<TTree> TreeData::Snapshot<ColumnTypes...>::result() const {
  return m_snapshot;
}

template <typename... ColumnTypes>
std::shared_ptr<TTree> TreeData::Snapshot<ColumnTypes...>::merge(
    std::vector<std::shared_ptr<TTree>> const &results) const {
  TList list;
  for (auto const &result : results) {
    list.Add(result.get());
  }
  auto merged = std::shared_ptr<TTree>(TTree::MergeTrees(&list));
  merged->SetDirectory(0);
  return merged;
}