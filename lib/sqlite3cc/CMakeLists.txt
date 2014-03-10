set(sqlite3cc_sources
    basic_statement.cc
    command.cc
    connection.cc
    exception.cc
    manipulator.cc
    query.cc
    row.cc
    transaction.cc
    util.cc
)

add_library(sqlite3cc SHARED ${sqlite3cc_sources})

include_directories(BEFORE sqlite3cc ${CMAKE_SOURCE_DIR}/include)

# Sqlite3
include_directories(BEFORE sqlite3cc ${SQLITE3_INCLUDE_DIRS})
target_link_libraries(sqlite3cc "${SQLITE3_LIBRARIES}")

# Boost
include_directories(${Boost_INCLUDE_DIRS})
