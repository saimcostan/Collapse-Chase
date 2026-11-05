cmake_minimum_required(VERSION 3.24)
project(CollapseChase LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)

include(FetchContent)
set(SFML_BUILD_AUDIO OFF)
set(SFML_BUILD_NETWORK OFF)
FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 2.6.1
    GIT_SHALLOW ON
    FIND_PACKAGE_ARGS 2.6 COMPONENTS graphics window system)
FetchContent_MakeAvailable(SFML)

add_executable(game main.cpp)
target_link_libraries(game PRIVATE sfml-graphics sfml-window sfml-system)

add_custom_command(TARGET game POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_SOURCE_DIR}/assets $<TARGET_FILE_DIR:game>/assets)

if(WIN32)
    add_custom_command(TARGET game POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
                $<TARGET_RUNTIME_DLLS:game> $<TARGET_FILE_DIR:game>
        COMMAND_EXPAND_LISTS)
endif()