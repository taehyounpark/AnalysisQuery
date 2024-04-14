#pragma once

// local includes
#include "AsgMessaging/StatusCode.h"
#include <AsgMessaging/MessageCheck.h>

#include <SampleHandler/SampleHandler.h>

// jet reclustering and trimming
#include "xAODEventInfo/EventInfo.h"
#include "AthContainers/ConstDataVector.h"
#include "AthContainers/DataVector.h"

// CP interface includes
#include "PATInterfaces/SystematicRegistry.h"
#include "PATInterfaces/SystematicSet.h"
#include "PATInterfaces/SystematicsUtil.h"
#include "PATInterfaces/SystematicVariation.h"
#include "PATInterfaces/ISystematicsTool.h"

#include "queryosity.h"

template <typename Cont> using SystematicMap = std::unordered_map<std::string,Cont>;

template <typename Dec>
using EventDecision = qty::column::definition<Dec(xAOD::EventInfo)>;

namespace EventHelpers {

  std::vector< CP::SystematicSet > getListofSystematics(const CP::SystematicSet inSysts, std::string systNames, float systVal );

  template <typename Cont>
  ConstDataVector<Cont> makeConstDataVector(Cont* cont);

}

template <typename Cont>
ConstDataVector<Cont> EventHelpers::makeConstDataVector(Cont* cont) {
  auto cdv = ConstDataVector<Cont>(SG::VIEW_ELEMENTS);
  cdv.reserve( cont->size() );
  for (auto itr : *(cont)) { cdv.push_back(itr); }
  return cdv;
}