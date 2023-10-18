# find the R binary
cmake_minimum_required(VERSION 3.14)


project(processR
  CXX
) # CXX is the language name

MESSAGE("generating make files for ${CMAKE_PROJECT_NAME}")

set (CMAKE_CXX_STANDARD 17)

#found in config
#R
MESSAGE(STATUS "R_HOME is ${R_HOME}")
MESSAGE(STATUS "R_BIN is ${R_BIN}")
MESSAGE(STATUS "R_INCLUDE is ${R_INCLUDE}")
include_directories( ${R_INCLUDE})
MESSAGE(STATUS "R_LIBRARY is ${R_LIBRARY}")
MESSAGE(STATUS "R_USER is ${R_USER}")
#BH
MESSAGE(STATUS "R_BH is ${R_BH}")
MESSAGE(STATUS "R_BH_INCLUDE is ${R_BH_INCLUDE}")
include_directories(${R_BH_INCLUDE})#Rcpp
MESSAGE(STATUS "R_RCPP is ${R_RCPP}")
MESSAGE(STATUS "R_RCPP_INCLUDE is ${R_RCPP_INCLUDE}")
include_directories( ${R_RCPP_INCLUDE})
MESSAGE(STATUS "R_RCPP_LIB is ${R_RCPP_LIB}")




link_directories(${R_RCPP_LIB}/libRcpp.so)
link_directories(${R_LIBRARY}/libR.so)



include(GenerateExportHeader)
include_directories("${CMAKE_CURRENT_BINARY_DIR}") 

add_library(processR SHARED src/ProcessR.cpp)
generate_export_header(processR)



target_link_libraries(processR ${R_LIB})
target_link_libraries(processR ${_LIB_RCPP})



set(CMAKE_SHARED_LINKER_FLAGS "-L${R_LIBRARY} -L${_LIB_RCPP}")

install(TARGETS processR DESTINATION libs)


EXECUTE_PROCESS(COMMAND ${CMAKE_COMMAND} -E copy ${_LIB_RINSIDE} ${CMAKE_INSTALL_PREFIX}/libs)
EXECUTE_PROCESS(COMMAND ${CMAKE_COMMAND} -E copy ${_LIB_RCPP} ${CMAKE_INSTALL_PREFIX}/libs)


EXECUTE_PROCESS(COMMAND ${CMAKE_COMMAND} -E copy ${_LIB_RINSIDE} ${CMAKE_INSTALL_PREFIX}/bin)
EXECUTE_PROCESS(COMMAND ${CMAKE_COMMAND} -E copy ${_LIB_RCPP} ${CMAKE_INSTALL_PREFIX}/bin)

MESSAGE(STATUS "CMAKE_INSTALL_PREFIX is ${CMAKE_INSTALL_PREFIX}")
