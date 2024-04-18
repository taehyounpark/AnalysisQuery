#pragma once

// local includes
#include "AsgMessaging/StatusCode.h"
#include <AsgMessaging/MessageCheck.h>

#include <SampleHandler/SampleHandler.h>
#include <EventLoop/StatusCode.h>

// jet reclustering and trimming
#include "AthContainers/ConstDataVector.h"
#include "AthContainers/DataVector.h"
#include "xAODEventInfo/EventInfo.h"

// CP interface includes
#include "PATInterfaces/ISystematicsTool.h"
#include "PATInterfaces/SystematicRegistry.h"
#include "PATInterfaces/SystematicSet.h"
#include "PATInterfaces/SystematicVariation.h"
#include "PATInterfaces/SystematicsUtil.h"

#include "PathResolver/PathResolver.h"

#include "queryosity.h"

#include <nlohmann/json.hpp>

namespace AnaQ {

using Settings = nlohmann::json;

template <typename Cont> using SystematicMap = std::unordered_map<std::string,Cont>;

struct SystematicMode {
  SystematicMode(const std::string& name, float value);
  ~SystematicMode() = default;
  CP::SystematicSet const& from(const CP::SystematicSet& inSysts) const;
  std::string name;
  float value;
};

template <typename Dec>
using EventDecision = qty::column::definition<Dec(xAOD::EventInfo)>;

template <typename T>
using Observable = qty::column::observable<T>;

template <typename T>
using Column = qty::column::definition<T>;

template <typename T>
using Query = qty::query::definition<T>;

namespace EventHelpers {

CP::SystematicSet
getSystematicVariation(const CP::SystematicSet& inSysts, const std::string& systName,
                     float systVal);

template <typename Cont> ConstDataVector<Cont> makeConstDataVector(Cont *cont);

} // namespace EventHelpers

}

template <typename Cont>
ConstDataVector<Cont> AnaQ::EventHelpers::makeConstDataVector(Cont* cont) {
  auto cdv = ConstDataVector<Cont>(SG::VIEW_ELEMENTS);
  cdv.reserve(cont->size());
  for (auto itr : *(cont)) {
    cdv.push_back(itr);
  }
  return cdv;
}