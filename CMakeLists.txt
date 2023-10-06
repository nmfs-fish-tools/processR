# find the R binary
cmake_minimum_required(VERSION 3.14)


project(processR
  CXX
) # CXX is the language name

MESSAGE("generating make files for ${CMAKE_PROJECT_NAME}")

set (CMAKE_CXX_STANDARD 17)



# detection for OSX
if(APPLE)

  find_library(LIBR_LIBRARIES R)

  if(LIBR_LIBRARIES MATCHES ".*\\.framework")
    set(LIBR_HOME "${LIBR_LIBRARIES}/Resources" CACHE PATH "R home directory")
    set(LIBR_INCLUDE_DIRS "${LIBR_HOME}/include" CACHE PATH "R include directory")
    set(LIBR_EXECUTABLE "${LIBR_HOME}/R" CACHE PATH "R executable")
    set(LIBR_LIB_DIR "${LIBR_HOME}/lib" CACHE PATH "R lib directory")
  else()
    get_filename_component(_LIBR_LIBRARIES "${LIBR_LIBRARIES}" REALPATH)
    get_filename_component(_LIBR_LIBRARIES_DIR "${_LIBR_LIBRARIES}" DIRECTORY)
    set(LIBR_EXECUTABLE "${_LIBR_LIBRARIES_DIR}/../bin/R")
    execute_process(
      COMMAND ${LIBR_EXECUTABLE} "--slave" "--vanilla" "-e" "cat(R.home())"
      OUTPUT_VARIABLE LIBR_HOME)
    set(LIBR_HOME ${LIBR_HOME} CACHE PATH "R home directory")
    set(LIBR_INCLUDE_DIRS "${LIBR_HOME}/include" CACHE PATH "R include directory")
    set(LIBR_LIB_DIR "${LIBR_HOME}/lib" CACHE PATH "R lib directory")
  endif()
  
# detection for UNIX & Win32
else()

  # attempt to find R executable
  if(NOT LIBR_EXECUTABLE)
    find_program(LIBR_EXECUTABLE NAMES R R.exe)
  endif()
  
  if(UNIX)

    if(NOT LIBR_EXECUTABLE)
      message(FATAL_ERROR "Unable to locate R executable.\
        \nEither add its location to PATH or provide it through the LIBR_EXECUTABLE cmake variable")
    endif()

    # ask R for the home path
    execute_process(
      COMMAND ${LIBR_EXECUTABLE} "--slave" "--vanilla" "-e" "cat(R.home())"
      OUTPUT_VARIABLE LIBR_HOME
    )
    # ask R for the include dir
    execute_process(
      COMMAND ${LIBR_EXECUTABLE} "--slave" "--no-save" "-e" "cat(R.home('include'))"
      OUTPUT_VARIABLE LIBR_INCLUDE_DIRS
    )
    # ask R for the lib dir
    execute_process(
      COMMAND ${LIBR_EXECUTABLE} "--slave" "--no-save" "-e" "cat(R.home('lib'))"
      OUTPUT_VARIABLE LIBR_LIB_DIR
    )

  # Windows
  else()
    # ask R for R_HOME 
    if(LIBR_EXECUTABLE)
      execute_process(
        COMMAND ${LIBR_EXECUTABLE} "--slave" "--no-save" "-e" "cat(normalizePath(R.home(), winslash='/'))"
        OUTPUT_VARIABLE LIBR_HOME)
    endif()
    # if R executable not available, query R_HOME path from registry
    if(NOT LIBR_HOME)
      get_filename_component(LIBR_HOME
        "[HKEY_LOCAL_MACHINE\\SOFTWARE\\R-core\\R\\${R_VERSION};InstallPath]"
        ABSOLUTE)
      if(NOT LIBR_HOME)
        message(FATAL_ERROR "\nUnable to locate R executable.\
          \nEither add its location to PATH or provide it through the LIBR_EXECUTABLE cmake variable")
      endif()
    endif()
    # set exe location based on R_ARCH
    if(NOT LIBR_EXECUTABLE)
      set(LIBR_EXECUTABLE "${LIBR_HOME}/bin/${R_ARCH}/R.exe")
    endif()
    # set other R paths based on home path
    set(LIBR_INCLUDE_DIRS "${LIBR_HOME}/include")
    set(LIBR_LIB_DIR "${LIBR_HOME}/bin/${R_ARCH}")
 
message(STATUS "LIBR_HOME [${LIBR_HOME}]")
message(STATUS "LIBR_EXECUTABLE [${LIBR_EXECUTABLE}]")
message(STATUS "LIBR_INCLUDE_DIRS [${LIBR_INCLUDE_DIRS}]")
message(STATUS "LIBR_LIB_DIR [${LIBR_LIB_DIR}]")
message(STATUS "LIBR_CORE_LIBRARY [${LIBR_CORE_LIBRARY}]")

  endif()

endif()


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
