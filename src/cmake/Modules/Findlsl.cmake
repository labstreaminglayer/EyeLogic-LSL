# - Try to find lsl
# Once done this will define
#  LSL_FOUND - System has lsl
#  LSL_INCLUDE_DIRS - The lsl include directories
#  LSL_LIBRARIES - The libraries needed to use lsl
#  LSL_BINARIES - The biaries needed to run the .exe


set( FIND_LSL_FAILURE_MESSAGE "Could not find all necessary liblsl resources. Please check that LSL_DIR=\"${LSL_DIR}\" points to the directory containing the include, lib & bin directories of your desired liblsl version." )

set( LSL_HINT_INC ${LSL_DIR}/include )
set( LSL_HINT_BIN ${LSL_DIR}/bin )
set( LSL_HINT_LIB ${LSL_DIR}/lib )

set( LSL_NAME_INC lsl.h )
set( LSL_NAME_BIN lsl.dll )
set( LSL_NAME_LIB lsl )

find_path(LSL_INCLUDE_DIR lsl_cpp.h
          HINTS ${LSL_HINT_INC} )
if( NOT LSL_INCLUDE_DIR )
	message( WARNING "Could not find lsl_cpp.h in ${LSL_HINT_INC}!" )
	message( FATAL_ERROR ${FIND_LSL_FAILURE_MESSAGE} )
endif( NOT LSL_INCLUDE_DIR )

find_path(LSL_BINARY_DIR NAMES lsl.dll
             HINTS ${LSL_HINT_BIN} )
if( NOT LSL_BINARY_DIR )
	message( WARNING "Could not find lsl.dll in ${LSL_HINT_BIN}!" )
	message( FATAL_ERROR ${FIND_LSL_FAILURE_MESSAGE} )
endif( NOT LSL_BINARY_DIR )
string( CONCAT LSL_BINARY ${LSL_BINARY_DIR} "/lsl.dll" )

find_library(LSL_LIBRARY NAMES lsl
             HINTS ${LSL_HINT_LIB} )
if( NOT LSL_LIBRARY )
	message( WARNING "Could not find lsl.lib in ${LSL_HINT_LIB}!" )
	message( FATAL_ERROR ${FIND_LSL_FAILURE_MESSAGE} )
endif( NOT LSL_LIBRARY )

include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set LSL_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(lsl  DEFAULT_MSG
                                  LSL_LIBRARY LSL_INCLUDE_DIR)

mark_as_advanced(LSL_INCLUDE_DIR LSL_BINARY LSL_LIBRARY )

set( LSL_BINARIES ${LSL_BINARY} )
set(LSL_LIBRARIES ${LSL_LIBRARY} )
set(LSL_INCLUDE_DIRS ${LSL_INCLUDE_DIR} )

