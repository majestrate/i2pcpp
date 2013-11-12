set(datatypes_sources
    ssu/AcknowledgementScheduler.cpp
    ssu/EstablishmentManager.cpp
    ssu/EstablishmentState.cpp
    ssu/InboundMessageFragments.cpp
    ssu/InboundMessageState.cpp
    ssu/OutboundMessageFragments.cpp
    ssu/OutboundMessageState.cpp
    ssu/Packet.cpp
    ssu/PacketBuilder.cpp
    ssu/PacketHandler.cpp
    ssu/PeerState.cpp
    ssu/PeerStateList.cpp
    Transport.cpp
    UDPTransport.cpp
)

# Botan
include_directories(${BOTAN_INCLUDE_DIR})

# Boost
find_package(Boost 1.54.0 REQUIRED COMPONENTS system)
include_directories(BEFORE i2p ${Boost_INCLUDE_DIR})
add_definitions(-DBOOST_ALL_DYN_LINK)

add_library(transport STATIC ${datatypes_sources})
