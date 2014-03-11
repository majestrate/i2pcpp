include(cpp11)

include_directories(${CMAKE_SOURCE_DIR}/include)

add_executable(i2pd main.cpp Logger.cpp StatsBackend.cpp)
set_target_properties(i2pd PROPERTIES OUTPUT_NAME i2p)
target_link_libraries(i2pd i2p)

# Boost
include_directories(BEFORE i2pd ${Boost_INCLUDE_DIRS})
target_link_libraries(i2pd ${Boost_LIBRARIES})

add_definitions(-DBOOST_ALL_DYN_LINK)
