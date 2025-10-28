cmake_minimum_required(VERSION 3.25)
project("kbd-lang-switcher" VERSION 0.1)

if(NOT WIN32)
message(FATAL "${PROJECT_NAME} is only for Windows")
endif()

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(CMAKE_CXX_COMPILER_ID MATCHES "Clang|MSVC" AND CMAKE_SYSTEM_NAME STREQUAL "Windows")
  set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>" CACHE STRING "MSVC Runtime Library" FORCE)
endif()

file(GLOB SOURCES "src/*.cpp")

add_executable(${PROJECT_NAME}
"${CMAKE_SOURCE_DIR}/assets/resources.rc"
${SOURCES}
)

if(WIN32)
  set_target_properties(${PROJECT_NAME} PROPERTIES WIN32_EXECUTABLE TRUE)
  target_compile_definitions(${PROJECT_NAME} PRIVATE UNICODE _UNICODE)
endif()

target_compile_definitions(${PROJECT_NAME} PRIVATE
  SRAL_STATIC
  wxMSVC_VERSION_ABI_COMPAT
  WIN32_LEAN_AND_MEAN
)

# Find dependencies
find_package(wxWidgets CONFIG REQUIRED COMPONENTS core base)
find_package(spdlog REQUIRED)

# --- Dependencies with FetchContent ---
include(FetchContent)
set(FETCHCONTENT_BASE_DIR ${CMAKE_BINARY_DIR}/_dependencies)

# SRAL
FetchContent_Declare(SRAL
  GIT_REPOSITORY https://github.com/m1maker/SRAL.git
  GIT_TAG "main"
  GIT_SHALLOW ON
  SYSTEM
)
set(BUILD_SRAL_TEST OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(SRAL)
add_library(SRAL::SRAL_static ALIAS SRAL_static)

target_include_directories(${PROJECT_NAME} PRIVATE
"${CMAKE_CURRENT_SOURCE_DIR}/src"
)

target_link_libraries(${PROJECT_NAME} PRIVATE
  SRAL::SRAL_static
  spdlog::spdlog
  wx::core
  wx::base
)

target_precompile_headers(${PROJECT_NAME} PRIVATE
<windows.h>
<spdlog/spdlog.h>
<wx/wxprec.h>
<wx/app.h>
)

install(TARGETS ${PROJECT_NAME} DESTINATION bin CONFIGURATIONS release)

include(InstallRequiredSystemLibraries)
set(CPACK_GENERATOR "NSIS")
set(CPACK_PACKAGE_NAME ${PROJECT_NAME}-installer)
set(CPACK_PACKAGE_VENDOR "Cyrmax")
set(CPACK_NSIS_INSTALLED_ICON_NAME "${CMAKE_SOURCE_DIR}/assets/icon.ico")
set(CPACK_NSIS_MUI_ICON "${CMAKE_SOURCE_DIR}/assets/icon.ico")
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/assets/nsis ${CMAKE_MODULE_PATH})

    include(CPack)
    configure_file(
    ${PROJECT_SOURCE_DIR}/assets/nsis/NSIS.definitions.nsh.in
    ${CMAKE_CURRENT_BINARY_DIR}/assets/nsis/NSIS.definitions.nsh
    )
