#pragma once

#include "AthAnalysisBaseComps/AthAnalysisAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"
#include "StoreGate/StoreGateSvc.h"

#include "TString.h"

#include <queryosity.hpp>

template <typename T>
class xcolumn : public queryosity::column::reader<T> {

public:
  xcolumn(const std::string &name, ServiceHandle<StoreGateSvc>& evtStore, ServiceHandle<StoreGateSvc>& detStore)
    : m_name(name), m_evtStore(evtStore), m_detStore(detStore) {}
  ~xcolumn() = default;

  virtual T const &read(unsigned int, unsigned long long) const final override {
    T const *container(nullptr);
    if (m_evtStore->retrieve(container, this->m_name).isFailure()) {
      if (m_detStore->retrieve(container, this->m_name).isFailure()) {
        throw std::runtime_error(TString::Format(
            "failed to retrieve '%s' from event", this->m_name.c_str()));
      }
    }
    return *container;
  }

protected:
  std::string m_name;
  ServiceHandle<StoreGateSvc>& m_evtStore;
  ServiceHandle<StoreGateSvc>& m_detStore;

};