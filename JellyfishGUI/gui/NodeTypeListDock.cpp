#include "NodeTypeListDock.h"


NodeTypeListDock::NodeTypeListDock(std::shared_ptr<CoreSystem> Core, QWidget* parent)
{
    _core = Core;
    this->setWindowTitle("Nodes");
    QWidget* Wid = new QWidget(this);
    QVBoxLayout *lay = new QVBoxLayout;
    Wid->setLayout(lay);
    SearchBar = new QLineEdit(this);
    NodeList = new NodeQTreeView(this);
    lay->addWidget(SearchBar);
    lay->addWidget(NodeList);
    this->setWidget(Wid);
    connect(SearchBar, &QLineEdit::textChanged, this, &NodeTypeListDock::SetSearch);

    setAcceptDrops(true);

    UpdateNodeList();
}

void NodeTypeListDock::UpdateNodeList()
{
    TreeIdxToNodeString.clear();
    std::map<std::string, std::map<std::string, json>>* list = _core->getNodeTypeList();
    auto model = new QStandardItemModel();
    auto proxyModel = new NodeQTreeViewProxyModel;
    proxyModel->setSourceModel(model);
    QStandardItem *rootNode = model->invisibleRootItem();

    for (auto const& [key, val] : *list)
    {
        QStandardItem *PluginItem = new QStandardItem(QString::fromStdString(key));
        model->appendRow(PluginItem);
        for (auto const& [inner_key, inner_val] : val)
        {
            QStandardItem *Node = new QStandardItem(QString::fromStdString(inner_val["NodeName"]));
            Node->setToolTip(QString::fromStdString(inner_val["NodeDescription"]));
            Node->setDragEnabled(true);
            QStringList info = {QString::fromStdString(key), QString::fromStdString(inner_key)};
            Node->setData(info);

            bool CategoryExist = false;
            for(int r = 0; r < model->rowCount(PluginItem->index()); r++)
            {
                QModelIndex CategoryIdx = model->index(r, 0, PluginItem->index());
                if(model->data(CategoryIdx).toString().toStdString() == inner_val["NodeCategory"])
                {
                    CategoryExist = true;
                    model->itemFromIndex(CategoryIdx)->appendRow(Node);
                    break;
                }
            }
            if(!CategoryExist)
            {
                QStandardItem *CategoryItem = new QStandardItem(QString::fromStdString(inner_val["NodeCategory"]));
                PluginItem->appendRow(CategoryItem);
                CategoryItem->appendRow(Node);
            }
        }
    }
    NodeList->setModel(proxyModel);
    NodeList->setHeaderHidden(true);
    NodeList->expandAll();
    NodeList->_model = proxyModel;
}

void NodeTypeListDock::SetSearch()
{
    NodeList->_model->SetSearch(SearchBar->text());
}

bool NodeQTreeViewProxyModel::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const{
    QModelIndex idx = sourceModel()->index(source_row, 0, source_parent);

    QString name = sourceModel()->data(idx).toString();
    if(sourceModel()->hasChildren(idx)){  return true;  }

    const bool match = (name.contains(Search, Qt::CaseInsensitive));
    return match;
}

QVariant NodeQTreeViewProxyModel::headerData(int section, Qt::Orientation orientation, int role) const 
{
    return sourceModel()->headerData(section, orientation, role);
}

void NodeQTreeView::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        auto model = dynamic_cast<QStandardItemModel*>(_model->sourceModel());
        auto idx = _model->mapToSource(this->indexAt(event->pos()));

        if(idx.row() > -1)
        {
            auto item = model->itemFromIndex(idx);
            this->setCurrentIndex(this->indexAt(event->pos()));

            if(item->data().isValid())
            {
                
                QDrag *drag = new QDrag(this);
                QMimeData *mimeData = new QMimeData;

                mimeData->setText(item->data().toStringList().join("~"));
                drag->setMimeData(mimeData);
                Qt::DropAction dropAction = drag->exec();
                
            }
        }
    }
}

