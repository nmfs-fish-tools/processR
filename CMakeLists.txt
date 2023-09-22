# find the R binary
cmake_minimum_required (VERSION 3.0)

project(processR)

MESSAGE("generating make files for ${CMAKE_PROJECT_NAME}")

set (CMAKE_CXX_STANDARD 17)

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

MESSAGE(STATUS "R_HOME is ${R_HOME}")
SET(R_INCLUDE ${R_HOME}/include)
include_directories("${R_INCLUDE}")

SET(R_LIB ${R_HOME}/lib)
  find_library(_R_LIBRARY R
            HINTS ${R_HOME}/lib
            DOC "R library (example libR.a, libR.dylib, etc.).")

#set RInside home
SET(RINSIDE_HOME ${R_USER}/RInside)
MESSAGE(STATUS "RINSIDE_HOME is ${RINSIDE_HOME}")

#set RInside home
SET(RINSIDE_INCLUDE ${RINSIDE_HOME}/include)


find_library(_LIB_RINSIDE
	NAMES RInside.so
	PATHS ${RINSIDE_HOME}/libs
	NO_CACHE
	REQUIRED)


#set Rcpp home
SET(RCPP_HOME ${R_HOME}/library/Rcpp)

SET(RCPP_INCLUDE ${RCPP_HOME}/include)
find_library(_LIB_RCPP
	NAMES Rcpp.so
	PATHS ${RCPP_HOME}/libs
	NO_CACHE
	REQUIRED)


MESSAGE(STATUS "RCPP_INCLUDE is ${RCPP_INCLUDE}")
MESSAGE(STATUS "R_LIB is ${R_LIB}")
MESSAGE(STATUS "RINSIDE_INCLUDE is ${RINSIDE_INCLUDE}")
MESSAGE(STATUS "_LIB_RINSIDE is ${_LIB_RINSIDE}")
MESSAGE(STATUS "_LIB_RCPP is ${_LIB_RCPP}")


link_directories(${RINSIDE_HOME}/libs)
link_directories(${RCPP_HOME}/libs)
link_directories(${R_HOME}/lib)

#find boost
find_package( Boost REQUIRED COMPONENTS program_options regex )
include_directories( ${Boost_INCLUDE_DIRS} )
include_directories( ${RCPP_INCLUDE} )
include_directories( ${RINSIDE_INCLUDE} )
include_directories( ${R_INCLUDE} )
 




include(GenerateExportHeader)
include_directories("${CMAKE_CURRENT_BINARY_DIR}") 

add_library(processR SHARED src/processR.cpp)
generate_export_header(processR)

add_executable(rchild src/rchild.cpp )





target_link_libraries(processR ${_R_LIBRARY})
target_link_libraries(processR ${_LIB_RCPP})

target_link_libraries(rchild ${_R_LIBRARY})
target_link_libraries(rchild ${_LIB_RCPP})
target_link_libraries(rchild ${_LIB_RINSIDE})


install(TARGETS processR DESTINATION lib)
install(TARGETS rchild DESTINATION inst/bin)



