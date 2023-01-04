cmake_minimum_required(VERSION 3.12)

project(eyelogiclsl LANGUAGES CXX)

if ( NOT DEFINED CMAKE_GENERATOR_PLATFORM )
    message( FATAL_ERROR "Generator platform not set. Please set -A <Win32 or x64>." )
endif ( NOT DEFINED CMAKE_GENERATOR_PLATFORM )

set( PROJECT_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/.. )
set( INSTALL_ROOT_DIR ${PROJECT_ROOT_DIR}/install CACHE STRING "Installation directory" )

set( ELApi_DIR "${PROJECT_ROOT_DIR}/dependencies/eyelogic" CACHE STRING "Location of the EyeLogic API" )
if ( NOT EXISTS ${ELApi_DIR} )
    message( FATAL_ERROR "Cannot find EyeLogic SDK directory ${ELApi_DIR}!" )
endif ( NOT EXISTS ${ELApi_DIR} )

if ( NOT DEFINED LSL_DIR )
    message( FATAL_ERROR "Variable LSL_DIR not set. Please declare variable -DLSL_DIR=\"<LSL directory path>\"" )
else ()
    if ( NOT EXISTS ${LSL_DIR} )
    message( FATAL_ERROR "Cannot find liblsl directory ${LSL_DIR}!" )
    endif ( NOT EXISTS ${LSL_DIR} )
endif( NOT DEFINED LSL_DIR )

# must be set for find_package to find Find*.cmake files
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules/")
find_package( ELApi REQUIRED )
find_package( lsl REQUIRED )

file(GLOB_RECURSE SOURCE_FILES_${PROJECT_NAME} sources "*.cpp")
file(GLOB_RECURSE HEADER_FILES_${PROJECT_NAME} "*.h" "*.hpp")

set( PROJECT_SOURCES
    ${SOURCE_FILES_${PROJECT_NAME}}
    ${HEADER_FILES_${PROJECT_NAME}} )

set(INCLUDE_DIRS_${PROJECT_NAME}
    ${PROJECT_SOURCE_DIR}
    ${ELApi_INCLUDE_DIR}
    ${LSL_INCLUDE_DIR}
    CACHE STRING "${PROJECT_NAME}: Include Directories" FORCE)

set(LINK_LIBS_${PROJECT_NAME}
    ${ELApi_LIBRARIES}
    ${LSL_LIBRARIES}
    CACHE STRING "${PROJECT_NAME}: Link Libraries" FORCE)

include_directories(${INCLUDE_DIRS_${PROJECT_NAME}})
add_executable(${PROJECT_NAME} ${PROJECT_SOURCES} )
target_link_libraries(${PROJECT_NAME} PRIVATE ${LINK_LIBS_${PROJECT_NAME}})

install( DIRECTORY DESTINATION ${INSTALL_ROOT_DIR} )
install ( TARGETS ${PROJECT_NAME} DESTINATION ${INSTALL_ROOT_DIR} )
install( FILES ${LSL_BINARIES} ${ELApi_BINARIES} DESTINATION ${INSTALL_ROOT_DIR} )

