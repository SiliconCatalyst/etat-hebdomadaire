cmake_minimum_required(VERSION 3.15)
project(etat-hebdo-dettes CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --- Sources ----------------------------------------------------------------
add_executable(etat-hebdo-dettes
    src/main.cpp
    src/http_server.cpp
    src/achats_locaux.cpp
)

# --- Include paths ------------------------------------------------------
target_include_directories(etat-hebdo-dettes PRIVATE
    include
    vendor
)

# --- Threads (httplib needs this) -------------------------------------------
find_package(Threads REQUIRED)
target_link_libraries(etat-hebdo-dettes PRIVATE Threads::Threads)

if(WIN32)
    target_link_libraries(etat-hebdo-dettes PRIVATE ws2_32)
endif()

# --- OpenXLSX -----------------------------------------------------------
# OpenXLSX fetches miniz, whose own CMakeLists.txt declares a minimum CMake
# version too old for CMake >= 4.0 to accept. This tells CMake to treat that
# old file as if it required 3.5, without affecting our own project's policies.
set(CMAKE_POLICY_VERSION_MINIMUM 3.5)
add_subdirectory(vendor/OpenXLSX)
target_link_libraries(etat-hebdo-dettes PRIVATE OpenXLSX::OpenXLSX)

# --- Copy web/ next to the built executable ---------------------------------
add_custom_command(TARGET etat-hebdo-dettes POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/web
        $<TARGET_FILE_DIR:etat-hebdo-dettes>/web
)

# --- Copy the platform-specific officeforge binary next to the built executable
if(WIN32)
    set(OFFICEFORGE_SRC ${CMAKE_SOURCE_DIR}/include/officeforge-windows-amd64.exe)
    set(OFFICEFORGE_DST officeforge.exe)
else()
    set(OFFICEFORGE_SRC ${CMAKE_SOURCE_DIR}/include/officeforge-linux-amd64)
    set(OFFICEFORGE_DST officeforge)
endif()

add_custom_command(TARGET etat-hebdo-dettes POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${OFFICEFORGE_SRC}
        $<TARGET_FILE_DIR:etat-hebdo-dettes>/${OFFICEFORGE_DST}
)

if(NOT WIN32)
    add_custom_command(TARGET etat-hebdo-dettes POST_BUILD
        COMMAND chmod +x $<TARGET_FILE_DIR:etat-hebdo-dettes>/${OFFICEFORGE_DST}
    )
endif()
