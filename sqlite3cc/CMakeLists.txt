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

include_directories(include)

add_library(sqlite3cc STATIC ${sqlite3cc_sources})

# Boost
include_directories(${Boost_INCLUDE_DIRS})
