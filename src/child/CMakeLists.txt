# find the R binary
cmake_minimum_required (VERSION 3.0)

project(rchiild)

MESSAGE("generating make files for ${CMAKE_PROJECT_NAME}")

set (CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "-O3 -Wall -Wextra")

MESSAGE(STATUS "Looking for R executable")

		EXECUTE_PROCESS(
			COMMAND which R 
			OUTPUT_VARIABLE R_EXECUTABLE)

MESSAGE(STATUS "R_EXECUTABLE is ${R_EXECUTABLE}") 


EXECUTE_PROCESS(
		COMMAND R "--slave" "--no-save" "-e" "cat(R.home())"
		OUTPUT_VARIABLE R_HOME)
		
		EXECUTE_PROCESS(
		COMMAND R "--slave" "--no-save" "-e" "cat(Sys.getenv('R_LIBS_USER'))"
		OUTPUT_VARIABLE R_USER)

MESSAGE(STATUS "R_USER is ${R_USER}")


MESSAGE(STATUS "R_HOME is ${R_HOME}")
SET(R_INCLUDE ${R_HOME}/include)
include_directories("${R_INCLUDE}")


SET(R_LIB ${R_HOME}/lib)
  find_library(_R_LIBRARY R
            HINTS ${R_HOME}/lib
            DOC "R library (example libR.a, libR.dylib, etc.).")



#set RInside home
IF(EXISTS ${R_HOME}/library/RInside)

SET(RINSIDE_HOME ${R_HOME}/RInside)
MESSAGE(STATUS "RINSIDE_HOME is ${RINSIDE_HOME}")
SET(RINSIDE_INCLUDE ${RINSIDE_HOME}/include)

ELSEIF(EXISTS ${R_USER}/RInside)

SET(RINSIDE_HOME ${R_USER}/RInside)
MESSAGE(STATUS "RINSIDE_HOME is ${RINSIDE_HOME}")
SET(RINSIDE_INCLUDE ${RINSIDE_HOME}/include)

ELSE()
 MESSAGE(FATAL_ERROR "Package RInside not found")
ENDIF()





#set Rcpp home
IF(EXISTS ${R_HOME}/library/Rcpp)

 SET(RCPP_HOME ${R_HOME}/library/Rcpp)
 SET(RCPP_INCLUDE ${RCPP_HOME}/include)

ELSEIF(EXISTS ${R_USER}/library/Rcpp)

 SET(RCPP_HOME ${R_USER}/library/Rcpp)
 SET(RCPP_INCLUDE ${R_USER}/include)

ELSE()
 MESSAGE(FATAL_ERROR "Package Rcpp not found")
ENDIF()



SET(BIN_DIR ${CMAKE_INSTALL_PREFIX}/bin)
file(MAKE_DIRECTORY ${BIN_DIR})

MESSAGE(STATUS "RCPP_INCLUDE is ${RCPP_INCLUDE}")
MESSAGE(STATUS "R_LIB is ${R_LIB}")
MESSAGE(STATUS "RINSIDE_INCLUDE is ${RINSIDE_INCLUDE}")
MESSAGE(STATUS "_LIB_RINSIDE is ${_LIB_RINSIDE}")
MESSAGE(STATUS "_LIB_RCPP is ${_LIB_RCPP}")


link_directories(${RINSIDE_HOME}/libs)
#link_directories(${RCPP_HOME}/libs)
link_directories(${R_HOME}/lib)

#find boost
find_package( Boost REQUIRED COMPONENTS program_options regex )
include_directories( ${Boost_INCLUDE_DIRS} )
include_directories( ${RCPP_INCLUDE} )
include_directories( ${RINSIDE_INCLUDE} )
include_directories( ${R_INCLUDE} )
 




include(GenerateExportHeader)
include_directories("${CMAKE_CURRENT_BINARY_DIR}") 


add_executable(rchild rchild.cpp )




target_link_libraries(rchild ${_R_LIBRARY})
#target_link_libraries(rchild ${_LIB_RCPP})
target_link_libraries(rchild ${RINSIDE_HOME}/lib/libRInside.a)

set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

#set(CMAKE_SHARED_LINKER_FLAGS "-l${_R_LIBRARY} -L${${RCPP_HOME}/libs}")
set(CMAKE_EXE_LINKER_FLAGS " -L${RINSIDE_HOME}/lib")


install(TARGETS rchild DESTINATION bin)


#EXECUTE_PROCESS(COMMAND ${CMAKE_COMMAND} -E copy ${_LIB_RINSIDE} ${CMAKE_INSTALL_PREFIX}/bin)
#EXECUTE_PROCESS(COMMAND ${CMAKE_COMMAND} -E copy ${_LIB_RCPP} ${CMAKE_INSTALL_PREFIX}/bin)

#MESSAGE(STATUS "CMAKE_INSTALL_PREFIX is ${CMAKE_INSTALL_PREFIX}")
