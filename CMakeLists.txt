cmake_minimum_required(VERSION 3.16)

project(asdfmidi LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(rtmidi STATIC
    external/rtmidi/RtMidi.cpp
)

target_include_directories(rtmidi PUBLIC
    external/rtmidi
)

find_package(X11 REQUIRED)
find_library(ALSA_LIB asound REQUIRED)

add_executable(asdfmidi
    src/main.cpp
    src/midi_output.cpp
    src/keyboard_x11.cpp
    src/mapping.cpp
    src/app.cpp
)

target_include_directories(asdfmidi PRIVATE
    src
)

target_link_libraries(asdfmidi PRIVATE
    rtmidi
    X11::X11
    ${ALSA_LIB}
)
