# Note: temporary, move these to a suitable place later

set(exception_sources
    FormattingError.cpp
    RecordNotFound.cpp
    SQLError.cpp
    StatementPrepareError.cpp
)

add_library(exceptions STATIC ${exception_sources})
