cmake_minimum_required(VERSION 3.9)
project(CHW)

set(CMAKE_CXX_STANDARD 11)

add_executable(
        Huffman
        Huffman.h
        ShannonFano.h
        Encoder.h
        Decoder.h
        Node.h
        LZ77.h
        LZ77Decoder.h
        main.cpp
        Huffman.cpp
        ShannonFano.cpp
        Encoder.cpp
        Decoder.cpp
        Node.cpp
        LZ77.cpp
        LZ77Decoder.cpp)