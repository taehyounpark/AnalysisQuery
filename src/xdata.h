#pragma once

#include "AthAnalysisBaseComps/AthAnalysisAlgorithm.h"

#include <queryosity.hpp>

#include <memory>

template <typename T>
class xcolumn;

class xdata : public queryosity::dataset::reader<xdata>, public AthAnalysisAlgorithm {

public:
  xdata(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~xdata();

  virtual void parallelize(unsigned int) final override;
  virtual std::vector<std::pair<unsigned long long, unsigned long long>>
  partition() final override;

  virtual StatusCode execute() override;

  template <typename T>
  std::unique_ptr<xcolumn<T>> read(unsigned int slot,
                                     const std::string &name) const;

};

#include "xcolumn.h"

template <typename T>
std::unique_ptr<xcolumn<T>> xdata::read(unsigned int slot, const std::string& name) const {
  return std::make_unique<xcolumn<T>>(name, evtStore(), detStore());
}