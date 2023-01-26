# - Try to find ELApi
# Once done this will define
#  ELApi_FOUND - System has ELApi
#  ELApi_INCLUDE_DIRS - The ELApi include directories
#  ELApi_LIBRARIES - The libraries needed to use ELApi
#  ELApi_BINARIES - The biaries needed to run the .exe

set( FIND_ELApi_FAILURE_MESSAGE "Could not find all necessary ELApi resources. Please check that ELApi_DIR=\"${ELApi_DIR}\" points to a directory containing the include & bin directories of your desired EyeLogic SDK version." )

if ( CMAKE_GENERATOR_PLATFORM STREQUAL "x64" )
    set( BIN_SUBDIR x64 )
elseif ( CMAKE_GENERATOR_PLATFORM STREQUAL "Win32" )
    set( BIN_SUBDIR x86 )
    set( ELApi_PLATFORM_SUFFIX 32 )
else()
    message( WARNING "Unexpected generator platfrom: ${CMAKE_GENERATOR_PLATFORM} - expect (Win32, x64)" )
    message( FATAL_ERROR ${FIND_ELApi_FAILURE_MESSAGE} )
endif ( CMAKE_GENERATOR_PLATFORM STREQUAL "x64" )

set( ELApi_HINT_BIN ${ELApi_DIR}/bin/${BIN_SUBDIR} )
set( ELApi_HINT_LIB ${ELApi_DIR}/bin/${BIN_SUBDIR} )
set( ELApi_HINT_INC ${ELApi_DIR}/include )

string( CONCAT ELApi_NAME_INC elapi/ELApi .h )
string( CONCAT ELApi_NAME_BIN ELApi ${ELApi_PLATFORM_SUFFIX} .dll )
string( CONCAT ELApi_NAME_LIB ELApi ${ELApi_PLATFORM_SUFFIX} )

find_path(ELApi_INCLUDE_DIR ${ELApi_NAME_INC}
          HINTS ${ELApi_HINT_INC} )

if( NOT ELApi_INCLUDE_DIR )
	set( ELApi_DIR_FALSE TRUE )
	message( WARNING "Could not find ${ELApi_NAME_INC} in ${ELApi_HINT_INC}!" )
	message( FATAL_ERROR ${FIND_ELApi_FAILURE_MESSAGE} )
endif( NOT ELApi_INCLUDE_DIR )

find_path(ELApi_BINARY_DIR NAMES ${ELApi_NAME_BIN}
             HINTS ${ELApi_HINT_BIN} )
if( NOT ELApi_BINARY_DIR )
	message( WARNING "Could not find ${ELApi_NAME_BIN} in ${ELApi_HINT_BIN}!" )
	message( FATAL_ERROR ${FIND_ELApi_FAILURE_MESSAGE} )
endif( NOT ELApi_BINARY_DIR )
string( CONCAT ELApi_BINARY ${ELApi_BINARY_DIR} / ${ELApi_NAME_BIN} )

find_library(ELApi_LIBRARY NAMES ${ELApi_NAME_LIB}
             HINTS ${ELApi_HINT_LIB} )
if( NOT ELApi_LIBRARY )
	message( WARNING "Could not find library ${ELApi_NAME_LIB} in ${ELApi_HINT_LIB}!" )
	message( FATAL_ERROR ${FIND_ELApi_FAILURE_MESSAGE} )
endif( NOT ELApi_LIBRARY )

include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set ELAPi_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(ELApi  DEFAULT_MSG
                                  ELApi_LIBRARY ELApi_INCLUDE_DIR)

mark_as_advanced(ELApi_INCLUDE_DIR ELApi_BINARY ELApi_LIBRARY )

file( RELATIVE_PATH ELApi_BINARY_DIR_REL ${CMAKE_CURRENT_SOURCE_DIR} ${ELApi_BINARY_DIR} )
file( GLOB ELApi_BINARIES ${ELApi_BINARY_DIR_REL}/*.dll )

set(ELApi_LIBRARIES ${ELApi_LIBRARY} )
set(ELApi_INCLUDE_DIRS ${ELApi_INCLUDE_DIR} )

