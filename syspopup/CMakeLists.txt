CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
SET(CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS true)

PROJECT(syspopup C)
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

SET(APPSVC_SPPPGS_CHECK_MODULES "dlog bundle sqlite3 glib-2.0 gio-2.0 evas elementary ecore ecore-input libtzplatform-config")
SET(pc_requires "bundle elementary")

IF (with_wayland)
	pkg_check_modules(sppkgs REQUIRED ${APPSVC_SPPPGS_CHECK_MODULES})
ELSE (with_wayland)
	SET(pc_requires "${pc_requires} utilX")
	pkg_check_modules(sppkgs REQUIRED ${APPSVC_SPPPGS_CHECK_MODULES} utilX ecore-x x11)
ENDIF (with_wayland)

FOREACH(flag ${sppkgs_CFLAGS})
	SET(EXTRA_CFLAGS "${EXTRA_CFLAGS} ${flag}")
ENDFOREACH(flag)

# Compiler flags
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

INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/include)
ADD_DEFINITIONS("-DPREFIX=\"${CMAKE_INSTALL_PREFIX}\"")

# Linker flags
SET(CMAKE_EXE_LINKER_FLAGS "-Wl,--as-needed")


### Build ###

add_library(syspopup SHARED syspopup.c syspopup_efl.c)
target_link_libraries(syspopup syspopup_mods ${sppkgs_LDFLAGS})
SET_TARGET_PROPERTIES(syspopup PROPERTIES SOVERSION 0.1.0)

SET(PC_NAME libsyspopup)
SET(PC_REQUIRED ${pc_requires})
SET(PC_LDFLAGS -lsyspopup)

# pkgconfig file
#CONFIGURE_FILE(syspopup.pc.in syspopup/syspopup.pc @ONLY)
IF (with_wayland)
    CONFIGURE_FILE(syspopup-wayland.pc.in ${CMAKE_CURRENT_BINARY_DIR}/syspopup.pc @ONLY)
ELSE (with_wayland)
    CONFIGURE_FILE(syspopup-x.pc.in ${CMAKE_CURRENT_BINARY_DIR}/syspopup.pc @ONLY)
ENDIF (with_wayland)

### Install ###
INSTALL(TARGETS syspopup DESTINATION ${LIB_INSTALL_DIR} COMPONENT RuntimeLibraries)
INSTALL(FILES ${CMAKE_CURRENT_SOURCE_DIR}/../include/syspopup.h DESTINATION include)
INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/syspopup.pc DESTINATION ${LIB_INSTALL_DIR}/pkgconfig)
