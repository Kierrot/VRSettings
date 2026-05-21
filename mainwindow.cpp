#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    updateUI();

    connect(ui->syncWithReg, &QPushButton::clicked, this, [this]() {
        manager.updateLists();
        updateUI();
    });

    connect(ui->impLayers->model(), &QAbstractItemModel::rowsMoved, this,
            [this](const QModelIndex&, int start, int end, const QModelIndex&, int dest) {
                if (dest > start) dest--;
                if (dest >= manager.implicitLayers.size()) {
                    dest = manager.implicitLayers.size() - 1;
                }
                manager.implicitLayers.move(start, dest);
                manager.changeLayersSystemOrder(DataManager::DataType::Implicit, manager.implicitLayers);
            });

    connect(ui->expLayers->model(), &QAbstractItemModel::rowsMoved, this,
            [this](const QModelIndex&, int start, int end, const QModelIndex&, int dest) {
                if (dest >= manager.explicitLayers.size()) {
                    dest = manager.explicitLayers.size() - 1;
                }
                manager.explicitLayers.move(start, dest);
                manager.changeLayersSystemOrder(DataManager::DataType::Explicit, manager.explicitLayers);
            });

    connect(ui->runtimeComboBox, &QComboBox::currentIndexChanged, this,
            [this](){
        QString newRuntimePath = ui->runtimeComboBox->currentData().toString();
            if(FileManager::isFileExists(newRuntimePath)){
                    manager.setRegistryValueData(manager.getRegKey(DataManager::DataType::RuntimeActive),
                                     "ActiveRuntime",
                                     ui->runtimeComboBox->currentData().toString(),
                                     DataManager::RegType::String
                                     );}
            else{
                QMessageBox::StandardButton reply;
                reply = QMessageBox::question(
                    this,
                    "Runtime Corrupted!",
                    "Delete from Registry?",
                    QMessageBox::Yes | QMessageBox::No
                    );

                if (reply == QMessageBox::Yes) {
                    manager.deleteRegistryValue(manager.getRegKey(DataManager::DataType::RuntimeAvailable), newRuntimePath);
                    manager.updateLists();
                    updateUI();
                }

            }
    });

}

MainWindow::~MainWindow()
{
    delete ui;
}
void MainWindow::blockAllSignals(bool flag){
    ui->impLayers->blockSignals(flag);
    ui->expLayers->blockSignals(flag);
    ui->runtimeComboBox->blockSignals(flag);
}

void MainWindow::updateUI(){
    blockAllSignals(true);
    ui->impLayers->clear();
    ui->expLayers->clear();
    ui->runtimeComboBox->clear();

    fillListWidget(ui->impLayers,
                   manager.fetchData(DataManager::DataType::Implicit),
                   manager.getRegKey(DataManager::DataType::Implicit)
                   );

    fillListWidget(ui->expLayers,
                   manager.fetchData(DataManager::DataType::Explicit),
                   manager.getRegKey(DataManager::DataType::Explicit)
                   );

    fillComboBox(ui->runtimeComboBox,
                 manager.fetchData(DataManager::DataType::RuntimeAvailable)
                 );
    blockAllSignals(false);
}

void MainWindow::fillListWidget(QListWidget *list, const QList<DataManager::Item> &items, const QString &branchPath) {
    list->clear();
    list->blockSignals(true);
    for (const auto &itemData : items) {
        QListWidgetItem *item = new QListWidgetItem(itemData.displayName, list);
        item->setToolTip(itemData.registryKey);
        item->setData(Qt::UserRole, branchPath);

        if(itemData.type == DataManager::DataType::Implicit){
            item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
            item->setCheckState(itemData.isActive == 0 ? Qt::Checked : Qt::Unchecked);
        }
    }
    list->setDragDropMode(QAbstractItemView::InternalMove);
    list->blockSignals(false);
}

void MainWindow::fillComboBox(QComboBox *combo, const QList<DataManager::Item> &items) {
    combo->clear();

    for (const auto &itemData : items) {
        combo->addItem(itemData.displayName, itemData.registryKey);
    }
}

void MainWindow::on_impLayers_itemChanged(QListWidgetItem *item) {
    manager.setRegistryValueData(
        item->data(Qt::UserRole).toString(),
        item->toolTip(),
        item->checkState() ==  Qt::Checked ? 0 : 1,
        DataManager::RegType::DWord
    );
    manager.updateLists();
}

