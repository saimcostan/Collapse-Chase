cmake_minimum_required(VERSION 3.24)
project(CollapseChase LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)

include(FetchContent)
set(SFML_BUILD_AUDIO ON)
set(SFML_BUILD_NETWORK OFF)
FetchContent_Declare(SFML
    URL https://github.com/SFML/SFML/archive/refs/tags/2.6.1.zip
    FIND_PACKAGE_ARGS 2.6 COMPONENTS graphics window system audio)
FetchContent_MakeAvailable(SFML)

add_executable(game main.cpp)
set_property(TARGET game PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}")
target_link_libraries(game PRIVATE sfml-graphics sfml-window sfml-system sfml-audio)

add_custom_command(TARGET game POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_SOURCE_DIR}/assets $<TARGET_FILE_DIR:game>/assets)
add_custom_command(TARGET game POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_SOURCE_DIR}/level.txt
        $<TARGET_FILE_DIR:game>/level.txt)
if(WIN32)
    add_custom_command(TARGET game POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
                $<TARGET_RUNTIME_DLLS:game> $<TARGET_FILE_DIR:game>
        COMMAND_EXPAND_LISTS)
endif()
if(WIN32)
    add_custom_command(TARGET game POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${sfml_SOURCE_DIR}/extlibs/bin/x64/openal32.dll"
            $<TARGET_FILE_DIR:game>)
endif()