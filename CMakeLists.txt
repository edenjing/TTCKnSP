cmake_minimum_required(VERSION 3.22)
project(TTCK_Final)

set(CMAKE_CXX_STANDARD 14)

find_package(Boost 1.79.0 REQUIRED COMPONENTS filesystem)
include_directories(${Boost_INCLUDE_DIRS})

add_executable(TTCK_Final main.cpp TTCknSP.h tools.cpp tools.h linearPiecewiseFunction.cpp linearPiecewiseFunction.h graph.cpp graph.h heap.h RegionDecomposition.cpp)
