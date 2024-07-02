#pragma once

// local includes
#include "AsgMessaging/StatusCode.h"
#include <AsgMessaging/MessageCheck.h>
#include <RootCoreUtils/ThrowMsg.h>

// #include <SampleHandler/SampleHandler.h>
#include <EventLoop/StatusCode.h>

// jet reclustering and trimming
#include "AthContainers/ConstDataVector.h"
#include "AthContainers/DataVector.h"
#include "xAODBase/IParticleContainer.h"
#include "xAODBase/IParticleHelpers.h"
#include "xAODEventInfo/EventInfo.h"
#include "xAODTruth/TruthParticleContainer.h"

// CP interface includes
#include "PATInterfaces/ISystematicsTool.h"
#include "PATInterfaces/SystematicRegistry.h"
#include "PATInterfaces/SystematicSet.h"
#include "PATInterfaces/SystematicVariation.h"
#include "PATInterfaces/SystematicsUtil.h"

#include "PathResolver/PathResolver.h"

#include <queryosity.hpp>

#include <nlohmann/json.hpp>

#include <ROOT/RVec.hxx>

using Json = nlohmann::json;

template <typename Cont> using SystematicMap = std::unordered_map<std::string,Cont>;

struct SystematicMode {
  SystematicMode(const std::string& name, float value);
  ~SystematicMode() = default;
  std::vector<CP::SystematicSet> from(const CP::SystematicSet& inSysts) const;
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

using SystematicVariation = CP::SystematicVariation;

namespace EventHelpers {

std::vector<CP::SystematicSet>
getSystematicVariation(const CP::SystematicSet& inSysts, const std::string& systName,
                     float systVal);

template <typename Cont> ConstDataVector<Cont> makeConstDataVector(Cont *cont);

bool sortByPt(const xAOD::IParticle* partA, const xAOD::IParticle* partB);

std::map<std::string, CP::SystematicVariation> makeSystematicVariationMap(Json const& sysCfg);

template <typename Def, typename Nom>
std::map<std::string, qty::column::definition<Def>> varyColumn(Nom const& nomCfg, Json const& sysCfg);

} // namespace EventHelpers

template <typename Cont>
ConstDataVector<Cont> EventHelpers::makeConstDataVector(Cont* cont) {
  auto cdv = ConstDataVector<Cont>(SG::VIEW_ELEMENTS);
  cdv.reserve(cont->size());
  for (auto itr : *(cont)) {
    cdv.push_back(itr);
  }
  return cdv;
}

template <typename Def, typename Nom>
std::map<std::string, qty::column::definition<Def>> EventHelpers::varyColumn(Nom const& nomCfg, Json const& sysCfg) {
  std::map<std::string, qty::column::definition<Def>> colVarMap;
  auto varMap = makeSystematicVariationMap(sysCfg);
  for (auto const& [name, sysVar] : varMap) {
    colVarMap.insert({name, qty::column::definition<Def>(nomCfg, sysVar)});
  }
  return colVarMap;
}