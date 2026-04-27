#include "mainwindow.h"
#include "filemanager.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->syncWithReg, &QPushButton::clicked, this, [this]() {
        regManager.updateLists();
    });

    connect(ui->impLayers->model(), &QAbstractItemModel::rowsMoved, this,
            [this](const QModelIndex&, int start, int end, const QModelIndex&, int dest) {
                if (dest >= regManager.implicitLayers.size()) {
                    dest = regManager.implicitLayers.size() - 1;
                }
                regManager.implicitLayers.move(start, dest);
                regManager.changeLayersSystemOrder(RegistryManager::DataType::Implicit, regManager.implicitLayers);
            });

    connect(ui->expLayers->model(), &QAbstractItemModel::rowsMoved, this,
            [this](const QModelIndex&, int start, int end, const QModelIndex&, int dest) {
                if (dest >= regManager.explicitLayers.size()) {
                    dest = regManager.explicitLayers.size() - 1;
                }
                regManager.explicitLayers.move(start, dest);
                regManager.changeLayersSystemOrder(RegistryManager::DataType::Explicit, regManager.explicitLayers);
            });

    fillListWidget(ui->impLayers,
                   fetchData(regManager.implicitLayers, RegistryManager::DataType::Implicit),
                   regManager.getRegKey(RegistryManager::DataType::Implicit)
                   );

    fillListWidget(ui->expLayers,
                   fetchData(regManager.explicitLayers, RegistryManager::DataType::Explicit),
                   regManager.getRegKey(RegistryManager::DataType::Explicit)
                   );

    fillComboBox(ui->runtimeComboBox,
                 fetchData(regManager.availableRuntimes, RegistryManager::DataType::RuntimeAvailable)
                 );
}

MainWindow::~MainWindow()
{
    delete ui;
}

QList<MainWindow::Item> MainWindow::fetchData(const QList<RegistryEntry> &registryMap, RegistryManager::DataType dataType) {
    QList<MainWindow::Item> items;
    for (const auto &pair : registryMap) {
        QString rkey = pair.first, name = FileManager::getJsonElementByName(rkey, "name");
        int isActive;
        if(dataType == RegistryManager::DataType::RuntimeAvailable && name == FileManager::getJsonElementByName(regManager.activeRuntime, "name")){
            isActive = 1;
        }
        else
            isActive = pair.second;

        items.append({
            dataType,
            name,
            rkey,
            isActive
        });
    }
    return items;
}

void MainWindow::fillListWidget(QListWidget *list, const QList<Item> &items, const QString &branchPath) {
    list->clear();
    list->blockSignals(true);
    for (const auto &itemData : items) {
        QListWidgetItem *item = new QListWidgetItem(itemData.displayName, list);
        item->setToolTip(itemData.registryKey);
        item->setData(Qt::UserRole, branchPath);
        if(itemData.type == RegistryManager::DataType::Implicit){
            item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
            item->setCheckState(itemData.isActive == 0 ? Qt::Checked : Qt::Unchecked);
        }
    }
    list->setDragDropMode(QAbstractItemView::InternalMove);
    list->blockSignals(false);
}

void MainWindow::fillComboBox(QComboBox *combo, const QList<Item> &items) {
    combo->clear();
    for (const auto &itemData : items) {
        combo->addItem(itemData.displayName, itemData.registryKey);
    }


}

void MainWindow::on_impLayers_itemChanged(QListWidgetItem *item) {
    regManager.setRegistryValueData(
        item->data(Qt::UserRole).toString(),
        item->toolTip(),
        item->checkState() ==  Qt::Checked ? 0 : 1
    );
}

