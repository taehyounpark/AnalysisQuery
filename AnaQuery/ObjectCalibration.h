#pragma once

#include "EventHelpers.h"

// CP interface includes
#include "PATInterfaces/SystematicRegistry.h"
#include "PATInterfaces/SystematicSet.h"
#include "PATInterfaces/SystematicVariation.h"
#include "PATInterfaces/SystematicsUtil.h"

#include "AthContainers/ConstDataVector.h"
#include "AthContainers/DataVector.h"
#include "xAODBase/IParticle.h"
#include "xAODBase/IParticleContainer.h"
#include "xAODBase/IParticleHelpers.h"
#include "xAODCore/ShallowCopy.h"
#include "xAODEventInfo/EventInfo.h"

#include "queryosity.h"

namespace AnaQ {

template <typename Cont> struct ShallowCopy {
  ShallowCopy() = default;
  ShallowCopy(std::pair<Cont *, xAOD::ShallowAuxContainer *> containers);
  std::unique_ptr<Cont> elements;
  std::unique_ptr<xAOD::ShallowAuxContainer> aux;
};

template <typename Cont>
class ObjectCalibration
    : public qty::column::definition<ConstDataVector<Cont>(
          Cont)> {

private:
public:
  ObjectCalibration() = default;

protected:
  // std::string m_outAuxContainerName;
  // std::string m_outSCContainerName;
  // std::string m_outSCAuxContainerName;

  CP::SystematicSet m_systVar; //!
  mutable ShallowCopy<Cont>
      m_shallowCopy; //!

  // ClassDef(ObjectCalibration, 1);
};

} // namespace AnaQ

template <typename Cont>
AnaQ::ShallowCopy<Cont>::ShallowCopy(
    std::pair<Cont *, xAOD::ShallowAuxContainer *> containers)
    : elements(containers.first), aux(containers.second) {}
