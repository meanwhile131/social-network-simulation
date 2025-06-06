cmake_minimum_required(VERSION 3.13)
set(CMAKE_C_COMPILER clang)
set(CMAKE_CXX_COMPILER clang++)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

project(Simulation)
add_executable(main src/main.cpp)

find_package(PkgConfig REQUIRED)
pkg_search_module(GVC REQUIRED libgvc)
pkg_search_module(CGRAPH REQUIRED libcgraph)
target_include_directories(main PRIVATE ${GVC_INCLUDE_DIRS} ${CGRAPH_INCLUDE_DIRS})
target_link_libraries(main PRIVATE ${GVC_LIBRARIES} ${CGRAPH_LIBRARIES})