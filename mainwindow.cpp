#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    updateUI();

    connect(ui->syncWithReg, &QPushButton::clicked, this, [this]() {
        dm.updateLists();
        updateUI();
    });

    connect(ui->impLayers->model(), &QAbstractItemModel::rowsMoved, this,
            [this](const QModelIndex&, int start, int end, const QModelIndex&, int dest) {
                if (dest > start) dest--;
                if (dest >= dm.implicitLayers.size()) {
                    dest = dm.implicitLayers.size() - 1;
                }
                dm.implicitLayers.move(start, dest);
                dm.changeLayersSystemOrder(DataManager::DataType::Implicit, dm.implicitLayers);
            });

    connect(ui->expLayers->model(), &QAbstractItemModel::rowsMoved, this,
            [this](const QModelIndex&, int start, int end, const QModelIndex&, int dest) {
                if (dest >= dm.explicitLayers.size()) {
                    dest = dm.explicitLayers.size() - 1;
                }
                dm.explicitLayers.move(start, dest);
                dm.changeLayersSystemOrder(DataManager::DataType::Explicit, dm.explicitLayers);
            });

    connect(ui->runtimeComboBox, &QComboBox::currentIndexChanged, this,
            [this](){
        QString newRuntimePath = ui->runtimeComboBox->currentData().toString();
        qDebug() << ui->runtimeComboBox->currentData().toString();

            if(FileManager::isFileExists(newRuntimePath)){
                    dm.setRegistryValueData(dm.getRegKey(DataManager::DataType::RuntimeActive),
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
                    dm.deleteRegistryValue(dm.getRegKey(DataManager::DataType::RuntimeAvailable), newRuntimePath);
                    dm.updateLists();
                    updateUI();
                }

            }
    });

    connect(ui->fromImptoExp, &QPushButton::clicked, this, [this]() {
        if(!ui->impLayers->currentItem())
            return;

        auto item = ui->impLayers->currentItem();
        QString jsonName = item->toolTip();

        if(ui->impLayers->currentItem()->isSelected()){
            dm.deleteRegistryValue(dm.getRegKey(DataManager::DataType::Implicit), jsonName);
            dm.setRegistryValueData(dm.getRegKey(DataManager::DataType::Explicit), jsonName, item->checkState() == 0 ? 1 : 0, DataManager::RegType::DWord);
        }
        dm.updateLists();
        updateUI();
    });

    connect(ui->fromExptoImp, &QPushButton::clicked, this, [this]() {
        if(!ui->expLayers->currentItem())
            return;

        auto item = ui->expLayers->currentItem();
        QString jsonName = item->toolTip();

        if(ui->expLayers->currentItem()->isSelected())
        {
            dm.getRegistryValue(dm.getRegKey(DataManager::DataType::Explicit), jsonName);
            dm.setRegistryValueData(
                dm.getRegKey(DataManager::DataType::Implicit),
                jsonName,
                dm.getRegistryValue(dm.getRegKey(DataManager::DataType::Explicit), jsonName),
                DataManager::RegType::DWord);

            dm.deleteRegistryValue(dm.getRegKey(DataManager::DataType::Explicit), jsonName);
        }
        dm.updateLists();
        updateUI();
    });


    connect(ui->addLayerPush, &QPushButton::clicked, this, [this]() {
        QMessageBox mbLayerType;
        mbLayerType.setWindowTitle("Choose New Layer Type");
        mbLayerType.setText("Which Type of layer do youwant to add?");
        QPushButton *chooseImplicit = mbLayerType.addButton("Implicit", QMessageBox::AcceptRole);
        QPushButton *chooseExcplicit = mbLayerType.addButton("Excplicit", QMessageBox::ActionRole);
        QPushButton *cancelButton = mbLayerType.addButton("Cancel", QMessageBox::RejectRole);
        QString filePath;

        auto getLayerPath = []() {
            QString filePath = QFileDialog::getOpenFileName(
                nullptr,                      // Родительский виджет
                "Поиск файла для открытия",   // Заголовок окна
                "C:\\",                       // Начальная директория
                "Текстовые (*.json) ;; Все файлы (*.*)" // Фильтр файлов
                );

            if (!filePath.isEmpty()) {
            };

            return filePath;
        };

        mbLayerType.exec();

        if (mbLayerType.clickedButton() == chooseImplicit) {
            filePath = getLayerPath();
            dm.setRegistryValueData(
                dm.getRegKey(DataManager::DataType::Implicit),
                filePath,
                0,
                DataManager::RegType::DWord);

        } else if (mbLayerType.clickedButton() == chooseExcplicit) {
            filePath = getLayerPath();
            dm.setRegistryValueData(
                dm.getRegKey(DataManager::DataType::Explicit),
                filePath,
                0,
                DataManager::RegType::DWord);
        }
        else{
            mbLayerType.close();
        }

        dm.updateLists();
        updateUI();
    });

    connect(ui->delLayerPush, &QPushButton::clicked, this, [this](){
        bool isImplicit = ui->impLayers->currentItem() ? true : false;
        if(!isImplicit && !ui->expLayers->currentItem())
            return;

        auto item = isImplicit ? ui->impLayers->currentItem() : ui->expLayers->currentItem();

        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(
            this,
            "Deleting Layer",
            "Delete from Registry?",
            QMessageBox::Yes | QMessageBox::No
            );


        if (reply == QMessageBox::Yes) {
            qDebug() << "Deleting" << dm.getRegKey(isImplicit ? DataManager::DataType::Implicit : DataManager::DataType::Explicit) << item->toolTip();
            dm.deleteRegistryValue(dm.getRegKey(isImplicit ? DataManager::DataType::Implicit : DataManager::DataType::Explicit), item->toolTip());
            dm.updateLists();
            updateUI();
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
                   dm.fetchData(DataManager::DataType::Implicit),
                   dm.getRegKey(DataManager::DataType::Implicit)
                   );

    fillListWidget(ui->expLayers,
                   dm.fetchData(DataManager::DataType::Explicit),
                   dm.getRegKey(DataManager::DataType::Explicit)
                   );

    fillComboBox(ui->runtimeComboBox,
                 dm.fetchData(DataManager::DataType::RuntimeAvailable)
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
    dm.setRegistryValueData(
        item->data(Qt::UserRole).toString(),
        item->toolTip(),
        item->checkState() ==  Qt::Checked ? 0 : 1,
        DataManager::RegType::DWord
    );
    dm.updateLists();
}

