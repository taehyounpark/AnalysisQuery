#include "HepQuery/RDS.h"
#include "HepQuery/Tree.h"
#include "HepQuery/Hist.h"

#include "queryosity.h"

#ifdef __CINT__

#pragma link off all functions;
#pragma link off all classes;
#pragma link off all globals;

#pragma link C++ class HepQ::Hist < 1, float>;
#pragma link C++ class HepQ::Hist < 2, float>;
#pragma link C++ class HepQ::Hist < 3, float>;
#pragma link C++ class HepQ::Hist < 1, ROOT::RVec < float>>;
#pragma link C++ class HepQ::Hist < 2, ROOT::RVec < float>>;
#pragma link C++ class HepQ::Hist < 3, ROOT::RVec < float>>;

#pragma link C++ class HepQ::Tree;
#pragma link C++ class HepQ::Event;

#pragma link C++ class queryosity::dataflow;

#pragma link C++ nestedclass;
#pragma link C++ nestedtypedef;

#endif
