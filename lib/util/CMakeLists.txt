set(util_sources
    Base64.cpp
    I2PDH.cpp
    I2PHMAC.cpp
    gzip.cpp
)

include(cpp11)

add_library(util SHARED ${util_sources})

include_directories(BEFORE util ${CMAKE_SOURCE_DIR}/include)

# Botan
include_directories(BEFORE util ${BOTAN_INCLUDE_DIRS})
target_link_libraries(util "${BOTAN_LIBRARIES}")

# Boost
include_directories(${Boost_INCLUDE_DIRS})
target_link_libraries(util ${Boost_LIBRARIES})
add_definitions(-DBOOST_ALL_DYN_LINK)

# zlib
include_directories(BEFORE util ${ZLIB_INCLUDE_DIRS})
target_link_libraries(util "${ZLIB_LIBRARIES}")
