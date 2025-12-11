cmake_minimum_required(VERSION 3.14)
project(RealTimeAudioPlayer LANGUAGES CXX)

# Use C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable warnings
if (MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# ---- Sources ----
# Adjust these if your files are named differently
set(SOURCES
    src/main.cpp
    src/AudioPlayer.cpp
    src/AudioBuffer.cpp
    src/AudioEngine.cpp
)

add_executable(audio_player ${SOURCES})

# If you use headers in an include/ folder:
target_include_directories(audio_player PRIVATE include)

# If you use an audio library like PortAudio, uncomment and adapt:
# find_package(PortAudio REQUIRED)
# target_link_libraries(audio_player PRIVATE PortAudio::PortAudio)

# On Linux you might need pthread:
if (UNIX)
    target_link_libraries(audio_player PRIVATE pthread)
endif()
