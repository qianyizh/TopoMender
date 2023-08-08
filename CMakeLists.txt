set(ProjectName "TopoMender")
project(${ProjectName})
cmake_minimum_required(VERSION 2.9)
set(CMAKE_DEBUG_POSTFIX "d") 

set(NAME_TOPOMENDER "TopoMender")
set(NAME_MENDIT "TopoMender_MendIT")

add_subdirectory(TopoMender)
add_subdirectory(TopoMender_MendIT)
