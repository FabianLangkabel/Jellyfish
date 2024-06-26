cmake_minimum_required(VERSION 2.6)

project(JellyfishPluginVisualization CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_PREFIX_PATH "/usr/local/Qt6/")
set(VTK_DIR "${CMAKE_SOURCE_DIR}/Libraries/VTK/lib64/cmake/vtk-9.0")

find_package(VTK REQUIRED)

include_directories(
  "/usr/local/eigen"
  "/usr/local/libcint/include"
  "/usr/local/muparser/include"
  "/usr/local/VTK/include/vtk-9.3"
)

link_directories(
  "/usr/local/libcint/lib"
  "/usr/local/muparser/lib"
  "/usr/local/VTK/lib"
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

"Lib/gui/VisualizationGridWidget.cpp"
"Lib/gui/VisualizationLogWidget.cpp"
"Lib/gui/VisualizationRenderParameterWidget.cpp"
"Lib/gui/VisualizationRenderWidget.cpp"
"Lib/gui/VisualizationExportPicture.cpp"
"Lib/gui/ValueCuboid.cpp"

"Nodes/PlotCINewNode/PlotCINewNode.cpp"
"Nodes/PlotTDCINewNode/PlotTDCINewNode.cpp"

"Nodes/PlotCINewNode/plot_ci_new_window.cpp"
"Nodes/PlotCINewNode/VisualizationTIDCalculate.cpp"
"Nodes/PlotCINewNode/VisualizationTIDPlotObjectDocker.cpp"
"Nodes/PlotCINewNode/VisualizationTIDPlotObjects.cpp"

"Nodes/PlotTDCINewNode/plot_tdci_new_window.cpp"
"Nodes/PlotTDCINewNode/VisualizationTDCalculate.cpp"
"Nodes/PlotTDCINewNode/VisualizationTDPlotObjectDocker.cpp"
"Nodes/PlotTDCINewNode/VisualizationTDPlotObjects.cpp"
"Nodes/PlotTDCINewNode/VisualizationExportVideo.cpp"
)

add_library(Visualization SHARED ${SOURCE_FILES})
target_compile_options(Visualization PUBLIC ${CXXFLAGS})
target_link_libraries(Visualization ${LDLIBS})

target_link_libraries(Visualization
    Qt::Core
    Qt::Widgets
    Qt::Gui
    Qt::OpenGL
    Qt::Charts
)

target_link_libraries(Visualization openblas)
target_link_libraries(Visualization cint)
target_link_libraries(Visualization zip)

target_link_libraries(Visualization ${VTK_LIBRARIES})
vtk_module_autoinit(
  TARGETS Visualization
  MODULES ${VTK_LIBRARIES}
)