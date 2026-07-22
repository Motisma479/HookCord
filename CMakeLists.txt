cmake_minimum_required(VERSION 3.10)
project(HookCord)

set (CMAKE_CXX_STANDARD 20)
add_compile_definitions(HOOKCORD_VERSION="0.0.0") # Remember to also change it in the code and workflow

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
    set(CURL_USE_SCHANNEL ON CACHE INTERNAL "Utiliser le SSL natif de Windows")
else()
    set(CURL_USE_OPENSSL ON CACHE INTERNAL "Utiliser OpenSSL")
endif()
set(HTTP_ONLY ON CACHE INTERNAL "Activer uniquement le protocole HTTP") 
set(BUILD_CURL_EXE OFF CACHE INTERNAL "Désactiver l'exécutable curl")
set(BUILD_TESTING OFF CACHE INTERNAL "Désactiver les tests cURL")
set(CURL_DISABLE_LDAP ON CACHE INTERNAL "Désactiver le support LDAP")
add_subdirectory( "Externals/CURL" )
#--IMGUI--------------------------------------------------------------
include_directories( "Externals/IMGUI" )
set(EXTERNALS
    "Externals/IMGUI/imgui.cpp"
    "Externals/IMGUI/imgui_demo.cpp"
    "Externals/IMGUI/imgui_draw.cpp"
    "Externals/IMGUI/imgui_tables.cpp"
    "Externals/IMGUI/imgui_widgets.cpp"
    "Externals/IMGUI/backends/imgui_impl_opengl3.cpp"
    "Externals/IMGUI/backends/imgui_impl_glfw.cpp"
    "Externals/IMGUI/misc/cpp/imgui_stdlib.cpp"
)
#----------------------------------------------------------------------

include_directories("Headers")

set(PRECOMPILED_HEADER "Headers/pch.hpp")
set(SOURCES
    "Sources/main.cpp"
)

if(NOT CMAKE_BUILD_TYPE MATCHES Debug)
    # Disable console for Window application
    if( MSVC )
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup" )
        target_compile_definitions(${PROJECT_NAME} PRIVATE CURL_STATICLIB)
    else()
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -mwindows")
    endif()
endif()

#add flags to make the executable static
set(FLAGS_LINKER "-static -static-libgcc -static-libstdc++")
set(CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} ${FLAGS_LINKER}")

add_executable(${PROJECT_NAME} ${EXTERNALS} ${SOURCES})
target_link_libraries( ${PROJECT_NAME} PRIVATE glfw glad_gl libcurl_static)

target_precompile_headers(${PROJECT_NAME} PRIVATE ${PRECOMPILED_HEADER})

target_include_directories(${PROJECT_NAME} PUBLIC
  "Externals/GLAD/include"
)