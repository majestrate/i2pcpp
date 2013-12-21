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
include_directories(${Boost_INCLUDE_DIR})

add_library(transport STATIC ${datatypes_sources})
