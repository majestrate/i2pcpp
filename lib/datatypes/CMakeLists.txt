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
    SessionKey.cpp
)

include(cpp11)

add_library(datatypes STATIC ${datatypes_sources})

include_directories(BEFORE datatypes ${CMAKE_SOURCE_DIR}/include)

# Botan
include_directories(BEFORE datatypes ${BOTAN_INCLUDE_DIRS})

# Boost
include_directories(${Boost_INCLUDE_DIRS})
