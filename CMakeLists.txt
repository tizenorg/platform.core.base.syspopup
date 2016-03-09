CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
SET(CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS true)

PROJECT(syspopup_mods C)
SET(VERSION 1.1)

### Global setting ###

SET(PREFIX ${CMAKE_INSTALL_PREFIX})
SET(EXEC_PREFIX "\${prefix}")
SET(INCLUDEDIR "\${prefix}/include")

# Build type : Release
IF("${CMAKE_BUILD_TYPE}" STREQUAL "")
	SET(CMAKE_BUILD_TYPE "Release")
ENDIF()
MESSAGE("Build type: ${CMAKE_BUILD_TYPE}")

# Set required packages
INCLUDE(FindPkgConfig)

IF(with_wayland)
	ADD_DEFINITIONS("-DWAYLAND")
ENDIF(with_wayland)
IF(with_x11)
	ADD_DEFINITIONS("-DX11")
ENDIF(with_x11)

pkg_check_modules(libpkgs REQUIRED dlog bundle sqlite3 glib-2.0 gio-2.0 libtzplatform-config)

FOREACH(flag ${libpkgs_CFLAGS})
	SET(EXTRA_CFLAGS "${EXTRA_CFLAGS} ${flag}")
ENDFOREACH(flag)

# Compiler flags
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/include)
SET(EXTRA_CFLAGS "${EXTRA_CFLAGS} -Wl,-zdefs" )
SET(EXTRA_CFLAGS "${EXTRA_CFLAGS} -fvisibility=hidden")
SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${EXTRA_CFLAGS} -g")
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

# aul_mods : modules (static library)
add_library(syspopup_mods STATIC
		src/syspopup_db.c
		src/syspopup_core.c
		)

INSTALL(FILES ${CMAKE_CURRENT_SOURCE_DIR}/include/SLP_SYSPOPUP_PG.h DESTINATION include)
INSTALL(FILES ${CMAKE_CURRENT_SOURCE_DIR}/data/syspopup_db.sql DESTINATION ${TZ_SYS_RO_SHARE}/syspopup)

add_subdirectory(syspopup)
add_subdirectory(syspopup-caller)

# test
add_subdirectory(test)
add_subdirectory(syspopup-app)
