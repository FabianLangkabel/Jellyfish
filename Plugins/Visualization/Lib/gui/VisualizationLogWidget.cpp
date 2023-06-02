#include "VisualizationLogWidget.h"

VisualizationLogWidget::VisualizationLogWidget(QWidget *parent) : QWidget(parent)
{
	//Bottom Widget Log
	QTabWidget* tabWidget = new QTabWidget;
	MainLog = new QPlainTextEdit;
	MainLog->setReadOnly(true);
	tabWidget->addTab(MainLog, tr("Main"));

    QVBoxLayout* Layout = new QVBoxLayout;
	Layout->addWidget(tabWidget);
	setLayout(Layout);
}

VisualizationLogWidget::~VisualizationLogWidget()
{

}

void VisualizationLogWidget::AddToLog(int LogType, std::string LogMessage)
{
	MainLogMessages.push_back(LogMessage);
    MainLog->clear();
    for(const auto& l: MainLogMessages) {
        MainLog->appendPlainText(QString::fromStdString(l));
    }
}