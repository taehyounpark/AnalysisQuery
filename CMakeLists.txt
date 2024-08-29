################################################################################
# EventFlow
################################################################################

if($ENV{AnalysisBase_SET_UP})
  set(STANDALONE_BUILD 0)
  set(ANALYSISBASE_BUILD 1)  
  set(ATHENA_BUILD 0)  
elseif($ENV{Athena_SET_UP})
  set(STANDALONE_BUILD 0)
  set(ANALYSISBASE_BUILD 0)
  set(ATHENA_BUILD 1)
else()
  set(STANDALONE_BUILD 1)
  set(ANALYSISBASE_BUILD 0)
  set(ATHENA_BUILD 0)
endif()

if(ANALYSISBASE_BUILD)
  include(./cmake/analysisbase.cmake)
endif()

if(ATHENA_BUILD)
  include(./cmake/athena.cmake)
endif()

if(STANDALONE_BUILD)
  include(./cmake/standalone.cmake)
endif()