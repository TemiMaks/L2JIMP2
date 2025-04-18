cmake_minimum_required(VERSION 3.10)
project(L2JIMP2 C)

set(CMAKE_C_STANDARD 11)

# --- libcurl setup (MinGW) ---
set(CURL_ROOT "C:/MinGW/curl-8.12.1_4-win64-mingw")
include_directories("${CURL_ROOT}/include")
link_directories("${CURL_ROOT}/lib")

# Locate libcurl
find_library(CURL_LIBRARY
        NAMES libcurl libcurl-x64 curl
        PATHS "${CURL_ROOT}/lib"
)
if(NOT CURL_LIBRARY)
 message(FATAL_ERROR "cURL library not found in ${CURL_ROOT}/lib")
endif()

# --- Main executable (with OpenAI/API) ---
add_executable(L2JIMP
        main.c
        api_comm.c
        graph_generator.c
        csrrg.c
        partition.c
        utils.c
        graph_matrix.c
)
target_link_libraries(L2JIMP
        PRIVATE ${CURL_LIBRARY} ws2_32 crypt32
)
# Output will be L2JIMP.exe on Windows

# --- Lightweight partition-only executable ---
add_executable(graphPartitioner_cli
        main_partition.c    # calls processCsrrgFile()
        csrrg.c
        partition.c
        utils.c
        graph_matrix.c
)
# No extra libraries needed for partitioning
# Output will be graphPartitioner_cli.exe on Windows
