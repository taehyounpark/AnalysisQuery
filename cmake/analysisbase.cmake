project( HepQuery )

find_package( ROOT REQUIRED COMPONENTS Core Imt RIO Net Hist Graf Graf3d Gpad ROOTVecOps Tree TreePlayer Postscript Matrix Physics MathCore Thread MultiProc ROOTDataFrame )
find_package( AnalysisBase QUIET )

atlas_subdir( HepQuery )

atlas_add_root_dictionary ( HepQueryLib HepQueryDictSource
                           ROOT_HEADERS HepQuery/*.h Root/LinkDef.h
                           EXTERNAL_PACKAGES ROOT
)

# atlas_add_dictionary (HepQueryDict
#   HepQuery/HepQueryDict.h
#   HepQuery/selection.xml
#   LINK_LIBRARIES HepQueryLib queryosity::queryosity )

atlas_add_library( HepQueryLib 
  HepQuery/*.h Root/*.cxx ${HepQueryDictSource}
  PUBLIC_HEADERS HepQuery
  INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
  LINK_LIBRARIES ${ROOT_LIBRARIES} EventLoop xAODBase xAODRootAccess xAODCutFlow xAODEventInfo xAODTau xAODJet xAODMuon xAODEgamma
  xAODTracking xAODTruth queryosity::queryosity )

atlas_add_executable( example-daod examples/example-daod.cxx
  LINK_LIBRARIES HepQueryLib ) 
