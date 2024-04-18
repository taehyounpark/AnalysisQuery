#include "AnaQuery/EventHelpers.h"

CP::SystematicSet AnaQ::EventHelpers::getSystematicVariation(const CP::SystematicSet& inSysts, const std::string& systMode, float varSigma) {
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
            throw std::logic_error("setting a continuous systematic variation value to 0.0 is nominal");
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
    std::cout << "no systematic called '" << systMode <<"' found -- setting it to nominal" << std::endl;
    outSystList.insert(outSystList.begin(), CP::SystematicSet());
    const CP::SystematicVariation nullVar = CP::SystematicVariation("");
    outSystList.back().insert(nullVar);
  }
  // there should only be one systematic found
  return outSystList[0];
}

AnaQ::SystematicMode::SystematicMode(const std::string& name, float value) : name(name), value(value) {}

CP::SystematicSet const& AnaQ::SystematicMode::from(const CP::SystematicSet& inSysts) const {
  return EventHelpers::getSystematicVariation(inSysts, this->name, this->value);
}