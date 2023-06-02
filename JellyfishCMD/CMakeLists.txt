cmake_minimum_required(VERSION 3.8)
project(JellyfishCMD CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(APP_EXE "JellyfishCMD")

include_directories(
  "${CMAKE_SOURCE_DIR}/Libraries/libzip/include" 
)

link_directories(
  "${CMAKE_SOURCE_DIR}/Libraries/libzip/lib64"
)

set(SOURCE_FILES
"main.cpp"
"interface.cpp"
)

add_executable (${APP_EXE} ${SOURCE_FILES} ${RESOURCES})

target_link_libraries(${APP_EXE} JellyfishCore)
target_link_libraries(${APP_EXE} zip)