#include "maintoolbar.h"


MainToolBar::MainToolBar(MainTabWidget* TabWidget, QWidget *parent) : QToolBar(parent)
{
    _TabWidget = TabWidget;

    newAct = new QAction(QIcon(":/icons/paper_1_add.svg"), tr("&New"), this);
    newAct->setShortcuts(QKeySequence::New);
    newAct->setStatusTip(tr("Start a new Project"));
    connect(newAct, &QAction::triggered, _TabWidget, &MainTabWidget::newProject);
    openAct = new QAction(QIcon(":/icons/folder_1.svg"), tr("&Open..."), this);
    openAct->setShortcuts(QKeySequence::Open);
    openAct->setStatusTip(tr("Open an existing file"));
    connect(openAct, &QAction::triggered, _TabWidget, &MainTabWidget::openProject);
    saveAct = new QAction(QIcon(":/icons/save_1.svg"), tr("&Save"), this);
    saveAct->setShortcuts(QKeySequence::Save);
    saveAct->setStatusTip(tr("Save the document to disk"));
    connect(saveAct, &QAction::triggered, _TabWidget, &MainTabWidget::saveProject);
    aboutAct = new QAction(QIcon(":/icons/info_cr.svg"), tr("&About"), this);
    aboutAct->setStatusTip(tr("Show the application's About box"));
    //connect(aboutAct, &QAction::triggered, this, &MainWindow::about);
    /*
    auto fileMenu = addMenu(tr("&File"));
    fileMenu->addAction(newAct);
    fileMenu->addAction(openAct);
    fileMenu->addAction(saveAct);
    auto helpMenu = addMenu(tr("&Help"));
    helpMenu->addAction(aboutAct);
    */

    //ToolBar
    addAction(newAct);
    addAction(openAct);
    addAction(saveAct);
    addSeparator();
    addAction(aboutAct);
}

MainToolBar::~MainToolBar()
{

}