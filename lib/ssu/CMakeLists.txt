set(ssu_sources
    AcknowledgementManager.cpp
    EstablishmentManager.cpp
    EstablishmentState.cpp
    InboundMessageState.cpp
    InboundMessageFragments.cpp
    OutboundMessageFragments.cpp
    OutboundMessageState.cpp
    Packet.cpp
    PacketBuilder.cpp
    PacketHandler.cpp
    PeerState.cpp
    PeerStateList.cpp
    Context.cpp
    SSU.cpp
)

include(cpp11)

add_library(ssu SHARED ${ssu_sources})

include_directories(BEFORE ssu ${CMAKE_SOURCE_DIR}/include)

# Botan
include_directories(BEFORE ssu ${BOTAN_INCLUDE_DIRS})
target_link_libraries(ssu "${BOTAN_LIBRARIES}")

# Boost
include_directories(${Boost_INCLUDE_DIRS})
target_link_libraries(ssu ${Boost_LIBRARIES})
add_definitions(-DBOOST_ALL_DYN_LINK)
