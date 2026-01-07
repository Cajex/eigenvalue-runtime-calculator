cmake_minimum_required(VERSION 4.1)
project(eigenvalue_runtime_calculator VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_RELEASE ON)

find_package(nlohmann_json CONFIG REQUIRED)

find_package(PkgConfig REQUIRED)
pkg_check_modules(LAPACKE REQUIRED lapacke)
pkg_check_modules(OPENBLAS QUIET openblas)


add_executable(comp main.cpp)
target_include_directories(comp PRIVATE
        ${LAPACKE_INCLUDE_DIRS}
)
target_compile_options(comp PRIVATE $<$<CONFIG:Release>:-O3;-DNDEBUG;-march=native>)

target_link_options(comp PRIVATE $<$<CONFIG:Release>:-flto>)

if(OPENBLAS_FOUND)
    target_include_directories(comp PRIVATE ${OPENBLAS_INCLUDE_DIRS})
    target_link_libraries(comp PRIVATE ${OPENBLAS_LINK_LIBRARIES})
endif()

target_link_libraries(comp PRIVATE
        ${LAPACKE_LINK_LIBRARIES} nlohmann_json::nlohmann_json
)


