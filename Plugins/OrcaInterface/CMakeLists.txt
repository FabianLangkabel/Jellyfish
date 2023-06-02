cmake_minimum_required(VERSION 2.6)

project(JellyfishPluginOrcaInterface CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_PREFIX_PATH "/scratch/mmz/Libraries/QT/5.15.2/gcc_64/")

include_directories(
  "${CMAKE_SOURCE_DIR}/Libraries/eigen"
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

if(NDEBUG)
  add_definitions(-DNDEBUG)
endif()

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