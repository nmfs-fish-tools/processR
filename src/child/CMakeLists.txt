# find the R binary
cmake_minimum_required(VERSION 3.14)

project(rchiild
  CXX
) # CXX is the language name

MESSAGE("generating make files for ${CMAKE_PROJECT_NAME}")

set (CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "-O3 -w -Wextra ")

#found in config
#R
MESSAGE(STATUS "R_HOME is ${R_HOME}")
MESSAGE(STATUS "R_BIN is ${R_BIN}")
MESSAGE(STATUS "R_INCLUDE is ${R_INCLUDE}")

include_directories( ${R_INCLUDE})

MESSAGE(STATUS "R_LIBRARY is ${R_LIBRARY}")
MESSAGE(STATUS "R_USER is ${R_USER}")
#Rcpp
MESSAGE(STATUS "R_RCPP is ${R_RCPP}")
MESSAGE(STATUS "R_RCPP_INCLUDE is ${R_RCPP_INCLUDE}")

include_directories( ${R_RCPP_INCLUDE})

MESSAGE(STATUS "R_RCPP_LIB is ${R_RCPP_LIB}")
#BH
MESSAGE(STATUS "R_BH is ${R_BH}")
MESSAGE(STATUS "R_BH_INCLUDE is ${R_BH_INCLUDE}")

include_directories( ${R_BH_INCLUDE})

#RInside
MESSAGE(STATUS "R_RINSIDE is ${R_RINSIDE}")
MESSAGE(STATUS "R_RINSIDE_INCLUDE is ${R_RINSIDE_INCLUDE}")
include_directories( ${R_RINSIDE_INCLUDE})
MESSAGE(STATUS "R_RINSIDE_LIB is ${R_RINSIDE_LIB}")





# MESSAGE(STATUS "RHOME is ${RHOME}")
# MESSAGE(STATUS "RCPP_HOME is ${RCPP_HOME}")
# MESSAGE(STATUS "BOOST_HOME is ${BOOST_HOME}")
# MESSAGE(STATUS "RInside_HOME is ${RInside_HOME}")
# SET(_LIB_RINSIDE ${RInside_HOME}/lib/libRInside.a)
# SET(RINSIDE_INCLUDE ${RInside_HOME}/include)
# SET(R_INCLUDE ${RHOME}/include)
# include_directories( ${R_INCLUDE})
#  SET(R_LIB ${RHOME}/lib)
#  SET(RCPP_INCLUDE ${RCPP_HOME}/include)
#  include_directories("${RCPP_INCLUDE}")
#  SET(_LIB_RCPP ${RCPP_HOME}/libs/)
#   SET(BOOST_INCLUDE ${BOOST_HOME}/include)
#  include_directories("${BOOST_INCLUDE}")

SET(BIN_DIR ${CMAKE_INSTALL_PREFIX}/bin)
file(MAKE_DIRECTORY ${BIN_DIR})


# MESSAGE(STATUS "RCPP_INCLUDE is ${RCPP_INCLUDE}")
# MESSAGE(STATUS "R_LIB is ${R_LIB}")
# MESSAGE(STATUS "RINSIDE_INCLUDE is ${RINSIDE_INCLUDE}")
# MESSAGE(STATUS "_LIB_RINSIDE is ${_LIB_RINSIDE}")
# MESSAGE(STATUS "_LIB_RCPP is ${_LIB_RCPP}")
# MESSAGE(STATUS "BOOST_INCLUDE is ${BOOST_INCLUDE}")

# link_directories(${R_RINSIDE_LIB})
# link_directories(${R_RCPP_LIB})
# link_directories(${R_LIBRARY})

# SET(R_LIB ${R_HOME}/lib)
#   find_library(_R_LIBRARY R
#             HINTS ${R_HOME}/lib
#             DOC "R library (example libR.a, libR.dylib, etc.).")

#find boost
#find_package( Boost REQUIRED COMPONENTS program_options regex )
# include_directories( ${Boost_INCLUDE_DIRS} )
# include_directories( ${RCPP_INCLUDE} )
# include_directories( ${RINSIDE_INCLUDE} )
# include_directories( ${R_INCLUDE} )



# link_directories(${RCPP_HOME}/libs)
# link_directories(${R_HOME}/lib)
# link_directories(${R_LIBRARY})


include(GenerateExportHeader)
include_directories("${CMAKE_CURRENT_BINARY_DIR}") 

add_executable(rchild RChild.cpp )


# link_directories(${R_RCPP_LIB})
# link_directories(${R_LIBRARY})

if(APPLE)
target_link_libraries(rchild ${R_LIBRARY}/libR.dylib)
else()
target_link_libraries(rchild ${R_LIBRARY}/libR.so)
endif()

# target_link_libraries(rchild ${R_RCPP_LIB}/Rcpp.so)



# target_link_libraries(rchild ${R_LIBRARY}/libR.so)
# set_target_properties(rchild PROPERTIES IMPORTED_LOCATION ${R_LIBRARY}/libR.so)
#target_link_libraries(rchild ${R_RCPP_LIB}/Rcpp.so)
target_link_libraries(rchild ${R_RINSIDE_LIB}/libRInside.a)
set_target_properties(rchild PROPERTIES IMPORTED_LOCATION ${R_RCPP_LIB}/Rcpp.so)
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
# set(RINSIDE_LIB ${RInside_HOME}/lib/libRInside.a)
set(CMAKE_SHARED_LINKER_FLAGS "-l${R_LIBRARY} -lR }")
# set(CMAKE_EXE_LINKER_FLAGS "${R_LIBRARY}/libR.so ${R_RCPP_LIB}/Rcpp.so ${R_RINSIDE_LIB}/libRInside.a")


install(TARGETS rchild DESTINATION bin)


#EXECUTE_PROCESS(COMMAND ${CMAKE_COMMAND} -E copy ${_LIB_RINSIDE} ${CMAKE_INSTALL_PREFIX}/bin)
#EXECUTE_PROCESS(COMMAND ${CMAKE_COMMAND} -E copy ${_LIB_RCPP} ${CMAKE_INSTALL_PREFIX}/bin)

#MESSAGE(STATUS "CMAKE_INSTALL_PREFIX is ${CMAKE_INSTALL_PREFIX}")
