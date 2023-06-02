#include "maintabwidget.h"


MainTabWidget::MainTabWidget(std::shared_ptr<CoreSystem> Core, QWidget *parent) : QTabWidget(parent)
{
    _Core = Core;
    setTabsClosable(true);
    newProject();
    connect(this, &MainTabWidget::tabCloseRequested, this, &MainTabWidget::closeProject);
}

MainTabWidget::~MainTabWidget()
{

}

void MainTabWidget::newProject()
{
    ProjectTab* proj = new ProjectTab(_Core, this);
    addTab(proj, "New Project");
    QString ProjectID = QString::number(proj->get_Project()->get_ProjectNumber());
    proj->setObjectName(ProjectID);
    _OpenProjects[ProjectID] = proj;
    setCurrentWidget(proj);
}

void MainTabWidget::saveProject()
{
    ProjectTab* proj = _OpenProjects[currentWidget()->objectName()];
    QString newTabName = proj->saveProject();
    setTabText(currentIndex(), newTabName);
}

void MainTabWidget::openProject()
{
    QString fileName = QFileDialog::getOpenFileName(nullptr, tr("Open Jellyfish File"), QDir::homePath(), tr("Jellyfish File (*.jlf)"));
    if (!fileName.isEmpty())
    {
        ProjectTab* proj = new ProjectTab(_Core, fileName.toStdString(), this);
        QString ProjectID = QString::number(proj->get_Project()->get_ProjectNumber());
        QString ProjectName = QString::fromStdString(proj->get_Project()->get_ProjectName());
        proj->setObjectName(ProjectID);
        _OpenProjects[ProjectID] = proj;
        addTab(proj, ProjectName);
        setCurrentWidget(proj);
    }
}

void MainTabWidget::closeProject(int Tab)
{
    ProjectTab* proj = _OpenProjects[widget(Tab)->objectName()];
    proj->closeProject();
    _OpenProjects.erase(widget(Tab)->objectName());
    proj->close();
    delete proj;
}