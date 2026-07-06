cmake_minimum_required(VERSION 3.28)
project(CppProjectBase VERSION 0.0.1 LANGUAGES CXX)

# --- Runtime link selection ---
if (NOT DEFINED RUNTIME_LINK)
    message(STATUS "RUNTIME_LINK not defined, defaulting to 'dynamic'")
    set(RUNTIME_LINK "dynamic")
endif()
message(STATUS "Runtime link type: ${RUNTIME_LINK}")

# --- Configure MSVC runtime linkage ---
if (MSVC)
    if (RUNTIME_LINK STREQUAL "static")
        set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
        message(STATUS "Using static runtime (/MT or /MTd)")
    else()
        set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")
        message(STATUS "Using dynamic runtime (/MD or /MDd)")
    endif()
endif()

# --- Linux static and dynamic linkage settings
if(UNIX AND NOT APPLE)
    if(RUNTIME_LINK STREQUAL "static")
        # enable static linking
        message(STATUS "Linux Static linking")
        set(BUILD_SHARED_LIBS OFF)

        # Link libgcc and libstdc++ statically; however,  system libraries will remain dynamic
        message(STATUS "Using static linkage for libgcc and libstdc++")
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static-libgcc -static-libstdc++")

        # rpath settings to find shared libraries in the executable directory
        set(CMAKE_SKIP_BUILD_RPATH TRUE)
        set(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)
        
    else()
        message(STATUS "Linux dynamic linking")
        # portable RPATH
        set(CMAKE_SKIP_BUILD_RPATH FALSE)
        set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
        set(CMAKE_INSTALL_RPATH "$ORIGIN")
        set(CMAKE_INSTALL_RPATH_USE_LINK_PATH FALSE)

        # critical: disable RUNPATH
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--disable-new-dtags")

        # set(CMAKE_SKIP_RPATH FALSE)
        # set(CMAKE_SKIP_INSTALL_RPATH FALSE)
        # set(CMAKE_INSTALL_REMOVE_ENVIRONMENT_RPATH TRUE)
    endif()
endif()

# --- SFML (via Conan) ---
list(APPEND CMAKE_PREFIX_PATH "${CMAKE_BINARY_DIR}/generators")
find_package(SFML REQUIRED COMPONENTS system window graphics)

# --- Output directories ---
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}/bin)

# --- Executable setup ---
set(MY_TARGET cpp_project_base)
add_executable(${MY_TARGET} src/main.cpp)
target_compile_features(${MY_TARGET} PRIVATE cxx_std_17)
target_link_libraries(${MY_TARGET} PUBLIC sfml::sfml)

# --- Install required system libraries for dynamic runtime on Windows ---
if (MSVC AND RUNTIME_LINK STREQUAL "dynamic")
    include(InstallRequiredSystemLibraries)
    install(CODE [[
        include(InstallRequiredSystemLibraries)
        message(STATUS "Copying MSVC runtime for configuration: ${CMAKE_INSTALL_CONFIG_NAME}")
        file(COPY ${CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS} DESTINATION "${CMAKE_SOURCE_DIR}/bin")
    ]])
endif()