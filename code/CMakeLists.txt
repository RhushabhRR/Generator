

set(_LIB CanGenerator)

set(_INCLUDE_DIRECTORIES
    "${CMAKE_CURRENT_SOURCE_DIR}/api"
    )

set(_HEADERS 
    "${CMAKE_CURRENT_SOURCE_DIR}/api/CanGenerator.h"
    )

set(_SOURCES 
    "${CMAKE_CURRENT_SOURCE_DIR}/sources/CanGenerator.cpp"
    )

add_library(CanGenerator sources/CanGenerator.cpp)

target_include_directories(${_LIB} PUBLIC ${_INCLUDE_DIRECTORIES})
target_sources(${_LIB}
    PRIVATE ${_SOURCES}
    INTERFACE ${_HEADERS}
    )

##################
add_executable(APP_CanGenerator ${_SOURCES})
target_link_libraries(APP_CanGenerator ${_LIB})

message ("included sources")

