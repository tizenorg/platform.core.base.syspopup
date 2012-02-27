# Test executables
SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${TEST_CFLAGS}")

INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/syspopup-caller)
add_executable(sp_test test.c) 
target_link_libraries(sp_test syspopup_caller ${pkgs_LDFLAGS})
INSTALL(TARGETS sp_test DESTINATION bin)

