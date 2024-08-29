#include "EventFlow/TreeData.h"
#include "EventFlow/EventData.h"
#include "EventFlow/Hist.h"
#include "EventFlow/Analysis.h"

#include <queryosity.hpp>

#ifdef __CLING__

#pragma link off all functions;
#pragma link off all classes;
#pragma link off all globals;

#pragma link C++ nestedclasses;
#pragma link C++ nestedtypedefs;

#pragma link C++ class queryosity::dataflow;

#pragma link C++ class Hist < 1, float>;
#pragma link C++ class Hist < 2, float>;
#pragma link C++ class Hist < 3, float>;
#pragma link C++ class Hist < 1, ROOT::RVec < float>>;
#pragma link C++ class Hist < 2, ROOT::RVec < float>>;
#pragma link C++ class Hist < 3, ROOT::RVec < float>>;

#pragma link C++ class EventData;
#pragma link C++ class TreeData;
#pragma link C++ class TreeDump;

#endif
