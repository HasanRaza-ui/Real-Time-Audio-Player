# Real-Time Audio Player (C++17)

A lightweight, real-time audio playback engine written in Modern C++17.  
Designed for low-latency streaming, buffer management, and stable playback across platforms.

## 🚀 Features
- Real-time audio playback with double-buffering
- Efficient circular buffer for low-latency streaming
- Uses RAII for clean resource management
- Modular architecture for DSP extensions
- Cross-platform ready (Linux/Windows)

## 🛠️ Technical Highlights
- **C++17 STL** (`std::vector`, `std::unique_ptr`, `std::thread`)
- **Real-time buffer management**
- **Low-level file/stream handling**
- **Threading model for producer–consumer audio flow**
- **Clean separation of I/O, decoding, and playback**

## 📦 Build Instructions

mkdir build && cd build
cmake ..
make
./audio_player <path-to-audio-file>


## 🧪 What This Demonstrates
- Understanding of **real-time system constraints**
- Ability to design **stable, low-latency pipelines**
- Experience working with **memory management, threads, and synchronization**
- Competency in **cross-platform C++ development**

## 📈 Future Improvements
- Add a simple DSP pipeline (EQ, filters)
- Add unit tests for buffer edge cases
- Integrate PortAudio or RtAudio for hardware abstraction
