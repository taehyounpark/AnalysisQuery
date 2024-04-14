#include "AnaQuery/RDS.h"
#include "AnaQuery/Tree.h"
#include "AnaQuery/Hist.h"

#include "AnaQuery/ElectronCalibration.h"

#include "queryosity.h"

#ifdef __CINT__

#pragma link off all functions;
#pragma link off all classes;
#pragma link off all globals;

#pragma link C++ class queryosity::dataflow;

#pragma link C++ class AnaQ::Hist < 1, float>;
#pragma link C++ class AnaQ::Hist < 2, float>;
#pragma link C++ class AnaQ::Hist < 3, float>;
#pragma link C++ class AnaQ::Hist < 1, ROOT::RVec < float>>;
#pragma link C++ class AnaQ::Hist < 2, ROOT::RVec < float>>;
#pragma link C++ class AnaQ::Hist < 3, ROOT::RVec < float>>;

#pragma link C++ class AnaQ::Tree;
#pragma link C++ class AnaQ::Event;

#pragma link C++ class AnaQ::ElectronCalibration+;

#pragma link C++ nestedclass;
#pragma link C++ nestedtypedef;

#endif
