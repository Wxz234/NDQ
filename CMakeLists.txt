cmake_minimum_required(VERSION 3.28)
project(ndq)

add_subdirectory("thirdparty/DirectX-Headers")

add_subdirectory("source")
add_subdirectory("examples/01-window")
add_subdirectory("examples/02-triangle")