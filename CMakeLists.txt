cmake_minimum_required(VERSION 3.15)
project(etat-hebdo-dettes CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --- Sources ----------------------------------------------------------------
add_executable(debt-kpi-tool
    src/main.cpp
    src/http_server.cpp
)

# --- Include paths ------------------------------------------------------
target_include_directories(debt-kpi-tool PRIVATE
    include
    vendor
)

# --- Threads (httplib needs this) -------------------------------------------
find_package(Threads REQUIRED)
target_link_libraries(debt-kpi-tool PRIVATE Threads::Threads)

if(WIN32)
    target_link_libraries(debt-kpi-tool PRIVATE ws2_32)
endif()

# --- Copy web/ next to the built executable ---------------------------------
add_custom_command(TARGET debt-kpi-tool POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/web
        $<TARGET_FILE_DIR:debt-kpi-tool>/web
)

# --- Copy the platform-specific officeforge binary next to the built executable
if(WIN32)
    set(OFFICEFORGE_SRC ${CMAKE_SOURCE_DIR}/include/officeforge-windows-amd64.exe)
    set(OFFICEFORGE_DST officeforge.exe)
else()
    set(OFFICEFORGE_SRC ${CMAKE_SOURCE_DIR}/include/officeforge-linux-amd64)
    set(OFFICEFORGE_DST officeforge)
endif()

add_custom_command(TARGET debt-kpi-tool POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${OFFICEFORGE_SRC}
        $<TARGET_FILE_DIR:debt-kpi-tool>/${OFFICEFORGE_DST}
)

if(NOT WIN32)
    add_custom_command(TARGET debt-kpi-tool POST_BUILD
        COMMAND chmod +x $<TARGET_FILE_DIR:debt-kpi-tool>/${OFFICEFORGE_DST}
    )
endif()
