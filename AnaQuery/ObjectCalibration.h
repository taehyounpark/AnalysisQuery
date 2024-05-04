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

#include <queryosity.hpp>

namespace AnaQ {

template<typename Cont>
struct ShallowCopy
{
  ShallowCopy() = default;
  ShallowCopy(std::pair<Cont*, xAOD::ShallowAuxContainer*> containers);
  std::unique_ptr<Cont> elements;
  std::unique_ptr<xAOD::ShallowAuxContainer> aux;
};

template<typename Cont>
class ObjectCalibration : public Column<ConstDataVector<Cont>(Cont)>
{

public:
  ObjectCalibration(CP::SystematicVariation const& sysVar);
  ~ObjectCalibration() = default;

protected:
  CP::SystematicSet m_sysSet;
  mutable ShallowCopy<Cont> m_shallowCopy; //!
};

} // namespace AnaQ

template<typename Cont>
AnaQ::ShallowCopy<Cont>::ShallowCopy(
  std::pair<Cont*, xAOD::ShallowAuxContainer*> containers)
  : elements(containers.first)
  , aux(containers.second)
{
}

template<typename Cont>
AnaQ::ObjectCalibration<Cont>::ObjectCalibration(
  CP::SystematicVariation const& sysVar)
  : m_sysSet(std::vector<CP::SystematicVariation>{ sysVar })
{
}