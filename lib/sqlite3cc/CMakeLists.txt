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

include_directories(${CMAKE_SOURCE_DIR}/include)

add_library(sqlite3cc STATIC ${sqlite3cc_sources})

# Sqlite3
include_directories(BEFORE sqlite3cc ${SQLITE3_INCLUDE_DIRS})

# Boost
include_directories(${Boost_INCLUDE_DIRS})
