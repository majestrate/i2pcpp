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
    StatsBackend.cpp
    Version.cpp
    api/router_c.cpp
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
    tunnel/TunnelManager.cpp
    tunnel/Message.cpp
)

include(cpp11)

if("${CMAKE_BUILD_TYPE}" STREQUAL "MinSizeRel")
    get_target_property(i2p_LOCATION i2p LOCATION)
    add_custom_command(
        TARGET i2p
        POST_BUILD
        COMMAND ${CMAKE_STRIP} -s ${i2p_LOCATION})
endif("${CMAKE_BUILD_TYPE}" STREQUAL "MinSizeRel")

add_library(i2p SHARED ${i2pcpp_sources})

set(CMAKE_INCLUDE_CURRENT_DIR ON) # Needed for build.h
include_directories(${CMAKE_SOURCE_DIR}/include)

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

# --- EXTERNAL LIBRARIES ---

# pthreads
target_link_libraries(i2p "${CMAKE_THREAD_LIBS_INIT}")

# zlib
include_directories(BEFORE i2p ${ZLIB_INCLUDE_DIRS})
target_link_libraries(i2p "${ZLIB_LIBRARIES}")

# Sqlite3
include_directories(BEFORE i2p ${SQLITE3_INCLUDE_DIRS})
target_link_libraries(i2p "${SQLITE3_LIBRARIES}")

# Botan
include_directories(BEFORE i2p ${BOTAN_INCLUDE_DIRS})
target_link_libraries(i2p "${BOTAN_LIBRARIES}")

# Boost
include_directories(BEFORE i2p ${Boost_INCLUDE_DIRS})
target_link_libraries(i2p ${Boost_LIBRARIES})

add_definitions(-DBOOST_ALL_DYN_LINK)

# --- INTERNAL COMPONENTS ---

# datatypes library
target_link_libraries(i2p datatypes)

# util library
target_link_libraries(i2p util)

# sqlite3cc library
target_link_libraries(i2p sqlite3cc)
