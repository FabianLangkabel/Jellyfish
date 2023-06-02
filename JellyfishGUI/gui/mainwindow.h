#pragma once

#include <QMainWindow>
#include <QtWidgets>
#include <QApplication>

#include "maintabwidget.h"
#include "maintoolbar.h"
#include "NodeTypeListDock.h"

#include "../../JellyfishCore/Core.hpp"


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    MainTabWidget *TabSystem;
    MainToolBar *ToolBar;
    NodeTypeListDock *NodeTypeList;
};
