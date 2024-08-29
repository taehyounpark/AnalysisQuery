#include "EventFlow/EventHelpers.h"

bool EventHelpers::sortByPt(const xAOD::IParticle *partA,
                                  const xAOD::IParticle *partB) {
  return partA->pt() > partB->pt();
}