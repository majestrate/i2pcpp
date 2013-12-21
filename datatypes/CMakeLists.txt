set(datatypes_sources
    Certificate.cpp
    ByteArray.cpp
    BuildRecord.cpp
    BuildRequestRecord.cpp
    BuildResponseRecord.cpp
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
include_directories(${Boost_INCLUDE_DIR})

add_library(datatypes STATIC ${datatypes_sources})
