set(i2pd_sources
    main.cpp
    Logger.cpp
    Server.cpp
    StatsBackend.cpp
)

include(cpp11)

# websocketpp
add_definitions(-D_WEBSOCKETPP_CPP11_STL_)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

include_directories(${CMAKE_SOURCE_DIR}/include)

add_executable(i2pd ${i2pd_sources})
set_target_properties(i2pd PROPERTIES OUTPUT_NAME i2p)
target_link_libraries(i2pd i2p)

# Boost
include_directories(BEFORE i2pd ${Boost_INCLUDE_DIRS})
target_link_libraries(i2pd ${Boost_LIBRARIES})

# botan
include_directories(BEFORE i2pd ${BOTAN_INCLUDE_DIRS})

add_definitions(-DBOOST_ALL_DYN_LINK)
