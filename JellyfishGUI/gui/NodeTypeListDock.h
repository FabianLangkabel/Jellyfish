#pragma once

#include <QtWidgets>
#include <map>

#include "../../JellyfishCore/Core.hpp"

class NodeQTreeViewProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    NodeQTreeViewProxyModel(QObject* parent = 0){};
    bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    void SetSearch(QString search){Search = search; invalidateFilter(); }

private:
    QString Search = "";
};

class NodeQTreeView : public QTreeView
{
    Q_OBJECT
public:
    NodeQTreeView(QWidget* parent = nullptr){}
    NodeQTreeViewProxyModel* _model;

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override {}
};


class NodeTypeListDock : public QDockWidget
{
    Q_OBJECT
public:
    NodeTypeListDock(std::shared_ptr<CoreSystem> Core, QWidget* parent = nullptr);
    std::shared_ptr<CoreSystem> _core;
    QLineEdit *SearchBar;
    NodeQTreeView *NodeList;
    void UpdateNodeList();
    void SetSearch();

private:
    std::map<QModelIndex, std::string> TreeIdxToNodeString;
};