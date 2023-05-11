cmake_minimum_required(VERSION 3.22)

project(MyFractalCompression)

find_package(ZLIB REQUIRED)

set(CMAKE_CXX_STANDARD 20)
file(GLOB SOURCES "src/*")
add_executable(MyFractalCompression ${SOURCES} Compress/Decoder.cpp Compress/Decoder.h Compress/Encoder.cpp Compress/Encoder.h Compress/IFSTransform.cpp Compress/IFSTransform.h)
target_link_libraries(MyFractalCompression PRIVATE ZLIB::ZLIB)

