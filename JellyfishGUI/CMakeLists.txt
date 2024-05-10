cmake_minimum_required(VERSION 3.8)
project(JellyfishGUI CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_PREFIX_PATH "/usr/local/Qt6/")

find_package(Qt6
  COMPONENTS
   Core
   Widgets
   Gui
   OpenGL
   Charts
)


qt_add_resources(RESOURCES ./Nodeeditor/resources/resources.qrc)
qt_add_resources(RESOURCES ./icons.qrc)

set(CMAKE_AUTOMOC ON)

set(APP_EXE "JellyfishGUI")
set(CMAKE_CXX_FLAGS "-Wall")

set(SOURCE_FILES
#"icons.qrc"
"gui/mainwindow.cpp"
"gui/maintabwidget.cpp"
"gui/projecttab.cpp"
"gui/nodeinspector.cpp"
"gui/maintoolbar.cpp"
"gui/NodeTypeListDock.cpp"
"main.cpp"

#"Nodeeditor/resources/resources.qrc"
"Nodeeditor/src/GUIConnection.cpp"
"Nodeeditor/src/ConnectionBlurEffect.cpp"
"Nodeeditor/src/ConnectionGeometry.cpp"
"Nodeeditor/src/ConnectionGraphicsObject.cpp"
"Nodeeditor/src/ConnectionPainter.cpp"
"Nodeeditor/src/ConnectionState.cpp"
"Nodeeditor/src/ConnectionStyle.cpp"
"Nodeeditor/src/FlowScene.cpp"
"Nodeeditor/src/FlowView.cpp"
"Nodeeditor/src/FlowViewStyle.cpp"
"Nodeeditor/src/GUINode.cpp"
"Nodeeditor/src/NodeConnectionInteraction.cpp"
"Nodeeditor/src/NodeGraphicsObject.cpp"
"Nodeeditor/src/NodePainter.cpp"
"Nodeeditor/src/NodeStyle.cpp"
"Nodeeditor/src/Properties.cpp"
"Nodeeditor/src/StyleCollection.cpp"
)

add_executable (${APP_EXE} ${SOURCE_FILES} ${RESOURCES})
target_link_libraries(${APP_EXE}
    Qt::Core
    Qt::Widgets
    Qt::Gui
    Qt::OpenGL
    Qt::Charts
)

target_link_libraries(${APP_EXE} JellyfishCore)
target_link_libraries(${APP_EXE} zip)