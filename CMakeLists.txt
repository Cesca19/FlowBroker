cmake_minimum_required(VERSION 3.28)
project(FlowBroker VERSION 0.0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)

# --- Runtime link selection ---
if (NOT DEFINED RUNTIME_LINK)
    message(STATUS "RUNTIME_LINK not defined, defaulting to 'dynamic'")
    set(RUNTIME_LINK "dynamic")
endif()
message(STATUS "Runtime link type: ${RUNTIME_LINK}")

# --- Configure MSVC runtime linkage (project default, used by targets that
# don't override it, i.e. the server) ---
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

        set(CMAKE_SKIP_BUILD_RPATH TRUE)
        set(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)
    else()
        message(STATUS "Linux dynamic linking")
        set(CMAKE_SKIP_BUILD_RPATH FALSE)
        set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
        set(CMAKE_INSTALL_RPATH "$ORIGIN")
        set(CMAKE_INSTALL_RPATH_USE_LINK_PATH FALSE)

        # disable RUNPATH
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--disable-new-dtags")
    endif()
endif()

#  (via Conan) ---
list(APPEND CMAKE_PREFIX_PATH "${CMAKE_BINARY_DIR}/generators")
find_package(Qt6 REQUIRED COMPONENTS Widgets Network Charts OpenGLWidgets)
find_package(Boost REQUIRED)
find_package(Threads REQUIRED)

# --- Output directories ---
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}/bin)

# --- Server Executable setup ---
set(SERVER flow_broker_server)
add_executable(${SERVER}
        src/server/main.cpp
        src/server/data_sources/SimulatedDataSource.cpp
        src/server/data_sources/finance/SimulatedFinancialAsset.cpp
        src/server/data_sources/finance/SimulatedFinanceDataSource.cpp
        src/server/pipeline/MessageCatalog.cpp
        src/server/pipeline/MessageProducer.cpp
        src/server/pipeline/MessageConsumer.cpp
        src/server/topics/TopicState.cpp
        src/server/topics/TopicCache.cpp
        src/server/network/TcpConnection.cpp
        src/server/network/TcpServer.cpp
        src/server/network/UdpServer.cpp
        src/server/network/Server.cpp
)
target_compile_features(${SERVER} PRIVATE cxx_std_17)
target_link_libraries(${SERVER} PRIVATE Boost::boost Threads::Threads)

# --- Client Executable setup ---
set(CLIENT flow_broker_client)
add_executable(${CLIENT}
        src/client/main.cpp
        src/client/QTcpClient.cpp
        src/client/TopicGraph.cpp
        src/client/StaticPlugins.cpp
        )
target_compile_features(${CLIENT} PRIVATE cxx_std_17)

set(CLIENT_QT_LIBS "")
if(RUNTIME_LINK STREQUAL "static")
    target_compile_definitions(${CLIENT} PRIVATE QT_STATIC_PLATFORM_PLUGIN)
    if(UNIX AND NOT APPLE)
        list(APPEND CLIENT_QT_LIBS Qt6::QXcbIntegrationPlugin)
    elseif(WIN32)
        list(APPEND CLIENT_QT_LIBS Qt6::QWindowsIntegrationPlugin)
    endif()
endif()
list(APPEND CLIENT_QT_LIBS Qt6::Widgets Qt6::Network Qt6::Charts Qt6::OpenGLWidgets)
target_link_libraries(${CLIENT} PRIVATE ${CLIENT_QT_LIBS})

# --- Install required system libraries for dynamic runtime on Windows ---
if (MSVC AND RUNTIME_LINK STREQUAL "dynamic")
    include(InstallRequiredSystemLibraries)
    install(CODE [[
        include(InstallRequiredSystemLibraries)
        message(STATUS "Copying MSVC runtime for configuration: ${CMAKE_INSTALL_CONFIG_NAME}")
        file(COPY ${CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS} DESTINATION "${CMAKE_SOURCE_DIR}/bin")
    ]])
endif()