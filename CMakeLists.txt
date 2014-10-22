cmake_minimum_required(VERSION 2.6)
PROJECT (simpleJPEG)
ADD_EXECUTABLE(converter huffman.c main.c process.c table.c)
TARGET_LINK_LIBRARIES(converter m)
