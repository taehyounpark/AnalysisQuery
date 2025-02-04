#include "xanalysis.h"

xanalysis::xanalysis(const std::string& name,
                                               ISvcLocator* pSvcLocator)
    : AthAnalysisAlgorithm(name, pSvcLocator),
  m_df(),
  m_ds(m_df.load(qty::dataset::input<xdata>(std::string(name+"_data"), pSvcLocator)))
{}

xanalysis::~xanalysis() {}

StatusCode xanalysis::initialize() {
  analyze(m_df, m_ds);
  return StatusCode::SUCCESS;
}

StatusCode xanalysis::beginInputFile() {
  //
  // This method is called at the start of each input file, even if
  // the input file contains no events. Accumulate metadata information here
  //

  // example of retrieval of CutBookkeepers: (remember you will need to include
  // the necessary header files and use statements in requirements file)
  //  const xAOD::CutBookkeeperContainer* bks = 0;
  //  CHECK( inputMetaStore()->retrieve(bks, "CutBookkeepers") );

  // example of IOVMetaData retrieval (see
  // https://twiki.cern.ch/twiki/bin/viewauth/AtlasProtected/AthAnalysisBase#How_to_access_file_metadata_in_C)
  // float beamEnergy(0); CHECK(
  // retrieveMetadata("/TagInfo","beam_energy",beamEnergy) ); std::vector<float>
  // bunchPattern; CHECK(
  // retrieveMetadata("/Digitiation/Parameters","BeamIntensityPattern",bunchPattern)
  // );

  return StatusCode::SUCCESS;
}

StatusCode xanalysis::execute() {
  for (const auto& player : m_df.m_processor.get_slots()) {
    player->execute(0,0);
  }
  return StatusCode::SUCCESS;
}

StatusCode xanalysis::finalize() {
  for (const auto& player : m_df.m_processor.get_slots()) {
    player->finalize(0);
  }
  return StatusCode::SUCCESS;
}

void xanalysis::analyze(qty::dataflow& df, qty::dataflow::input<xdata>& ds) {
}