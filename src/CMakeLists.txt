set(i2pcpp_sources
    Database.cpp
    InboundMessageDispatcher.cpp
    Log.cpp
    OutboundMessageDispatcher.cpp
    PeerManager.cpp
    ProfileManager.cpp
    Router.cpp
    RouterContext.cpp
    Signals.cpp
    Version.cpp
    control/LoggingBackend.cpp
    control/Server.cpp
    dht/Kademlia.cpp
    dht/SearchManager.cpp
    dht/SearchState.cpp
    handlers/DatabaseSearchReply.cpp
    handlers/DatabaseStore.cpp
    handlers/DeliveryStatus.cpp
    handlers/Message.cpp
    handlers/TunnelData.cpp
    handlers/TunnelGateway.cpp
    handlers/VariableTunnelBuild.cpp
    handlers/VariableTunnelBuildReply.cpp
    i2np/DatabaseLookup.cpp
    i2np/DatabaseSearchReply.cpp
    i2np/DatabaseStore.cpp
    i2np/DeliveryStatus.cpp
    i2np/Message.cpp
    i2np/Garlic.cpp
    i2np/TunnelData.cpp
    i2np/TunnelGateway.cpp
    i2np/VariableTunnelBuild.cpp
    i2np/VariableTunnelBuildReply.cpp
    transport/ssu/AcknowledgementManager.cpp
    transport/ssu/EstablishmentManager.cpp
    transport/ssu/EstablishmentState.cpp
    transport/ssu/InboundMessageFragments.cpp
    transport/ssu/InboundMessageState.cpp
    transport/ssu/OutboundMessageFragments.cpp
    transport/ssu/OutboundMessageState.cpp
    transport/ssu/Packet.cpp
    transport/ssu/PacketBuilder.cpp
    transport/ssu/PacketHandler.cpp
    transport/ssu/PeerState.cpp
    transport/ssu/PeerStateList.cpp
    transport/Transport.cpp
    transport/UDPTransport.cpp
    tunnel/InboundTunnel.cpp
    tunnel/OutboundTunnel.cpp
    tunnel/Tunnel.cpp
    tunnel/Fragment.cpp
    tunnel/FirstFragment.cpp
    tunnel/FollowOnFragment.cpp
    tunnel/FragmentHandler.cpp
    tunnel/FragmentState.cpp
    tunnel/TunnelHop.cpp
    tunnel/TunnelManager.cpp
    tunnel/Message.cpp
)

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -stdlib=libc++")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -stdlib=libc++")
    set(CMAKE_CXX_FLAGS_MINSIZEREL "-Oz")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wno-constexpr-not-const")
else("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    message(WARNING "Beware: Only clang is officially supported!")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
endif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")

if("${CMAKE_BUILD_TYPE}" STREQUAL "MinSizeRel")
    get_target_property(i2p_LOCATION i2p LOCATION)
    add_custom_command(
        TARGET i2p
        POST_BUILD
        COMMAND ${CMAKE_STRIP} -s ${i2p_LOCATION})
endif("${CMAKE_BUILD_TYPE}" STREQUAL "MinSizeRel")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

add_executable(i2p main.cpp ${i2pcpp_sources})

# Version information
add_custom_command(
  OUTPUT build.h
  COMMAND "${CMAKE_SOURCE_DIR}/share/genbuild.sh" "${CMAKE_CURRENT_BINARY_DIR}/build.h"
  WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
  COMMENT "Generating version file")

add_custom_target(version ALL DEPENDS build.h)
add_definitions(-DHAVE_BUILD_INFO)
add_dependencies(i2p version)

# Database schema
add_custom_command(
  OUTPUT schema.sql.o
  COMMAND ${CMAKE_LINKER} -r -b binary -o ${CMAKE_CURRENT_BINARY_DIR}/schema.sql.o schema.sql
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/share
  COMMENT "Generating database schema object")

add_custom_target(schema ALL DEPENDS schema.sql.o)
add_dependencies(i2p schema)
target_link_libraries(i2p "${CMAKE_CURRENT_BINARY_DIR}/schema.sql.o")

set(CMAKE_INCLUDE_CURRENT_DIR ON)

# pthreads
find_package(Threads)
target_link_libraries (i2p "${CMAKE_THREAD_LIBS_INIT}")

# zlib
find_package(ZLIB REQUIRED)
include_directories(BEFORE i2p ${ZLIB_INCLUDE_DIRS})
target_link_libraries(i2p "${ZLIB_LIBRARIES}")

# Sqlite3
find_package(SQLite3 REQUIRED)
include_directories(BEFORE i2p ${SQLITE3_INCLUDE_DIRS})
target_link_libraries(i2p "${SQLITE3_LIBRARIES}")

# Botan
find_package(Botan REQUIRED)
include_directories(BEFORE i2p ${BOTAN_INCLUDE_DIRS})
target_link_libraries(i2p ${BOTAN_LIBRARIES})

# Boost
set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)
find_package(Boost 1.55.0 REQUIRED COMPONENTS system thread filesystem chrono date_time program_options log)
include_directories(BEFORE i2p ${Boost_INCLUDE_DIRS})
target_link_libraries(i2p ${Boost_LIBRARIES})
add_definitions(-DBOOST_ALL_DYN_LINK)

# websocketpp
find_path(WEBSOCKETPP_INCLUDE_DIRS websocketpp/server.hpp PATHS "${WEBSOCKETPP_INCLUDEDIR}" NO_DEFAULT_PATH)
find_path(WEBSOCKETPP_INCLUDE_DIRS websocketpp/server.hpp)

include_directories(BEFORE i2p ${WEBSOCKETPP_INCLUDE_DIRS})
add_definitions(-D_WEBSOCKETPP_CPP11_STL_)

# datatypes library
add_subdirectory(datatypes)
include_directories(datatypes)
target_link_libraries(i2p datatypes)

# exceptions library
add_subdirectory(exceptions)
include_directories(exceptions)
target_link_libraries(i2p exceptions)

# util library
add_subdirectory(util)
include_directories(util)
target_link_libraries(i2p util)

# sqlite3cc
add_subdirectory(sqlite3cc)
include_directories(sqlite3cc/include)
target_link_libraries(i2p sqlite3cc)

# tests
add_subdirectory(tests)
