cmake_minimum_required(VERSION 3.10)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  set(CMAKE_C_FLAGS_DEBUG "-Og -g")
  set(CMAKE_ASM_FLAGS_DEBUG "-Og -g")
  set(CMAKE_EXE_LINKER_FLAGS_DEBUG "")
  add_compile_options(-Werror -gdwarf-4)

elseif(CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
  set(CMAKE_C_FLAGS_RELEASE "-O3 -g")
  set(CMAKE_ASM_FLAGS_RELEASE "-O3 -g")
  set(CMAKE_EXE_LINKER_FLAGS_RELEASE "")
  add_compile_options(-Werror -gdwarf-4)

elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
  set(CMAKE_C_FLAGS_RELEASE "-O3")
  set(CMAKE_ASM_FLAGS_RELEASE "-O3")
  set(CMAKE_EXE_LINKER_FLAGS_RELEASE "")
  add_compile_definitions()

elseif(CMAKE_BUILD_TYPE STREQUAL "MinSizeRel")
  set(CMAKE_C_FLAGS_RELEASE "-Os")
  set(CMAKE_ASM_FLAGS_RELEASE "-Os")
  set(CMAKE_EXE_LINKER_FLAGS_RELEASE "")
  add_compile_definitions()
else()
  message(FATAL_ERROR "Unknown build type.")
endif()

execute_process(COMMAND "./findcheck.sh"
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/CMake
  RESULT_VARIABLE CHECK_FOUNDED OUTPUT_QUIET ERROR_QUIET
)

add_subdirectory(./OneMessage/src om.out)

if(${CHECK_FOUNDED})
add_definitions(-DOM_TEST)
target_include_directories(
    OneMessage
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/OneMessage/test
)
else()
add_definitions(-DOM_RELEASE)
target_include_directories(
    OneMessage
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
)
endif()


project(om_cli)

add_executable(om_cli om_cli.cpp om_cli.hpp)

add_dependencies(om_cli OneMessage)

target_link_libraries(
    om_cli
    PUBLIC OneMessage
)

target_include_directories(
    om_cli
    PUBLIC $<TARGET_PROPERTY:OneMessage,INTERFACE_INCLUDE_DIRECTORIES>
)
