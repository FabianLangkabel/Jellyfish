cmake_minimum_required(VERSION 2.6)

project(JellyfishCore CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(JellyfishCore_VERSION_MAJOR 2)
set(JellyfishCore_VERSION_MINOR 0)

set(CMAKE_PREFIX_PATH "/usr/local/Qt6/")

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
"Core.cpp"
"Project.cpp"
"Utils.cpp"
)

add_library(JellyfishCore ${SOURCE_FILES})
target_compile_options(JellyfishCore PUBLIC ${CXXFLAGS})
target_link_libraries(JellyfishCore ${LDLIBS})
target_link_libraries(JellyfishCore zip)

target_link_libraries(JellyfishCore
    Qt::Core
    Qt::Widgets
    Qt::Gui
    Qt::OpenGL
    Qt::Charts
)