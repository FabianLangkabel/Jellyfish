cmake_minimum_required(VERSION 2.6)

project(JellyfishCore CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(JellyfishCore_VERSION_MAJOR 2)
set(JellyfishCore_VERSION_MINOR 0)

set(CMAKE_PREFIX_PATH "/scratch/mmz/Libraries/QT/5.15.2/gcc_64/")
include_directories(
  "${CMAKE_SOURCE_DIR}/Libraries/libzip/include" 
)

link_directories(
  "${CMAKE_SOURCE_DIR}/Libraries/libzip/lib64"
)

find_package(Qt6
  COMPONENTS
   Core
   Widgets
   Gui
   OpenGL
   Charts
)

if (NOT Qt6_FOUND)
  find_package(Qt5 5.13
    COMPONENTS
     Core
     Widgets
     Gui
     OpenGL
     Charts
  )
endif()

if (NOT (Qt6_FOUND OR Qt5_FOUND))
  message(FATAL_ERRROR "Qt libraries were not found.")
endif()

if(WIN32)
  set(CXXFLAGS -O2 -s -Wall -Wextra)
  set(LDLIBS -s pthread stdc++fs ole32 oleaut32 psapi advapi32)
else()
  set(CXXFLAGS -O2 -s -Wall -Wextra)
  set(LDLIBS -s dl pthread stdc++fs)
endif()

if(MAX_PLUGINS_ARGS)
  add_definitions(-DMAX_PLUGINS_ARGS=${MAX_PLUGINS_ARGS})
endif()

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