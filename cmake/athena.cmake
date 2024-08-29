project(EventFlow)

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
find_package(Athena QUIET)
find_package(nlohmann_json REQUIRED)

atlas_subdir(EventFlow)

atlas_add_root_dictionary(
  EventFlowLib
  EventFlowDict
  ROOT_HEADERS
  EventFlow/*.h
  Root/LinkDef.h
  EXTERNAL_PACKAGES
  ROOT
  )

atlas_add_library(
  EventFlowLib
  EventFlow/*.h
  Root/*.h
  Root/*.cxx
  ${EventFlowDict}
  PUBLIC_HEADERS 
  EventFlow
  LINK_LIBRARIES
  queryosity::queryosity
  nlohmann_json::nlohmann_json
  ${ROOT_LIBRARIES}
  ${VDT_LIBRARIES}
  AthContainers
  AsgTools
  # EventLoop
  PathResolver
  xAODBase
  xAODRootAccess
  xAODEventInfo
  xAODEventShape
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
  # xAODMetaDataCnv
  xAODMetaData
  PATInterfaces
  CaloEvent 
  AnaAlgorithmLib
  AthAnalysisBaseCompsLib
  CaloUtilsLib
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
  # JetTileCorrectionLib
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
  INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${VDT_INCLUDE_DIRS}
  LINK_LIBRARIES ${ROOT_LIBRARIES} ${VDT_LIBRARIES} RootUtils xAODRootAccess
  )

atlas_install_python_modules( python/*.py )
atlas_install_scripts( scripts/analyze )
