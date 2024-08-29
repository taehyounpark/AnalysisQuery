#pragma once

#include "EventHelpers.h"

#include "PATInterfaces/ISystematicsTool.h"
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

#include "Action.h"
#include <queryosity.hpp>

template <typename Cont> struct ShallowCopy {
  ShallowCopy() = default;
  ShallowCopy(std::pair<Cont *, xAOD::ShallowAuxContainer *> shallowCopyContainers);
  std::unique_ptr<Cont> elementContainer;
  std::unique_ptr<xAOD::ShallowAuxContainer> auxContainer;
};

template <typename Cont, typename Cfg>
class ObjectCalibration : public EventFlow::Column<ConstDataVector<Cont>(Cont), Cfg> {

public:
  ObjectCalibration(Cfg const &cfg);
  ObjectCalibration(Cfg const &cfg, CP::SystematicVariation const &var);
  ~ObjectCalibration() = default;

  void keepShallowCopy(std::pair<Cont *, xAOD::ShallowAuxContainer *> shallowCopyContainers) const;
  void freeShallowCopy() const;

protected:
  mutable ShallowCopy<Cont> m_shallowCopy; //!
};

template <typename Cont>
ShallowCopy<Cont>::ShallowCopy(
    std::pair<Cont *, xAOD::ShallowAuxContainer *> shallowCopyContainers)
    : elementContainer(shallowCopyContainers.first), auxContainer(shallowCopyContainers.second) {}

template <typename Cont, typename Cfg>
ObjectCalibration<Cont, Cfg>::ObjectCalibration(Cfg const &cfg)
    : EventFlow::Column<ConstDataVector<Cont>(Cont), Cfg>(cfg) {}

template <typename Cont, typename Cfg>
ObjectCalibration<Cont, Cfg>::ObjectCalibration(
    Cfg const &cfg, CP::SystematicVariation const &var)
    : EventFlow::Column<ConstDataVector<Cont>(Cont), Cfg>(cfg, var) {}

template <typename Cont, typename Cfg>
void ObjectCalibration<Cont, Cfg>::keepShallowCopy(std::pair<Cont *, xAOD::ShallowAuxContainer *> shallowCopyContainers) const {
  this->m_shallowCopy = ShallowCopy(shallowCopyContainers);
}

template <typename Cont, typename Cfg>
void ObjectCalibration<Cont, Cfg>::freeShallowCopy() const {
  this->m_shallowCopy = {};
}