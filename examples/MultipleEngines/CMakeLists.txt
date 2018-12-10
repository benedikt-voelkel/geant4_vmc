#------------------------------------------------
# The Virtual Monte Carlo examples
# Copyright (C) 2014 Ivana Hrivnacova
# All rights reserved.
#
# For the licensing terms see geant4_vmc/LICENSE.
# Contact: root-vmc@cern.ch
#-------------------------------------------------

# CMake Configuration file for the VMC MultipleEngines example
# B. Volkel, 10/12/2018

#----------------------------------------------------------------------------
# Setup the project
cmake_minimum_required(VERSION 2.6.4 FATAL_ERROR)

#----------------------------------------------------------------------------
# Project ME
#
project(ME)

#----------------------------------------------------------------------------
# Define unique names of libraries and executables based on project name
#
set(library_name vmc_${PROJECT_NAME})
set(g4library_name geant4_${PROJECT_NAME})
set(program_name example${PROJECT_NAME})


#----------------------------------------------------------------------------
# CMake Module Path
#
set(CMAKE_MODULE_PATH
    ${Geant4VMC_DIR}/Modules
    ${Geant3_DIR}/Modules
    ${CMAKE_MODULE_PATH})

#----------------------------------------------------------------------------
# VMC Configuration file
# (for building MC independent code)
#
include(UseVMC)

#----------------------------------------------------------------------------
# Setup project include directories; compile definitions; link libraries
#
include_directories(
  ${VMC_INCLUDE_DIR}
  ${PROJECT_SOURCE_DIR}/include
  ${CMAKE_CURRENT_BINARY_DIR})

#----------------------------------------------------------------------------
# Generate Root dictionaries
#
ROOT_GENERATE_DICTIONARY(
  ${CMAKE_SHARED_LIBRARY_PREFIX}${library_name}
  no_rootmap
  ${CMAKE_CURRENT_SOURCE_DIR}/include/MEMCSingleApplication.h
  ${CMAKE_CURRENT_SOURCE_DIR}/include/MEMCMultiApplication.h
  ${CMAKE_CURRENT_SOURCE_DIR}/include/MEMCSingleStack.h
  ${CMAKE_CURRENT_SOURCE_DIR}/include/MEMCMultiStack.h
  ${CMAKE_CURRENT_SOURCE_DIR}/include/MEParticle.h
  LINKDEF ${CMAKE_CURRENT_SOURCE_DIR}/include/${program_name}LinkDef.h)

#----------------------------------------------------------------------------
# Locate sources and headers for this project
# NB: headers are included so they will show up in IDEs
#
set(headers
    ${PROJECT_SOURCE_DIR}/include/MEMCMultiApplication.h
    ${PROJECT_SOURCE_DIR}/include/MEMCMultiStack.h
    ${PROJECT_SOURCE_DIR}/include/MEMCSingleApplication.h
    ${PROJECT_SOURCE_DIR}/include/MEMCSingleStack.h
    ${PROJECT_SOURCE_DIR}/include/MEParticle.h)

    set(headers
        ${PROJECT_SOURCE_DIR}/src/MEMCMultiApplication.cxx
        ${PROJECT_SOURCE_DIR}/src/MEMCMultiStack.cxx
        ${PROJECT_SOURCE_DIR}/src/MEMCSingleApplication.cxx
        ${PROJECT_SOURCE_DIR}/src/MEMCSingleStack.cxx
        ${PROJECT_SOURCE_DIR}/src/MEParticle.cxx)

#----------------------------------------------------------------------------
# Add the example library
#
add_library(${library_name} ${sources} ${CMAKE_SHARED_LIBRARY_PREFIX}${library_name}_dict.cxx ${headers})
target_link_libraries(${library_name} ${VMC_LIBRARIES})

#----------------------------------------------------------------------------
# Install the library and dictionary map (if Root 6.x)
# to CMAKE_INSTALL_LIBDIR directory
#

if (VMC_INSTALL_EXAMPLES)
  install(TARGETS ${library_name} DESTINATION ${CMAKE_INSTALL_LIBDIR})
  # dictionary map (only if ROOT 6.x)
  if (${ROOT_FOUND_VERSION} GREATER 59999)
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_SHARED_LIBRARY_PREFIX}${library_name}_dict_rdict.pcm
      DESTINATION ${CMAKE_INSTALL_LIBDIR})
  endif()
endif()

#----------------------------------------------------------------------------
# MC Configuration file
# (for building MC dependent code)
#
#----------------------------------------------------------------------------
# MultipleEngines example requires G3 and G4
# Check again for availability of both Geant3 and 4
find_package(Geant4VMC QUIET)
find_package(Geant3 QUIET)
# If both is found including the flag 'VMC_MULTI' (taken from command line)
if(Geant4VMC_FOUND AND Geant3_FOUND AND VMC_MULTI)
  # Set internal flag, only done locally from this CMakeLists.txt
  set(REQUIRED_BY_MULTI ON)
  message(STATUS "Both Geant3 and Geant4 were found, build MultipleEngines example")
# Warn, in case both was found but flag 'VMC_MULTI' was not set
elseif(Geant4VMC_FOUND AND Geant3_FOUND)
  message(WARNING "Found Geant3 and Geant4. Use -DVMC_MULTI=ON to enable MultipleEngines example")
  # Nothing else to be done, return.
  return()
# Couldn't find both Geant3 and 4. Notify and return.
else()
  message(STATUS "Could not find both Geant3 and Geant4.")
  message(STATUS "MultipleEngines example disabled")
  return()
endif()
include(UseMC)

#----------------------------------------------------------------------------
# Do not build executables if no MC is selected
#
if (NOT MC_FOUND)
  return()
endif(NOT MC_FOUND)

#----------------------------------------------------------------------------
# Add the executables, and link them to all libraries
#
add_executable(${MC_PREFIX}vmc_${program_name} ${program_name}.cxx)
target_link_libraries(${MC_PREFIX}vmc_${program_name} ${library_name} ${MC_LIBRARIES})

#----------------------------------------------------------------------------
# Add program to the project targets
# (this avoids the need of typing the program name after make)
#
add_custom_target(${PROJECT_NAME} DEPENDS
                  ${MC_PREFIX}vmc_${program_name})

#----------------------------------------------------------------------------
# Install the executables to 'bin'
#
if (VMC_INSTALL_EXAMPLES)
  install(TARGETS ${MC_PREFIX}vmc_${program_name}
          DESTINATION bin)
endif()
