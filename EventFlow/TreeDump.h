#pragma once

#include <queryosity.hpp>

#include <memory>
#include <string>
#include <vector>

#include <ROOT/RVec.hxx>

#include "TTree.h"

namespace EventFlow 
{

template <typename... ColumnTypes>
class TreeDump
    : public qty::query::definition<std::shared_ptr<TTree>(
          ColumnTypes...)> {

public:
  static constexpr size_t N = sizeof...(ColumnTypes);
  using BranchArray_t = std::array<TBranch *, N>;
  using ColumnTuple_t = std::tuple<ColumnTypes...>;

public:
  template <typename... Names>
  TreeDump(const std::string &treeName, Names const &...branchNames);
  TreeDump(const std::string &treeName, std::vector<std::string> const& branchNames);
  virtual ~TreeDump() = default;

  virtual void fill(qty::column::observable<ColumnTypes>...,
                    double) final override;
  virtual std::shared_ptr<TTree> result() const final override;
  virtual std::shared_ptr<TTree>
  merge(std::vector<std::shared_ptr<TTree>> const &results) const final override;

private:
  // Helper function to convert a vector to a tuple (for known size at runtime)
  template<std::size_t... I>
  auto makeBranchNamesTupleImpl(const std::vector<std::string>& v, std::index_sequence<I...>) {
    return std::make_tuple(v[I]...);
  }

  auto makeBranchNamesTuple(const std::vector<std::string>& v) {
    assert(v.size() == N && "number of branch names must exactly match that of output columns");
    return makeBranchNamesTupleImpl(v, std::make_index_sequence<N>{});
  }

  // helper function to make branch of i-th data type with i-th column name
  template <typename T, std::size_t I>
  TBranch *makeBranch(const std::string &name) {
    return m_snapshot->Branch<T>(name.c_str(), &std::get<I>(m_columns));
  }

  template <std::size_t I, typename T>
  void fillBranch(const qty::column::observable<T> &column) {
    std::get<I>(m_columns) = column.value();
  }

  // expand the packs ColumnTypes, Indices, and branchNames simultaneously
  template <std::size_t... Is, typename... Names>
  BranchArray_t makeBranches(std::index_sequence<Is...>,
                             Names const &...branchNames) {
    return {{makeBranch<ColumnTypes, Is>(branchNames)...}};
  }

  template <std::size_t... Is, typename... Observables>
  void fillBranches(std::index_sequence<Is...>, Observables... columns) {
    (fillBranch<Is>(columns), ...);
  }

protected:
  std::shared_ptr<TTree> m_snapshot; //!
  BranchArray_t m_branches;
  ColumnTuple_t m_columns;
};

}

template <typename... ColumnTypes>
template <typename... Names>
EventFlow::TreeDump<ColumnTypes...>::TreeDump(const std::string &treeName,
                                         Names const &...branchNames)
    : m_snapshot(std::make_shared<TTree>(treeName.c_str(), treeName.c_str())),
      m_branches(makeBranches(std::index_sequence_for<ColumnTypes...>(),
                              branchNames...)) {
  m_snapshot->SetDirectory(0);
}

template <typename... ColumnTypes>
EventFlow::TreeDump<ColumnTypes...>::TreeDump(const std::string &treeName,
                                         std::vector<std::string> const & branchNames)
    : m_snapshot(std::make_shared<TTree>(treeName.c_str(), treeName.c_str())),
      m_branches(std::apply([this](auto&&... args) {
            return makeBranches(std::index_sequence_for<ColumnTypes...>(), args...);
        }, makeBranchNamesTuple(branchNames))) {
  m_snapshot->SetDirectory(0);
}


template <typename... ColumnTypes>
void EventFlow::TreeDump<ColumnTypes...>::fill(
    qty::column::observable<ColumnTypes>... columns, double) {
  this->fillBranches(std::index_sequence_for<ColumnTypes...>(), columns...);
  m_snapshot->Fill();
}

template <typename... ColumnTypes>
std::shared_ptr<TTree> EventFlow::TreeDump<ColumnTypes...>::result() const {
  return m_snapshot;
}

template <typename... ColumnTypes>
std::shared_ptr<TTree> EventFlow::TreeDump<ColumnTypes...>::merge(
    std::vector<std::shared_ptr<TTree>> const &results) const {
  TList list;
  for (auto const &result : results) {
    list.Add(result.get());
  }
  auto merged = std::shared_ptr<TTree>(TTree::MergeTrees(&list));
  merged->SetDirectory(0);
  return merged;
}
