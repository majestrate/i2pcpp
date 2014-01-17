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
    RouterHash.cpp
    RouterIdentity.cpp
    RouterInfo.cpp
    SessionKey.cpp
)

add_library(datatypes STATIC ${datatypes_sources})

# Botan
include_directories(BEFORE datatypes ${BOTAN_INCLUDE_DIRS})

# Boost
include_directories(${Boost_INCLUDE_DIRS})
