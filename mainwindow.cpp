#include "mainwindow.h"
#include "filemanager.h"
#include "ui_mainwindow.h"

void printArray(QList<RegistryEntry> list){
    for(const auto pair : list){
        qDebug() << "p1" << pair.first << "p2"  << pair.second;
    }
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    updateLocalLists();

    updateGlobalLists();
    connect(ui->syncWithReg, &QPushButton::clicked, this, &MainWindow::updateGlobalLists);

    connect(ui->impLayers->model(), &QAbstractItemModel::rowsMoved, this,
            [this](const QModelIndex&, int start, int end, const QModelIndex&, int dest) {
                if (dest >= m_implicitLayers.size()) {
                    dest = m_implicitLayers.size() - 1;
                }
                qDebug() << "BEFORE";
                printArray(m_implicitLayers);
                qDebug() << "AFTER";
                m_implicitLayers.move(start, dest);
                printArray(m_implicitLayers);
                regManager.changeLayersSystemOrder(RegistryManager::LayerType::Implicit, m_implicitLayers);
            });

    connect(ui->expLayers->model(), &QAbstractItemModel::rowsMoved, this,
            [this](const QModelIndex&, int start, int end, const QModelIndex&, int dest) {
                if (dest >= m_explicitLayers.size()) {
                    dest = m_explicitLayers.size() - 1;
                }
                m_explicitLayers.move(start, dest);
                regManager.changeLayersSystemOrder(RegistryManager::LayerType::Explicit, m_explicitLayers);
            });
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::updateLocalLists(){
    m_implicitLayers = regManager.getImplicitKeys();
    m_explicitLayers = regManager.getExplicitKeys();
}

void MainWindow::updateGlobalLists(){
    fillList(ui->impLayers, m_implicitLayers, true, regManager.getLayerKey(RegistryManager::LayerType::Implicit));
    fillList(ui->expLayers, m_explicitLayers, false, regManager.getLayerKey(RegistryManager::LayerType::Explicit));
}

void MainWindow::fillList(QListWidget *list, const QList<RegistryEntry> &registryMap, bool checksNeeded, const QString &branchPath){
    list->clear();
    list->blockSignals(true);
    for (const auto &pair : registryMap){
        const QString &rkey = pair.first;
        QString layerName = FileManager::getJsonElementByName(rkey, "name");
        QListWidgetItem *item = new QListWidgetItem(layerName, list);
        item->setToolTip(rkey);
        item->setData(Qt::UserRole, branchPath);
        if(checksNeeded){
            item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
            item->setCheckState(pair.second == 0 ? Qt::Checked : Qt::Unchecked);
        }
    }
    list->setDragDropMode(QAbstractItemView::InternalMove);
    list->blockSignals(false);
}

void MainWindow::on_impLayers_itemChanged(QListWidgetItem *item) {
    regManager.setRegistryValueData(item->data(Qt::UserRole).toString(), item->toolTip(), item->checkState() ==  Qt::Checked ? 0 : 1);
    updateLocalLists();
}

