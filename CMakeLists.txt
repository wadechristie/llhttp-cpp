cmake_minimum_required(VERSION 3.18)

##
## PROJECT
##

project(llhttp-cpp VERSION 0.1.0 LANGUAGES CXX C)

##
## MAIN_PROJECT CHECK
##

set(MAIN_PROJECT OFF)
if (CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    set(MAIN_PROJECT ON)
endif()

##
## OPTIONS
##

if(${MAIN_PROJECT})
    set(LLHTTP_CPP_BuildTests_INIT         ON)
    set(LLHTTP_CPP_BuildDocs_INIT          ON)
else()
    set(LLHTTP_CPP_BuildTests_INIT         OFF)
    set(LLHTTP_CPP_BuildDocs_INIT          OFF)
endif()

option(LLHTTP_CPP_BuildTests       "Build the unittests."          ${LLHTTP_CPP_BuildTests_INIT})
option(LLHTTP_CPP_BuildDocs        "Build the documentation."      ${LLHTTP_CPP_BuildDocs_INIT})
option(LLHTTP_CPP_BuildTools       "Build the tools directory."    ON)
option(LLHTTP_CPP_UseASAN          "Build w/ address sanitizer."   OFF)
option(LLHTTP_CPP_UseGcov          "Build w/ code coverage."       OFF)

##
## CONFIGURATION
##

if(${MAIN_PROJECT})
    set(CMAKE_CXX_STANDARD 17)

    include(FetchContent)
endif()

##
## Address Sanitizer
##

if (MAIN_PROJECT AND LLHTTP_CPP_UseASAN AND NOT CERTIFY_UseGcov)
    message(AUTHOR_WARNING "LLHTTP_CPP_UseASAN: ON - Building w/ ASAN flags '-fsanitize=address -fno-omit-frame-pointer -fsanitize=undefined -fno-sanitize-recover=all'")
    add_compile_options(
        -fsanitize=address -fno-omit-frame-pointer
        -fsanitize=undefined -fno-sanitize-recover=all
    )
    link_libraries(
        -fsanitize=address -fno-omit-frame-pointer
        -fsanitize=undefined -fno-sanitize-recover=all
    )
endif()

##
## Code Coverage
##

if (MAIN_PROJECT AND LLHTTP_CPP_UseGcov)
    message(AUTHOR_WARNING "LLHTTP_CPP_UseGcov: ON - Building w/ code coverage")
    add_compile_options(-fprofile-arcs -ftest-coverage)
    link_libraries(gcov)
endif()

##
## llhttp
##

if(NOT TARGET llhttp::llhttp)
    FetchContent_Declare(
        llhttp
        URL "https://github.com/nodejs/llhttp/archive/refs/tags/release/v9.1.3.tar.gz"
    )

    set(BUILD_SHARED_LIBS OFF CACHE INTERNAL "")
    set(BUILD_STATIC_LIBS ON CACHE INTERNAL "")
    FetchContent_MakeAvailable(llhttp)
endif()

##
## Library
##

SET(LLHTTP_CPP_SRC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/llhttp-cpp.cpp
)

SET(LLHTTP_CPP_INC
    ${CMAKE_CURRENT_SOURCE_DIR}/include/llhttp-cpp.hpp
)

add_library(llhttp-cpp ${LLHTTP_CPP_INC} ${LLHTTP_CPP_SRC})
add_library(llhttp-cpp::llhttp-cpp ALIAS llhttp-cpp)
target_compile_features(llhttp-cpp PUBLIC cxx_std_17)
target_compile_options(llhttp-cpp PRIVATE -Wall -Werror)
target_link_libraries(llhttp-cpp PUBLIC llhttp_static)

target_include_directories(llhttp-cpp
PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include/>)

install(
    TARGETS llhttp-cpp
    DESTINATION lib
)
install(
    FILES ${LLHTTP_CPP_INC}
    DESTINATION include/
)
