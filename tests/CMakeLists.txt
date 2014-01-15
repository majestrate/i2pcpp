file(GLOB i2p_test_sources *.cpp)
include_directories(../)

# Boost unit test
find_package(Boost 1.55.0 COMPONENTS system thread filesystem chrono date_time log unit_test_framework)
if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
    link_directories(${Boost_LIBRARIES})
else(Boost_FOUND)
    message(FATAL_ERROR "Boost 1.55.0 not found")
endif(Boost_FOUND)

add_executable(i2ptest ${i2p_test_sources})

target_link_libraries(i2ptest datatypes exceptions util)
target_link_libraries(i2ptest ${BOTAN_LIBRARIES})
target_link_libraries(i2ptest ${ZLIB_LIBRARIES})
target_link_libraries(i2ptest ${Boost_LIBRARIES})
