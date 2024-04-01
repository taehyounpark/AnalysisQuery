
project(HepQuery)

file(GLOB HEPQ_HEADERS HepQuery/*.h)
list(FILTER HEPQ_HEADERS EXCLUDE REGEX ".*HepQuery/Event\.h$")
list(FILTER HEPQ_HEADERS EXCLUDE REGEX ".*HepQuery/HepQueryDict\.h$")

file(GLOB HEPQ_SOURCES Root/*.cxx)
list(FILTER HEPQ_SOURCES EXCLUDE REGEX ".*Root/Event\.cxx$")
list(FILTER HEPQ_HEADERS EXCLUDE REGEX ".*Root/LinkDef\.h$")

add_library(HepQuery SHARED ${HEPQ_SOURCES} ${HEPQ_HEADERS} )

target_include_directories(HepQuery PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_compile_features(HepQuery PUBLIC cxx_std_17)

target_compile_options(
  HepQuery PRIVATE 
  -Wall -Wextra 
  $<$<CONFIG:Release>:-O3> 
  $<$<CONFIG:RelWithDebInfo>:-O3> 
  $<$<CONFIG:Debug>:-O0> $<$<CONFIG:Debug>:-ggdb3> 
)

find_package(ROOT REQUIRED COMPONENTS Core Imt RIO Net Hist Graf Graf3d Gpad ROOTVecOps Tree TreePlayer Postscript Matrix Physics MathCore Thread MultiProc ROOTDataFrame )
find_library(ROOT_TREEPLAYER_LIBRARY TreePlayer HINTS ${ROOT_LIBRARY_DIR} REQUIRED)
find_library(ROOT_RDATAFRAME_LIBRARY ROOTDataFrame HINTS ${ROOT_LIBRARY_DIR} REQUIRED)
target_link_libraries(HepQuery queryosity::queryosity ROOT::Core ROOT::RIO ROOT::Hist ROOT::Tree ROOT::TreePlayer ROOT::Imt ROOT::ROOTVecOps ROOT::ROOTDataFrame ROOT::Physics)

REFLEX_GENERATE_DICTIONARY(
  HepQuery HepQuery/HepQueryDict.h
  SELECTION HepQuery/selection.xml
)

export(PACKAGE HepQuery)
set(CONF_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}" "${PROJECT_SOURCE_DIR}/HepQuery" )
set(CONF_LIBRARY_DIRS "${PROJECT_BINARY_DIR}")
set(CONF_LIBRARIES HepQuery)
configure_file(HepQuery-config.cmake.in
  "${PROJECT_BINARY_DIR}/config.cmake" @ONLY)

set(HEPQ_PYTHONPATH ${CMAKE_CURRENT_BINARY_DIR})
set(HEPQ_LD_LIBRARY_PATH ${CMAKE_CURRENT_BINARY_DIR})

install(DIRECTORY
  "${CMAKE_CURRENT_BINARY_DIR}/HepQuery"
  EXPORT HepQueryConfig
  DESTINATION lib
  )

add_executable(example-tree examples/example-tree.cxx)
target_compile_features(example-tree PUBLIC cxx_std_17)
target_include_directories(example-tree PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${ROOT_INCLUDE_DIRS} ${ROOT_LIBRARY_DIR})
target_link_libraries(example-tree HepQuery ${ROOT_LIBRARIES})

set(SETUP ${CMAKE_CURRENT_BINARY_DIR}/setup.sh)
file(WRITE ${SETUP} "#!/bin/bash\n")
file(APPEND ${SETUP} "ulimit -S -s unlimited\n" )
file(APPEND ${SETUP} "export LD_LIBRARY_PATH=\${LD_LIBRARY_PATH}:${HEPQ_LD_LIBRARY_PATH}\n")
file(APPEND ${SETUP} "export DYLD_LIBRARY_PATH=\${LD_LIBRARY_PATH}:${HEPQ_LD_LIBRARY_PATH}\n")
file(APPEND ${SETUP} "export DYLD_LIBRARY_PATH=\${LD_LIBRARY_PATH}:${HEPQ_LD_LIBRARY_PATH}\n")

