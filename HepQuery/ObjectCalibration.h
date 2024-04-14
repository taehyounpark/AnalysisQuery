#pragma once

#include "EventHelpers.h"

// CP interface includes
#include "PATInterfaces/SystematicRegistry.h"
#include "PATInterfaces/SystematicSet.h"
#include "PATInterfaces/SystematicsUtil.h"
#include "PATInterfaces/SystematicVariation.h"

#include "xAODCore/ShallowCopy.h"
#include "xAODEventInfo/EventInfo.h"
#include "AthContainers/ConstDataVector.h"
#include "AthContainers/DataVector.h"
#include "xAODBase/IParticleHelpers.h"
#include "xAODBase/IParticleContainer.h"
#include "xAODBase/IParticle.h"

#include "queryosity.h"

namespace HepQ {

template <typename Cont>
struct ShallowCopy {
  ShallowCopy() = default;
  ShallowCopy(std::pair<Cont*, xAOD::ShallowAuxContainer*> containers);
  std::unique_ptr<Cont> elements;
  std::unique_ptr<xAOD::ShallowAuxContainer> aux;
};

template <typename Cont>
class ObjectCalibration : public qty::column::definition<SystematicMap<ConstDataVector<Cont>>(Cont)>
{

private:

public:
  ObjectCalibration() = default;

protected:
  std::string m_outAuxContainerName;
  std::string m_outSCContainerName;
  std::string m_outSCAuxContainerName;

  std::vector<CP::SystematicSet> m_systList; //!
  std::vector<std::string> m_systNames; //!

  mutable std::unordered_map<std::string, ShallowCopy<Cont>> m_shallowCopies; //!

  // ClassDef(ObjectCalibration, 1);
};

}

template <typename Cont>
HepQ::ShallowCopy<Cont>::ShallowCopy(std::pair<Cont*, xAOD::ShallowAuxContainer*> containers) :
  elements(containers.first), aux(containers.second) {}
