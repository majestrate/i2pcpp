set(datatypes_sources
    Certificate.cpp
    ByteArray.cpp
    BuildRecord.cpp
    BuildRequestRecord.cpp
    BuildResponseRecord.cpp
    Date.cpp
    Endpoint.cpp
    Lease.cpp
    Mapping.cpp
    RouterAddress.cpp
    RouterIdentity.cpp
    RouterInfo.cpp
    SessionKey.cpp
)

include(cpp11)

add_library(datatypes SHARED ${datatypes_sources})

include_directories(BEFORE datatypes ${CMAKE_SOURCE_DIR}/include)

# Botan
include_directories(BEFORE datatypes ${BOTAN_INCLUDE_DIRS})
target_link_libraries(datatypes "${BOTAN_LIBRARIES}")

# Boost
include_directories(${Boost_INCLUDE_DIRS})
target_link_libraries(datatypes ${Boost_LIBRARIES})
add_definitions(-DBOOST_ALL_DYN_LINK)
