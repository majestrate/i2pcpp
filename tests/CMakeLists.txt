set(test_sources
    Datatypes.cpp
)

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -stdlib=libc++")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -stdlib=libc++")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wno-constexpr-not-const")
else("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    message(WARNING "Beware: Only clang is officially supported!")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
endif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")

add_executable(testi2p ${test_sources})

# Botan
include_directories(BEFORE testi2p ${BOTAN_INCLUDE_DIRS})
target_link_libraries(testi2p ${BOTAN_LIBRARIES})

# zlib
include_directories(BEFORE testi2p ${ZLIB_INCLUDE_DIRS})
target_link_libraries(testi2p ${ZLIB_LIBRARIES})

# Boost
include_directories(BEFORE testi2p ${Boost_INCLUDE_DIRS})
target_link_libraries(testi2p ${Boost_LIBRARIES})
add_definitions(-DBOOST_ALL_DYN_LINK)

# i2pcpp
include_directories(BEFORE testi2p ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(testi2p datatypes exceptions util)
