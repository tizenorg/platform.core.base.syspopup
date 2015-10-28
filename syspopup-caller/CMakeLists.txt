CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
SET(CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS true)

PROJECT(syspopup_caller C)
SET(VERSION 1.1)

### Global setting ###

SET(PREFIX ${CMAKE_INSTALL_PREFIX})
SET(EXEC_PREFIX "\${prefix}")
SET(LIBDIR "\${prefix}/lib")
SET(INCLUDEDIR "\${prefix}/include")

# Build type : Release
IF("${CMAKE_BUILD_TYPE}" STREQUAL "")
	SET(CMAKE_BUILD_TYPE "Release")
ENDIF()
MESSAGE("Build type: ${CMAKE_BUILD_TYPE}")

# Set required packages
INCLUDE(FindPkgConfig)

SET(pc_requires "bundle")

pkg_check_modules(spcpkgs REQUIRED dlog bundle sqlite3 glib-2.0 gio-2.0 libtzplatform-config)

FOREACH(flag ${spcpkgs_CFLAGS})
	SET(EXTRA_CFLAGS "${EXTRA_CFLAGS} ${flag}")
ENDFOREACH(flag)

# Compiler flags
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/include)
SET(EXTRA_CFLAGS "${EXTRA_CFLAGS} -Wl,-zdefs" )
SET(EXTRA_CFLAGS "${EXTRA_CFLAGS} -fvisibility=hidden")
SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${EXTRA_CFLAGS}")
SET(CMAKE_C_FLAGS_DEBUG "-O0 -g")
SET(CMAKE_C_FLAGS_RELEASE "-O2")

SET(CMAKE_SKIP_BUILD_RPATH true)

# Get uname value to set 'TARGET' definition
# TODO: Is this needed?
FIND_PROGRAM(UNAME NAMES uname)
EXEC_PROGRAM("${UNAME}" ARGS "-m" OUTPUT_VARIABLE "ARCH")
IF("${ARCH}" STREQUAL "arm")
	ADD_DEFINITIONS("-DTARGET")
	MESSAGE("add -DTARGET")
ENDIF("${ARCH}" STREQUAL "arm")

ADD_DEFINITIONS("-DPREFIX=\"${CMAKE_INSTALL_PREFIX}\"")

# Linker flags
SET(CMAKE_EXE_LINKER_FLAGS "-Wl,--as-needed")

### Build ###

add_library(syspopup_caller SHARED syspopup_caller.c)
target_link_libraries(syspopup_caller syspopup_mods ${spcpkgs_LDFLAGS})
SET_TARGET_PROPERTIES(syspopup_caller PROPERTIES SOVERSION 0.1.0)

SET(PC_NAME libsyspopup_caller)
SET(PC_REQUIRES ${pc_requires})
SET(PC_LDFLAG -lsyspopup_caller)

# pkgconfig file
#CONFIGURE_FILE(syspopup-caller.pc.in syspopup-caller/syspopup-caller.pc @ONLY)
CONFIGURE_FILE(syspopup-caller.pc.in ${CMAKE_CURRENT_BINARY_DIR}/syspopup-caller.pc @ONLY)

### Install ###
INSTALL(TARGETS syspopup_caller DESTINATION ${LIB_INSTALL_DIR} COMPONENT RuntimeLibraries)
INSTALL(FILES ${CMAKE_CURRENT_SOURCE_DIR}/../include/syspopup_caller.h DESTINATION include)
INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/syspopup-caller.pc DESTINATION ${LIB_INSTALL_DIR}/pkgconfig)

