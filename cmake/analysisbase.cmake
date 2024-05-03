project(AnaQuery)

find_package(
  ROOT REQUIRED
  COMPONENTS Core
             Imt
             RIO
             Net
             Hist
             Graf
             Graf3d
             Gpad
             ROOTVecOps
             Tree
             TreePlayer
             Postscript
             Matrix
             Physics
             MathCore
             Thread
             MultiProc
             ROOTDataFrame)
find_package(AnalysisBase QUIET)
find_package(nlohmann_json REQUIRED)

atlas_subdir(AnaQuery)

atlas_add_root_dictionary(
  AnaQueryLib
  AnaQueryDict
  ROOT_HEADERS
  AnaQuery/*.h
  Root/LinkDef.h
  EXTERNAL_PACKAGES
  ROOT)

atlas_add_library(
  AnaQueryLib
  AnaQuery/*.h
  Root/*.h
  Root/*.cxx
  ${AnaQueryDict}
  PUBLIC_HEADERS 
  AnaQuery
  INCLUDE_DIRS
  ${ROOT_INCLUDE_DIRS}
  LINK_LIBRARIES
  queryosity::queryosity
  nlohmann_json::nlohmann_json
  ${ROOT_LIBRARIES}
  AthContainers
  AsgTools
  AnaAlgorithmLib
  EventLoop
  xAODBase
  xAODRootAccess
  xAODEventInfo
  xAODCutFlow
  PathResolver
  xAODTau
  xAODJet
  xAODBTagging
  xAODMuon
  xAODEgamma
  xAODMissingET
  xAODTracking
  xAODTruth
  xAODMetaDataCnv
  xAODMetaData
  PATInterfaces
  GoodRunsListsLib
  PileupReweightingLib
  TrigConfxAODLib
  TrigDecisionToolLib
  TriggerMatchingToolLib
  TrigGlobalEfficiencyCorrectionLib
  # TriggerAnalysisInterfacesLib
  ElectronPhotonFourMomentumCorrectionLib
  ElectronEfficiencyCorrectionLib
  ElectronPhotonSelectorToolsLib
  IsolationSelectionLib
  IsolationCorrectionsLib
  EGammaVariableCorrectionLib
  MuonMomentumCorrectionsLib
  MuonEfficiencyCorrectionsLib
  MuonSelectorToolsLib
  MuonAnalysisInterfacesLib
  PhotonEfficiencyCorrectionLib
  JetCalibToolsLib
  JetSelectorToolsLib
  JetJvtEfficiencyLib
  JetSubStructureUtils
  JetTileCorrectionLib
  JetUncertaintiesLib
  JetEDM
  JetCPInterfaces
  JetAnalysisInterfacesLib
  JetMomentToolsLib
  METUtilitiesLib
  METInterface
  PATInterfaces
  BoostedJetTaggersLib
  FTagAnalysisInterfacesLib
  TauAnalysisToolsLib
  AssociationUtilsLib
  PMGToolsLib
  SystematicsHandlesLib
  )

atlas_add_executable(example-daod examples/example-daod.cxx LINK_LIBRARIES
                     AnaQueryLib queryosity::queryosity)
