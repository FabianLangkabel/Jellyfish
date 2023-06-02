#include "VisualizationTDPlotObjects.h"

//************************* DensityDifference *************************
VisualizationTDPlotDensityDifferenceWidget::VisualizationTDPlotDensityDifferenceWidget(std::shared_ptr <QC::CIResults> ciresults, QWidget *parent) : QWidget(parent)
{
    QVBoxLayout* Layout = new QVBoxLayout;
    ReferenceCIStateList = new QListWidget();
    Eigen::VectorXd Energys = (*(ciresults.get())).ci_values;
    for(int i = 0; i < Energys.size(); i++)
    {
        std::string ItemString = "CI-State " + std::to_string(i) + " (Energy: " + std::to_string(Energys[i]) + ")";
        ReferenceCIStateList->addItem(QString::fromStdString(ItemString));
    }
    ReferenceCIStateList->setCurrentRow(0);
    connect(ReferenceCIStateList, &QListWidget::currentRowChanged, this, &VisualizationTDPlotDensityDifferenceWidget::ParameterChanged);
    Layout->addWidget(new QLabel("Select Reference CI-State"));
    Layout->addWidget(ReferenceCIStateList);
    setLayout(Layout);
}

void VisualizationTDPlotDensityDifferenceWidget::ParameterChanged()
{
    ReferenceCIState = ReferenceCIStateList->currentRow();
}

//************************* NTODensity *************************
VisualizationTDPlotNTODensityWidget::VisualizationTDPlotNTODensityWidget(QWidget *parent) : QWidget(parent)
{
    QVBoxLayout* Layout = new QVBoxLayout;
    DensitySelector = new QComboBox;
    DensitySelector->addItem("Hole-Density");
    DensitySelector->addItem("Particle-Density");
    connect(DensitySelector, &QComboBox::currentTextChanged, this, &VisualizationTDPlotNTODensityWidget::ParameterChanged);
    Layout->addWidget(new QLabel("Select Density"));
    Layout->addWidget(DensitySelector);
    setLayout(Layout);
}

void VisualizationTDPlotNTODensityWidget::ParameterChanged()
{
    SelectedDensity = DensitySelector->currentIndex();
}