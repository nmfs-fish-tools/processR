# find the R binary
cmake_minimum_required(VERSION 3.14)

project(rchiild
  CXX
) # CXX is the language name

MESSAGE("generating make files for ${CMAKE_PROJECT_NAME}")

set (CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "-O3 -Wall -Wextra")

include(FetchContent)

MESSAGE(STATUS "RHOME is ${RHOME}")
MESSAGE(STATUS "RCPP_HOME is ${RCPP_HOME}")
MESSAGE(STATUS "BOOST_HOME is ${BOOST_HOME}")
MESSAGE(STATUS "RInside_HOME is ${RInside_HOME}")
SET(_LIB_RINSIDE ${RInside_HOME}/lib/libRInside.a)
SET(RINSIDE_INCLUDE ${RInside_HOME}/include)
 SET(R_INCLUDE ${RHOME}/include)
  include_directories( ${R_INCLUDE})
 SET(R_LIB ${RHOME}/lib)
 SET(RCPP_INCLUDE ${RCPP_HOME}/include)
 include_directories("${RCPP_INCLUDE}")
 SET(_LIB_RCPP ${RCPP_HOME}/libs/Rcpp.so)
  SET(BOOST_INCLUDE ${BOOST_HOME}/include)
 include_directories("${BOOST_INCLUDE}")

SET(BIN_DIR ${CMAKE_INSTALL_PREFIX}/bin)
file(MAKE_DIRECTORY ${BIN_DIR})

MESSAGE(STATUS "RCPP_INCLUDE is ${RCPP_INCLUDE}")
MESSAGE(STATUS "R_LIB is ${R_LIB}")
MESSAGE(STATUS "RINSIDE_INCLUDE is ${RINSIDE_INCLUDE}")
MESSAGE(STATUS "_LIB_RINSIDE is ${_LIB_RINSIDE}")
MESSAGE(STATUS "_LIB_RCPP is ${_LIB_RCPP}")
MESSAGE(STATUS "BOOST_INCLUDE is ${BOOST_INCLUDE}")

link_directories(${RINSIDE_HOME}/libs)
#link_directories(${RCPP_HOME}/libs)
link_directories(${R_HOME}/lib)

SET(R_LIB ${R_HOME}/lib)
  find_library(_R_LIBRARY R
            HINTS ${R_HOME}/lib
            DOC "R library (example libR.a, libR.dylib, etc.).")

#find boost
#find_package( Boost REQUIRED COMPONENTS program_options regex )
include_directories( ${Boost_INCLUDE_DIRS} )
include_directories( ${RCPP_INCLUDE} )
include_directories( ${RINSIDE_INCLUDE} )
include_directories( ${R_INCLUDE} )






include(GenerateExportHeader)
include_directories("${CMAKE_CURRENT_BINARY_DIR}") 


add_executable(rchild rchild.cpp )




target_link_libraries(rchild ${_R_LIBRARY})
#target_link_libraries(rchild ${_LIB_RCPP})
target_link_libraries(rchild ${RInside_HOME}/lib/libRInside.a)

set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
set(RINSIDE_LIB ${RInside_HOME}/lib/libRInside.a)
#set(CMAKE_SHARED_LINKER_FLAGS "-l${_R_LIBRARY} -L${${RCPP_HOME}/libs}")
set(CMAKE_EXE_LINKER_FLAGS " -L${RInside_HOME}/lib")


install(TARGETS rchild DESTINATION bin)


#EXECUTE_PROCESS(COMMAND ${CMAKE_COMMAND} -E copy ${_LIB_RINSIDE} ${CMAKE_INSTALL_PREFIX}/bin)
#EXECUTE_PROCESS(COMMAND ${CMAKE_COMMAND} -E copy ${_LIB_RCPP} ${CMAKE_INSTALL_PREFIX}/bin)

#MESSAGE(STATUS "CMAKE_INSTALL_PREFIX is ${CMAKE_INSTALL_PREFIX}")
