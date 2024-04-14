#include "AnaQuery/EventHelpers.h"

std::vector< CP::SystematicSet > AnaQ::EventHelpers::getListofSystematics(const CP::SystematicSet inSysts, std::string systNames, float systVal ) {

  std::vector< CP::SystematicSet > outSystList;

  // parse and split by comma
  std::vector<std::string> systNamesList;
  std::string token;
  std::istringstream ss(systNames);
  while (std::getline(ss, token, ',')) {
    systNamesList.push_back(token);
  }

  // msg << MSG::DEBUG << "systNames: " << endmsg;
  for ( const std::string &name : systNamesList ) {
    // msg << MSG::DEBUG << "\t" << name << endmsg;
  }

  // loop over input set
  //
  for ( const auto& syst : inSysts ) {

    // msg << MSG::DEBUG << syst.name() << endmsg;

    // 1.
    // input systName does not contain "All":
    // match with input systName(s) from the list:
    // add these systematics only to the output list
    //
    if ( systNames.find("All") == std::string::npos ) {
      // do systNames vector matching
      bool valid = false;
      for ( const auto& s : systNamesList ) {
        if ( s == syst.basename() ) {
          valid = true;
          break;
        }
      }

      // continue if not matched
      if ( !valid ) continue;

      // msg << MSG::DEBUG << "Found match! Adding systematic " << syst.name() << endmsg;

      // continuous systematics - can choose at what sigma to evaluate
      //
      if ( syst == CP::SystematicVariation (syst.basename(), CP::SystematicVariation::CONTINUOUS) ) {

        outSystList.push_back(CP::SystematicSet());

        if ( systVal == 0 ) {
          // msg << MSG::ERROR << "Setting continuous systematic to 0 is nominal! Please check!" << endmsg;
          // RCU_THROW_MSG("Failure");
        }

        outSystList.back().insert(CP::SystematicVariation (syst.basename(), systVal));
        outSystList.push_back(CP::SystematicSet());
        outSystList.back().insert(CP::SystematicVariation (syst.basename(), -1.0*fabs(systVal)));

      } else {
      // not a continuous system

        outSystList.push_back(CP::SystematicSet());
        outSystList.back().insert(syst);

      }
    }
    // 2.
    // input systName contains "All":
    // add all systematics to the output list
    //
    else if ( systNames.find("All") != std::string::npos ) {

      // msg << MSG::DEBUG << "Adding systematic " << syst.name() << endmsg;

      // continuous systematics - can choose at what sigma to evaluate
      // add +1 and -1 for when running all
      //
      if ( syst == CP::SystematicVariation (syst.basename(), CP::SystematicVariation::CONTINUOUS) ) {

      if ( systVal == 0 ) {
        // msg << MSG::ERROR << "Setting continuous systematic to 0 is nominal! Please check!" << endmsg;
        // RCU_THROW_MSG("Failure");
      }

      outSystList.push_back(CP::SystematicSet());
      outSystList.back().insert(CP::SystematicVariation (syst.basename(),  fabs(systVal)));
      outSystList.push_back(CP::SystematicSet());
      outSystList.back().insert(CP::SystematicVariation (syst.basename(), -1.0*fabs(systVal)));

      } else {
      // not a continuous systematic

        outSystList.push_back(CP::SystematicSet());
        outSystList.back().insert(syst);

      }

    }

  } // loop over recommended systematics

  // Add an empty CP::SystematicVariation at the top of output list to account for the nominal case
  // when running on all systematics or on nominal only
  //
  if ( systNames.find("Nominal") != std::string::npos || systNames.find("All") != std::string::npos || systNames.empty() ) {
    outSystList.insert( outSystList.begin(), CP::SystematicSet() );
    const CP::SystematicVariation nullVar = CP::SystematicVariation("");
    outSystList.back().insert(nullVar);
  }

  return outSystList;

}