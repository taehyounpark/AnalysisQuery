#pragma once

#include "AsgMessaging/StatusCode.h"
#include <AsgMessaging/MessageCheck.h>

#include "AthContainers/ConstDataVector.h"
#include "AthContainers/DataVector.h"
#include "xAODBase/IParticleContainer.h"
#include "xAODBase/IParticleHelpers.h"
#include "xAODEventInfo/EventInfo.h"
#include "xAODTruth/TruthParticleContainer.h"

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

template <typename Dec>
using EventDecision = qty::column::definition<Dec(xAOD::EventInfo)>;

template <typename T>
using Column = qty::column::definition<T>;

namespace EventHelpers {

template <typename Cont> ConstDataVector<Cont> getConstDataVector(Cont *cont);

bool sortByPt(const xAOD::IParticle* partA, const xAOD::IParticle* partB);

} // namespace EventHelpers

template <typename Cont>
ConstDataVector<Cont> EventHelpers::getConstDataVector(Cont* cont) {
  auto cdv = ConstDataVector<Cont>(SG::VIEW_ELEMENTS);
  cdv.reserve(cont->size());
  for (auto itr : *(cont)) {
    cdv.push_back(itr);
  }
  return cdv;
}