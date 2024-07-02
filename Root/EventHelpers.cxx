#include "AnalysisQuery/EventHelpers.h"

std::vector<CP::SystematicSet>
EventHelpers::getSystematicVariation(const CP::SystematicSet &inSysts,
                                           const std::string &systMode,
                                           float varSigma) {
  std::vector<CP::SystematicSet> outSystList;
  if (systMode.empty()) {
    // nominal only
    outSystList.insert(outSystList.begin(), CP::SystematicSet());
    const CP::SystematicVariation nullVar = CP::SystematicVariation("");
    outSystList.back().insert(nullVar);
  } else {
    // find match from input systematics
    for (const auto &syst : inSysts) {
      // continue if not matched
      if (systMode == syst.basename()) {
        // found matching systematic -- use it
        if (syst == CP::SystematicVariation(
                        // continuous - apply with sigma value
                        syst.basename(), CP::SystematicVariation::CONTINUOUS)) {
          if (varSigma == 0) {
            throw std::logic_error("setting a continuous systematic variation "
                                   "value to 0.0 is nominal");
          }
          outSystList.push_back(CP::SystematicSet());
          outSystList.back().insert(
              CP::SystematicVariation(syst.basename(), varSigma));
        } else {
          // discrete -- apply as-is
          outSystList.push_back(CP::SystematicSet());
          outSystList.back().insert(syst);
        }
        // found match -- done
        break;
      }
    }
  }
  if (outSystList.empty()) {
    // no systematic found from input -- use nominal
    std::cout << "no systematic called '" << systMode
              << "' found -- setting it to nominal" << std::endl;
    outSystList.insert(outSystList.begin(), CP::SystematicSet());
    const CP::SystematicVariation nullVar = CP::SystematicVariation("");
    outSystList.back().insert(nullVar);
  }
  // there should only be one systematic found
  return outSystList;
}

SystematicMode::SystematicMode(const std::string &name, float value)
    : name(name), value(value) {}

std::vector<CP::SystematicSet>
SystematicMode::from(const CP::SystematicSet &inSysts) const {
  return EventHelpers::getSystematicVariation(inSysts, this->name, this->value);
}

bool EventHelpers::sortByPt(const xAOD::IParticle *partA,
                                  const xAOD::IParticle *partB) {
  return partA->pt() > partB->pt();
}

std::map<std::string, CP::SystematicVariation>
EventHelpers::makeSystematicVariationMap(Json const &sysCfg) {
  std::map<std::string, CP::SystematicVariation> varMap;
  for (auto const &[sys, vars] : sysCfg.items()) {
    for (auto const& var : vars.items()) {
      auto sysVar = CP::SystematicVariation(sys, var.value().get<float>());
      varMap.insert({sysVar.name(), sysVar});
    }
  }
  return varMap;
}