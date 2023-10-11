# find the R binary
cmake_minimum_required(VERSION 3.14)


project(processR
  CXX
) # CXX is the language name

MESSAGE("generating make files for ${CMAKE_PROJECT_NAME}")

set (CMAKE_CXX_STANDARD 17)

MESSAGE(STATUS "Looking for R executable")


		EXECUTE_PROCESS(
			COMMAND which R 
			OUTPUT_VARIABLE R_EXECUTABLE)

MESSAGE(STATUS "R_EXECUTABLE is ${R_EXECUTABLE}") 



MESSAGE(STATUS "Calling  ${R_EXECUTABLE} --slave --no-save -e cat(R.home()) to find home....") 

EXECUTE_PROCESS(
		COMMAND ${R_EXECUTABLE} "--slave" "--no-save" "-e" "cat(R.home())" 
		OUTPUT_VARIABLE R_HOME 
		OUTPUT_STRIP_TRAILING_WHITESPACE)

MESSAGE(STATUS "R_HOME is ${R_HOME}")

SET(R_INCLUDE ${R_HOME}/include)
include_directories("${R_INCLUDE}")
		
		
EXECUTE_PROCESS(
		COMMAND ${R_EXECUTABLE} "--slave" "--no-save" "-e" "cat(Sys.getenv('R_LIBS_USER'))" 
		OUTPUT_VARIABLE R_USER 
		OUTPUT_STRIP_TRAILING_WHITESPACE)

MESSAGE(STATUS "R_USER is ${R_USER}")


SET(R_LIB ${R_HOME}/lib)
  find_library(_R_LIBRARY R
            HINTS ${R_HOME}/lib
            DOC "R library (example libR.a, libR.dylib, etc.).")




#set Rcpp home
IF(EXISTS ${R_HOME}/library/Rcpp)

 SET(RCPP_HOME ${R_HOME}/library/Rcpp)
 SET(RCPP_INCLUDE ${RCPP_HOME}/include)
 include_directories("${RCPP_INCLUDE}")
 SET(_LIB_RCPP ${RCPP_HOME}/libs/Rcpp.so)
 
ELSEIF(EXISTS ${R_USER}/Rcpp)

 SET(RCPP_HOME ${R_USER}/Rcpp)
 SET(RCPP_INCLUDE ${RCPP_HOME}/include)
 include_directories("${RCPP_INCLUDE}")
 SET(_LIB_RCPP ${RCPP_HOME}/libs/Rcpp.so)
 
ELSE()
 MESSAGE(FATAL_ERROR  "Package Rcpp not found at ${R_HOME} or ${R_USER}")
ENDIF()


#set boost home
IF(EXISTS ${R_HOME}/library/BH)

 SET(BOOST_HOME ${R_HOME}/library/BH)
 SET(BOOST_INCLUDE ${BOOST_HOME}/include)
 include_directories("${BOOST_INCLUDE}")

ELSEIF(EXISTS ${R_USER}/BH)

 SET(BOOST_HOME ${R_USER}/BH)
 SET(BOOST_INCLUDE ${BOOST_HOME}/include)
 include_directories("${BOOST_INCLUDE}")
 
ELSE()
 MESSAGE(FATAL_ERROR "Package BH not found")
ENDIF()


SET(LIB_DIR ${CMAKE_INSTALL_PREFIX}/libs)
file(MAKE_DIRECTORY ${LIB_DIR})

MESSAGE(STATUS "RCPP_INCLUDE is ${RCPP_INCLUDE}")
MESSAGE(STATUS "R_LIB is ${R_LIB}")
#MESSAGE(STATUS "RINSIDE_INCLUDE is ${RINSIDE_INCLUDE}")
MESSAGE(STATUS "_LIB_RCPP is ${_LIB_RCPP}")
MESSAGE(STATUS "BOOST_INCLUDE is ${BOOST_INCLUDE}")

link_directories(${RCPP_HOME}/libs)
link_directories(${R_HOME}/lib)

#find boost
#find_package( Boost REQUIRED COMPONENTS program_options regex )
#include_directories( ${Boost_INCLUDE_DIRS} )
#include_directories( ${RCPP_INCLUDE} )
#include_directories( ${RINSIDE_INCLUDE} )
#include_directories( ${R_INCLUDE} )
 

include(GenerateExportHeader)
include_directories("${CMAKE_CURRENT_BINARY_DIR}") 

add_library(processR SHARED src/processR.cpp)
generate_export_header(processR)



target_link_libraries(processR ${_R_LIBRARY})
target_link_libraries(processR ${_LIB_RCPP})



set(CMAKE_SHARED_LINKER_FLAGS "-L${_R_LIBRARY} -L${_LIB_RCPP}")
#set(CMAKE_EXE_LINKER_FLAGS "-l${_R_LIBRARY}  -L${_LIB_RCPP} -L${_LIB_RINSIDE}")

install(TARGETS processR DESTINATION libs)


EXECUTE_PROCESS(COMMAND ${CMAKE_COMMAND} -E copy ${_LIB_RINSIDE} ${CMAKE_INSTALL_PREFIX}/libs)
EXECUTE_PROCESS(COMMAND ${CMAKE_COMMAND} -E copy ${_LIB_RCPP} ${CMAKE_INSTALL_PREFIX}/libs)


EXECUTE_PROCESS(COMMAND ${CMAKE_COMMAND} -E copy ${_LIB_RINSIDE} ${CMAKE_INSTALL_PREFIX}/bin)
EXECUTE_PROCESS(COMMAND ${CMAKE_COMMAND} -E copy ${_LIB_RCPP} ${CMAKE_INSTALL_PREFIX}/bin)

MESSAGE(STATUS "CMAKE_INSTALL_PREFIX is ${CMAKE_INSTALL_PREFIX}")
