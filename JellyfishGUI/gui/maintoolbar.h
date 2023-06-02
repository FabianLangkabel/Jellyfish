#pragma once

#include <QMainWindow>
#include <QtWidgets>
#include <QApplication>

#include "maintabwidget.h"


class MainToolBar : public QToolBar
{
    Q_OBJECT

public:
    explicit MainToolBar(MainTabWidget* TabWidget, QWidget *parent = 0);
    ~MainToolBar();

private:
    QAction *newAct;
    QAction *openAct;
    QAction *saveAct;
	QAction *aboutAct;

    MainTabWidget* _TabWidget;
};
