set(datatypes_sources
    Certificate.cpp
    ByteArray.cpp
    BuildRecord.cpp
    BuildRequestRecord.cpp
    BuildResponseRecord.cpp
    Datatype.cpp
    Date.cpp
    Endpoint.cpp
    Mapping.cpp
    RouterAddress.cpp
    RouterIdentity.cpp
    RouterInfo.cpp
)

# Botan
include_directories(${BOTAN_INCLUDE_DIR})

# Boost
find_package(Boost 1.54.0 REQUIRED COMPONENTS system)
include_directories(BEFORE i2p ${Boost_INCLUDE_DIR})
add_definitions(-DBOOST_ALL_DYN_LINK)

add_library(datatypes STATIC ${datatypes_sources})
