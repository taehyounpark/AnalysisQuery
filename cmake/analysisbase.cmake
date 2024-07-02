project(AnalysisQuery)

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

atlas_subdir(AnalysisQuery)

atlas_add_root_dictionary(
  AnalysisQueryLib
  AnalysisQueryDict
  ROOT_HEADERS
  AnalysisQuery/*.h
  Root/LinkDef.h
  EXTERNAL_PACKAGES
  ROOT
  )

atlas_add_library(
  AnalysisQueryLib
  AnalysisQuery/*.h
  Root/*.h
  Root/*.cxx
  ${AnalysisQueryDict}
  PUBLIC_HEADERS 
  AnalysisQuery
  LINK_LIBRARIES
  queryosity::queryosity
  nlohmann_json::nlohmann_json
  ${ROOT_LIBRARIES}
  AthContainers
  AsgTools
  AnaAlgorithmLib
  EventLoop
  PathResolver
  xAODBase
  xAODRootAccess
  xAODEventInfo
  xAODCutFlow
  xAODCaloEvent 
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
  PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
  PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES}
  )

atlas_add_executable(example-daod examples/example-daod.cxx LINK_LIBRARIES
                     AnalysisQueryLib queryosity::queryosity)

atlas_install_python_modules( python/*.py )
atlas_install_scripts( scripts/analyze )
