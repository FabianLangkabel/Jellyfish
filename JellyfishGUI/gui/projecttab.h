#pragma once

#include <QMainWindow>
#include <QtWidgets>
#include <QApplication>

#include "nodeinspector.h"

#include "../Nodeeditor/src/FlowScene.hpp"
#include "../Nodeeditor/src/GUINode.hpp"
#include "../Nodeeditor/src/FlowView.hpp"

#include "../../JellyfishCore/Core.hpp"
#include "../../JellyfishCore/Project.h"

using QtNodes::FlowScene;
using QtNodes::FlowView;

class ProjectTab : public QWidget
{
    Q_OBJECT

public:
    explicit ProjectTab(std::shared_ptr<CoreSystem> Core, QWidget *parent = 0);
    explicit ProjectTab(std::shared_ptr<CoreSystem> Core, std::string ProjectFile, QWidget *parent = 0);
    ~ProjectTab();
    
public:
    std::shared_ptr<Project> get_Project() { return Nodeeditor->get_Project(); } 
    QString saveProject();
    void LoadProject();
    void closeProject();
    void UpdateLog();
    void NodeSelectionChanged();
    void UpdateNodeInspectorForSingleNode();

    void compute_node();
    void compute_node_and_dependence();

private:
    FlowScene* Nodeeditor;
    QPlainTextEdit* Log;
    NodeInspector* Inspector;
};
