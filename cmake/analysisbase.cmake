project( HepQuery )

find_package( ROOT REQUIRED COMPONENTS Core Imt RIO Net Hist Graf Graf3d Gpad ROOTVecOps Tree TreePlayer Postscript Matrix Physics MathCore Thread MultiProc ROOTDataFrame )
find_package( AnalysisBase QUIET )

atlas_subdir( HepQuery )

atlas_add_root_dictionary ( HepQueryLib HepQueryDict
                           ROOT_HEADERS HepQuery/*.h Root/LinkDef.h
                           EXTERNAL_PACKAGES ROOT
)

atlas_add_library(HepQueryLib HepQuery/*.h Root/*.h Root/*.cxx ${HepQueryDict}
PUBLIC_HEADERS HepQuery
INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
LINK_LIBRARIES queryosity::queryosity ${ROOT_LIBRARIES} 
AthContainers AsgTools AnaAlgorithmLib EventLoop 
xAODBase xAODRootAccess
xAODEventInfo xAODCutFlow   
PathResolver xAODTau xAODJet xAODBTagging xAODMuon xAODEgamma xAODMissingET 
xAODTracking xAODTruth  xAODMetaDataCnv xAODMetaData
PATInterfaces
GoodRunsListsLib
TrigConfxAODLib TrigDecisionToolLib TriggerMatchingToolLib 
PileupReweightingLib
MuonMomentumCorrectionsLib MuonEfficiencyCorrectionsLib MuonSelectorToolsLib MuonAnalysisInterfacesLib
JetCalibToolsLib JetSelectorToolsLib 
JetJvtEfficiencyLib JetSubStructureUtils JetTileCorrectionLib 
JetEDM JetUncertaintiesLib
JetCPInterfaces
JetAnalysisInterfacesLib 
JetMomentToolsLib
METUtilitiesLib METInterface
PATInterfaces
BoostedJetTaggersLib
FTagAnalysisInterfacesLib
ElectronPhotonFourMomentumCorrectionLib ElectronEfficiencyCorrectionLib ElectronPhotonSelectorToolsLib
IsolationSelectionLib IsolationCorrectionsLib EGammaVariableCorrectionLib
PhotonEfficiencyCorrectionLib 
TauAnalysisToolsLib  
AssociationUtilsLib 
PMGToolsLib SystematicsHandlesLib
)

atlas_add_executable( example-daod examples/example-daod.cxx
  LINK_LIBRARIES HepQueryLib queryosity::queryosity
  ) 
