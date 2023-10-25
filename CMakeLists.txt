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
  set(CMAKE_C_FLAGS_DEBUG "-Og -g")
  set(CMAKE_ASM_FLAGS_DEBUG "-Og -g")
  set(CMAKE_EXE_LINKER_FLAGS_DEBUG "")
  add_compile_options(-Werror -gdwarf-4)
endif()

add_subdirectory(./OneMessage/src om.out)

target_include_directories(
    OneMessage
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
)

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
