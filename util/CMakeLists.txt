set(util_sources
    Base64.cpp
    I2PDH.cpp
    I2PHMAC.cpp
    gzip.cpp
)

add_library(util STATIC ${util_sources})

# Botan
include_directories(BEFORE util ${BOTAN_INCLUDE_DIRS})

# Boost
include_directories(${Boost_INCLUDE_DIRS})
