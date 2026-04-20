cmake_minimum_required(VERSION 3.20)

project(CachyOSTracker VERSION 0.1.0 LANGUAGES CXX)

#Global C++ standards
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory(third_party/third_party/ftxui)

add_executable(${PROJECT_NAME}
    src/main.cpp
    src/providers/psiProvider.cpp
    src/providers/schedulerProvider.cpp
    src/providers/zramProvider.cpp
)

target_compile_options(${PROJECT_NAME} PRIVATE
    -O3
    -march=native
    -Wall -Wextra -Wpedantic    
)

target_include_directories(${PROJECT_NAME} PRIVATE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_link_libraries(${PROJECT_NAME}
    ftxui::screen
    ftxui::dom
    ftxui::component
)