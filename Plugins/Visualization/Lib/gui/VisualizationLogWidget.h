#pragma once

#include <QtWidgets>


class VisualizationLogWidget : public QWidget
{
public:
    explicit VisualizationLogWidget(QWidget *parent = 0);
    ~VisualizationLogWidget();

public:
    void AddToLog(int LogType, std::string LogMessage);

private:
    QPlainTextEdit* MainLog;
    std::vector<std::string> MainLogMessages;
};
