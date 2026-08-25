cmake_minimum_required(VERSION 3.10)
project(HookCord LANGUAGES C CXX DESCRIPTION "A small tool to interact with discord webhook system." VERSION 1.2.0) #add C because of dependencies
string(TOUPPER ${PROJECT_NAME} PROJECT_NAME_UPPER)
add_compile_definitions(APP_NAME="${PROJECT_NAME}")

set (CMAKE_CXX_STANDARD 20)
set (CMAKE_CXX_STANDARD_REQUIRED ON)

configure_file(
    ${CMAKE_SOURCE_DIR}/Headers/Core/Version.hpp.in
    ${CMAKE_BINARY_DIR}/Generated/Headers/Core/Version.hpp
    @ONLY
)

#--GLFW---------------------------------------------------------------
set( GLFW_BUILD_DOCS OFF CACHE BOOL  "GLFW lib only" )
set( GLFW_INSTALL OFF CACHE BOOL  "GLFW lib only" )
add_subdirectory( "Externals/GLFW" )
#--GLAD---------------------------------------------------------------
add_subdirectory( "Externals/GLAD/cmake" )
glad_add_library(glad_gl STATIC
    REPRODUCIBLE
    API gl:core=4.6   # change version if needed
)
#--CURL---------------------------------------------------------------
if(WIN32)
    set(CURL_USE_SCHANNEL ON CACHE INTERNAL "")
else()
    set(CURL_USE_OPENSSL ON CACHE INTERNAL "")
endif()

set(HTTP_ONLY ON CACHE INTERNAL "") 

set(BUILD_CURL_EXE OFF CACHE INTERNAL "")
set(BUILD_TESTING OFF CACHE INTERNAL "")

set(CURL_DISABLE_LDAP ON CACHE INTERNAL "")
set(CURL_DISABLE_LDAPS ON CACHE INTERNAL "")

set(CURL_USE_LIBSSH2 OFF CACHE INTERNAL "")
set(USE_LIBIDN2 OFF CACHE INTERNAL "")
set(CURL_USE_LIBPSL OFF CACHE INTERNAL "")

set(CURL_ZLIB OFF CACHE INTERNAL "")
set(CURL_BROTLI OFF CACHE INTERNAL "")
set(CURL_ZSTD OFF CACHE INTERNAL "")

set(USE_NGHTTP2 OFF CACHE INTERNAL "")

add_subdirectory( "Externals/CURL" )
#--IMGUI--------------------------------------------------------------
include_directories( "Externals/IMGUI" )
include_directories( "Externals/JSON_CPP/JSON_CPP" )
set(EXTERNALS
    "Externals/IMGUI/imgui.cpp"
    "Externals/IMGUI/imgui_demo.cpp"
    "Externals/IMGUI/imgui_draw.cpp"
    "Externals/IMGUI/imgui_tables.cpp"
    "Externals/IMGUI/imgui_widgets.cpp"
    "Externals/IMGUI/backends/imgui_impl_opengl3.cpp"
    "Externals/IMGUI/backends/imgui_impl_glfw.cpp"
    "Externals/IMGUI/misc/cpp/imgui_stdlib.cpp"
    "Externals/JSON_CPP/JSON_CPP/JSON_CPP.cpp"
)
#----------------------------------------------------------------------

set(PRECOMPILED_HEADER "Headers/pch.hpp")
set(SOURCES
    "Sources/main.cpp"
    "Sources/App.cpp"
    "Sources/Data.cpp"
)

set(PLATFORM_SOURCES)
if(WIN32)
    set(PLATFORM_SOURCES
        "Sources/Platforms/Windows/Utils.cpp"
    )
else()
    set(PLATFORM_SOURCES
        "Sources/Platforms/Linux/Utils.cpp"
    )
endif()

# app.rc
if(WIN32)
    string(TIMESTAMP CURRENT_YEAR "%Y")
    set(PROJECT_AUTHOR "Motisma479")
    configure_file(
        ${CMAKE_SOURCE_DIR}/app.rc.in
        ${CMAKE_BINARY_DIR}/Generated/app.rc
        @ONLY
    )

    list(APPEND SOURCES "${CMAKE_BINARY_DIR}/Generated/app.rc")
endif()


add_executable(${PROJECT_NAME} ${EXTERNALS} ${SOURCES} ${PLATFORM_SOURCES})
target_link_libraries( ${PROJECT_NAME} PRIVATE glfw glad_gl libcurl_static)

target_compile_definitions(${PROJECT_NAME} PRIVATE CURL_STATICLIB)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/Headers
    ${CMAKE_BINARY_DIR}/Generated/Headers
)

target_precompile_headers(${PROJECT_NAME} PRIVATE ${PRECOMPILED_HEADER})

target_include_directories(${PROJECT_NAME} PUBLIC
  "Externals/GLAD/include"
)

if(WIN32 AND CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_link_options(${PROJECT_NAME} PRIVATE
        -static
        -static-libgcc
        -static-libstdc++
    )
endif()
target_link_options(${PROJECT_NAME} PRIVATE
    $<$<AND:$<CXX_COMPILER_ID:MSVC>,$<PLATFORM_ID:Windows>,$<NOT:$<CONFIG:Debug>>>:/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup>
    $<$<AND:$<CXX_COMPILER_ID:GNU>,$<PLATFORM_ID:Windows>,$<NOT:$<CONFIG:Debug>>>:-mwindows>
)
