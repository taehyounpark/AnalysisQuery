#pragma once

// #include <AsgMessaging/StatusCode.h>
// #include <AsgMessaging/MessageCheck.h>

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

#include "Action.h"
#include <queryosity.hpp>

template <typename Dec, typename Cfg>
using EventDecision = EventFlow::Column<Dec(xAOD::EventInfo), Cfg>;

namespace EventHelpers {

template <typename Cont> ConstDataVector<Cont> makeConstDataVector(Cont *cont);

bool sortByPt(const xAOD::IParticle* partA, const xAOD::IParticle* partB);

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