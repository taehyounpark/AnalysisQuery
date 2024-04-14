#pragma once

#include <memory>
#include <string>
#include <utility>
#include <vector>

#include <ROOT/RDataSource.hxx>
#include <ROOT/RVec.hxx>

#include "queryosity.h"

namespace HepQ {

class RDS : public queryosity::dataset::reader<RDS> {

private:
  using RDataSource = ROOT::RDF::RDataSource;

public:
  template <typename T> class Column;

public:
  RDS(std::unique_ptr<RDataSource> rds);
  ~RDS() = default;

  virtual void parallelize(unsigned int) final override;

  virtual std::vector<std::pair<unsigned long long, unsigned long long>>
  partition() final override;

  template <typename T>
  std::unique_ptr<Column<T>> read(unsigned int, const std::string &);

  virtual void initialize() final override;
  virtual void initialize(unsigned int slot, unsigned long long begin,
                          unsigned long long end) final override;
  virtual void execute(unsigned int slot, unsigned long long entry) final override;
  virtual void finalize(unsigned int slot) final override;
  virtual void finalize() final override;

protected:
  std::unique_ptr<RDataSource> m_rds;
};

template <typename T> class RDS::Column : public queryosity::column::reader<T> {

public:
  Column(T **cursor) : m_cursor(cursor) {}
  ~Column() = default;

  virtual const T &read(unsigned int, unsigned long long) const final override {
    return static_cast<const T &>(**m_cursor);
  }

protected:
  T **m_cursor;
};

}

template <typename T>
std::unique_ptr<HepQ::RDS::Column<T>> HepQ::RDS::read(unsigned int slot,
                                          const std::string &name) {
  auto columnReaders = m_rds->GetColumnReaders<T>(name.c_str());
  return std::make_unique<Column<T>>(columnReaders[slot]);
}
