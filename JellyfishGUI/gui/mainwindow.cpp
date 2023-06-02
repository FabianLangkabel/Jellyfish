#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    std::shared_ptr<CoreSystem> Core = std::make_shared<CoreSystem>();
    Core->LoadPlugins();

    TabSystem = new MainTabWidget(Core, this);
    setCentralWidget(TabSystem);

    ToolBar = new MainToolBar(TabSystem, this);
    this->addToolBar(ToolBar);

    NodeTypeList = new NodeTypeListDock(Core,this);
    NodeTypeList->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    NodeTypeList->setMinimumWidth(250);
    addDockWidget(Qt::LeftDockWidgetArea, NodeTypeList);
}

MainWindow::~MainWindow()
{

}
