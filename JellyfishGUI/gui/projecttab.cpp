#include "projecttab.h"

using QtNodes::FlowView;
using QtNodes::FlowScene;

ProjectTab::ProjectTab(std::shared_ptr<CoreSystem> Core, QWidget *parent) : QWidget(parent)
{
    Nodeeditor = new FlowScene(Core);
    Log = new QPlainTextEdit(this);
    Inspector = new NodeInspector(this);

    QHBoxLayout* lay = new QHBoxLayout(this);
    QSplitter* splitter1 = new QSplitter(Qt::Vertical);
    splitter1->addWidget(new FlowView(Nodeeditor));
    splitter1->addWidget(Log);
    QSplitter* splitter2 = new QSplitter(Qt::Horizontal);
    splitter2->addWidget(splitter1);
    splitter2->addWidget(Inspector);
    lay->addWidget(splitter2);
    setLayout(lay);

    Log->resize(Log->width(), this->height() / 5);
    Log->setReadOnly(true);
    Inspector->resize(this->width() / 4, Inspector->height());

    connect(Nodeeditor, &FlowScene::selectionChanged, this, &ProjectTab::NodeSelectionChanged);
    NodeSelectionChanged();
}

ProjectTab::ProjectTab(std::shared_ptr<CoreSystem> Core, std::string ProjectFile, QWidget *parent) : QWidget(parent)
{
    Nodeeditor = new FlowScene(Core, ProjectFile);
    Log = new QPlainTextEdit(this);
    Inspector = new NodeInspector(this);

    QHBoxLayout* lay = new QHBoxLayout(this);
    QSplitter* splitter1 = new QSplitter(Qt::Vertical);
    splitter1->addWidget(new FlowView(Nodeeditor));
    splitter1->addWidget(Log);
    QSplitter* splitter2 = new QSplitter(Qt::Horizontal);
    splitter2->addWidget(splitter1);
    splitter2->addWidget(Inspector);
    lay->addWidget(splitter2);
    setLayout(lay);

    Log->resize(Log->width(), this->height() / 5);
    Log->setReadOnly(true);
    UpdateLog();
    Inspector->resize(this->width() / 4, Inspector->height());

    connect(Nodeeditor, &FlowScene::selectionChanged, this, &ProjectTab::NodeSelectionChanged);
    NodeSelectionChanged();
}

ProjectTab::~ProjectTab()
{
    delete Nodeeditor;
}

QString ProjectTab::saveProject()
{
    QString fileName = QFileDialog::getSaveFileName(nullptr, tr("Save Jellyfish File"), QDir::homePath(), tr("Jellyfish File (*.jlf)"));
    if (!fileName.isEmpty())
    {
        get_Project()->save(fileName.toStdString(), false);
    }
    UpdateLog();
    return QString::fromStdString(get_Project()->get_ProjectName());
}

void ProjectTab::closeProject()
{
    //std::cout << "Close Project " << Nodeeditor->get_Project()->get_ProjectName() << std::endl;
}

void ProjectTab::UpdateLog()
{
    Log->clear();
    for(const auto& l: get_Project()->get_Log()) {
        Log->appendPlainText(QString::fromStdString(l));
    }
}

void ProjectTab::NodeSelectionChanged()
{
    if (Inspector->layout() != NULL)
    {
        QLayoutItem* item;
        while ((item = Inspector->layout()->takeAt(0)) != NULL)
        {
            delete item->widget();
            delete item;
        }
        delete Inspector->layout();
    }

    if (Nodeeditor->selectedNodes().size() == 1)
    {
        UpdateNodeInspectorForSingleNode();
    }
    else
    {
        QVBoxLayout* layout = new QVBoxLayout;
        layout->setAlignment(Qt::AlignTop);
        layout->addWidget(new QLabel("Select a node to change its parameters"));
        Inspector->setLayout(layout);
    }
}

void ProjectTab::UpdateNodeInspectorForSingleNode()
{
    std::shared_ptr<Node> n = Nodeeditor->selectedNodes()[0]->_node;
    QVBoxLayout* mainlayout = new QVBoxLayout;
    mainlayout->setAlignment(Qt::AlignTop);
    QWidget* HeaderBox = new QWidget(this);
    {
        QVBoxLayout* HeaderBoxLayout = new QVBoxLayout;
        HeaderBoxLayout->addWidget(new QLabel(QString::fromStdString("NodeID: " + n->id)));

        QPushButton* Calculate_Single_Node = new QPushButton("Compute single Node");
        connect(Calculate_Single_Node, &QPushButton::clicked, this, &ProjectTab::compute_node);
        HeaderBoxLayout->addWidget(Calculate_Single_Node);
        QPushButton* Calculate_connected_Node = new QPushButton("Compute these and all dependent Nodes");
        connect(Calculate_connected_Node, &QPushButton::clicked, this, &ProjectTab::compute_node_and_dependence);
        HeaderBoxLayout->addWidget(Calculate_connected_Node);

        auto line = new QFrame;
        line->setFrameShape(QFrame::HLine);
        line->setFrameShadow(QFrame::Sunken);
        HeaderBoxLayout->addWidget(line);

        HeaderBox->setLayout(HeaderBoxLayout);
    }

    mainlayout->addWidget(HeaderBox);
    QWidget* NodeBox = new QWidget(this);
    n->NodeInspector(NodeBox);
    mainlayout->addWidget(NodeBox);
    Inspector->setLayout(mainlayout);
}

void ProjectTab::compute_node()
{
    get_Project()->compute_node(Nodeeditor->selectedNodes()[0]->_node->id, false);
    UpdateLog();
}

void ProjectTab::compute_node_and_dependence()
{
    get_Project()->compute_node_and_dependence(Nodeeditor->selectedNodes()[0]->_node->id, false);
    UpdateLog();
}