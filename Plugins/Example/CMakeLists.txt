cmake_minimum_required(VERSION 2.6)

project(JellyfishPlugin CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_PREFIX_PATH "/scratch/mmz/Libraries/QT/5.15.2/gcc_64/")
find_package(Qt6
  COMPONENTS
   Core
   Widgets
   Gui
   OpenGL
)

if (NOT Qt6_FOUND)
  find_package(Qt5 5.13
    COMPONENTS
     Core
     Widgets
     Gui
     OpenGL
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
"testnode.cpp"
"testnode2.cpp"
)

add_library(test SHARED ${SOURCE_FILES})
target_compile_options(test PUBLIC ${CXXFLAGS})
target_link_libraries(test ${LDLIBS})

target_link_libraries(test
    Qt::Core
    Qt::Widgets
    Qt::Gui
    Qt::OpenGL
)