cmake_minimum_required(VERSION 3.10)
project(L2JIMP2 C)

set(CMAKE_C_STANDARD 11)

# Set the path to cURL
set(CURL_ROOT ${CMAKE_SOURCE_DIR}/Include/curl/curl-8.12.1)

# Include directories for cURL headers
include_directories(${CURL_ROOT}/include)

# Find the cURL library
find_library(CURL_LIBRARY
        NAMES curl libcurl libcurl.a libcurl.lib
        PATHS ${CURL_ROOT}/lib  # Adjust this if the library is elsewhere
        NO_DEFAULT_PATH         # Forces CMake to look only in specified paths
)

if(NOT CURL_LIBRARY)
    message(FATAL_ERROR "Could not find libcurl library. Please ensure it’s in ${CURL_ROOT}/lib")
endif()

# Add the executable
add_executable(L2JIMP2 main.c)

# Link the cURL library
target_link_libraries(L2JIMP2 PRIVATE ${CURL_LIBRARY})