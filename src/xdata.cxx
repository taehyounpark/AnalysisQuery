#include "xdata.h"

xdata::xdata(const std::string& name, ISvcLocator* pSvcLocator)
    : AthAnalysisAlgorithm(name, pSvcLocator) {}

xdata::~xdata() {}

void xdata::parallelize(unsigned int nslots) {}

std::vector<std::pair<unsigned long long, unsigned long long>> xdata::partition() {
  return {{0,0}};
}

StatusCode xdata::execute() {
  return StatusCode::SUCCESS;
}
