#pragma once

#include <QMainWindow>
#include <QtWidgets>
#include <QApplication>
#include <map>

#include "projecttab.h"
#include "../../JellyfishCore/Core.hpp"
#include "../../JellyfishCore/Project.h"

class MainTabWidget : public QTabWidget
{
    Q_OBJECT

public:
    explicit MainTabWidget(std::shared_ptr<CoreSystem> Core, QWidget *parent = 0);
    ~MainTabWidget();

    void newProject();
    void saveProject();
    void openProject();
    void closeProject(int Tab);

private:
    std::shared_ptr<CoreSystem> _Core;
    std::unordered_map<QString, ProjectTab*> _OpenProjects;
};
