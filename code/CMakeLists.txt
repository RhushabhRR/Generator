

set(_LIB CanTpGenerator)

if(MSVC)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++17")
else(MSVC)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
endif(MSVC)

set(_INCLUDE_DIRECTORIES
    "${CMAKE_CURRENT_SOURCE_DIR}/api"
    "${CMAKE_CURRENT_SOURCE_DIR}/config"
    )

set(_HEADERS 
    "${CMAKE_CURRENT_SOURCE_DIR}/api/CanTpGenerator.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/config/CanTpGeneratorUserCfg.h"
    )

set(_SOURCES 
    "${CMAKE_CURRENT_SOURCE_DIR}/sources/CanTpGenerator.cpp"
    )

#Create libraries
add_library(${_LIB} STATIC ${_SOURCES})

#Include Directories for headers
target_include_directories(${_LIB} PUBLIC ${_INCLUDE_DIRECTORIES})

#Install libraries
install(TARGETS ${_LIB} 
        LIBRARY DESTINATION ${CMAKE_INSTALL_DIR}
        PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})


