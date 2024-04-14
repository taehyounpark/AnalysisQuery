
project(AnaQuery)

set(HEPQ_HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/AnaQuery/Tree.h ${CMAKE_CURRENT_SOURCE_DIR}/AnaQuery/Hist.h ${CMAKE_CURRENT_SOURCE_DIR}/AnaQuery/HistHelpers.h ${CMAKE_CURRENT_SOURCE_DIR}/AnaQuery/RDS.h)

set(HEPQ_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/Root/Tree.cxx ${CMAKE_CURRENT_SOURCE_DIR}/Root/RDS.cxx)

add_library(AnaQuery SHARED ${HEPQ_SOURCES} ${HEPQ_HEADERS})

target_include_directories(AnaQuery PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_compile_features(AnaQuery PUBLIC cxx_std_17)

target_compile_options(
  AnaQuery PRIVATE 
  -Wall -Wextra 
  $<$<CONFIG:Release>:-O3> 
  $<$<CONFIG:RelWithDebInfo>:-O3> 
  $<$<CONFIG:Debug>:-O0> 
  $<$<CONFIG:Debug>:-ggdb3> 
)

find_package(ROOT REQUIRED COMPONENTS Core Imt RIO Net Hist Graf Graf3d Gpad ROOTVecOps Tree TreePlayer Postscript Matrix Physics MathCore Thread MultiProc ROOTDataFrame )
find_library(ROOT_TREEPLAYER_LIBRARY TreePlayer HINTS ${ROOT_LIBRARY_DIR} REQUIRED)
find_library(ROOT_RDATAFRAME_LIBRARY ROOTDataFrame HINTS ${ROOT_LIBRARY_DIR} REQUIRED)
target_link_libraries(AnaQuery queryosity::queryosity ROOT::Core ROOT::RIO ROOT::Hist ROOT::Tree ROOT::TreePlayer ROOT::Imt ROOT::ROOTVecOps ROOT::ROOTDataFrame ROOT::Physics)

ROOT_GENERATE_DICTIONARY(
  AnaQueryDict ${HEPQ_HEADERS}
  LINKDEF Root/LinkDef.h
  MODULE AnaQuery
)

export(PACKAGE AnaQuery)
set(CONF_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}" "${PROJECT_SOURCE_DIR}/AnaQuery" )
set(CONF_LIBRARY_DIRS "${PROJECT_BINARY_DIR}")
set(CONF_LIBRARIES AnaQuery)
configure_file(AnaQuery-config.cmake.in
  "${PROJECT_BINARY_DIR}/config.cmake" @ONLY)

set(HEPQ_PYTHONPATH ${CMAKE_CURRENT_BINARY_DIR})
set(HEPQ_LD_LIBRARY_PATH ${CMAKE_CURRENT_BINARY_DIR})

install(DIRECTORY
  "${CMAKE_CURRENT_BINARY_DIR}/AnaQuery"
  EXPORT AnaQueryConfig
  DESTINATION lib
  )

add_executable(example-tree examples/example-tree.cxx)
target_compile_features(example-tree PUBLIC cxx_std_17)
target_include_directories(example-tree PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${ROOT_INCLUDE_DIRS} ${ROOT_LIBRARY_DIR})
target_link_libraries(example-tree AnaQuery ${ROOT_LIBRARIES})

set(SETUP ${CMAKE_CURRENT_BINARY_DIR}/setup.sh)
file(WRITE ${SETUP} "#!/bin/bash\n")
file(APPEND ${SETUP} "ulimit -S -s unlimited\n" )
file(APPEND ${SETUP} "export LD_LIBRARY_PATH=\${LD_LIBRARY_PATH}:${HEPQ_LD_LIBRARY_PATH}\n")
file(APPEND ${SETUP} "export DYLD_LIBRARY_PATH=\${LD_LIBRARY_PATH}:${HEPQ_LD_LIBRARY_PATH}\n")

