cmake_minimum_required(VERSION 2.6)

project(JellyfishPluginOrcaInterface CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_PREFIX_PATH "/usr/local/Qt6/")

include_directories(
  "/usr/local/eigen"
)

link_directories(
)

find_package(Qt6
  COMPONENTS
   Core
   Widgets
   Gui
   OpenGL
   Charts
)

set(CXXFLAGS -O2 -s -Wall -Wextra)
set(LDLIBS -s dl pthread stdc++fs)

set(SOURCE_FILES
"plugin.cxx"
"save_load.cpp"

"Lib/src/read_orca_file.cpp"

"Nodes/ReadOrcaNode/ReadOrcaNode.cpp"
)

add_library(OrcaInterface SHARED ${SOURCE_FILES})
target_compile_options(OrcaInterface PUBLIC ${CXXFLAGS})
target_link_libraries(OrcaInterface ${LDLIBS})

target_link_libraries(OrcaInterface
    Qt::Core
    Qt::Widgets
    Qt::Gui
    Qt::OpenGL
    Qt::Charts
)

target_link_libraries(OrcaInterface zip)