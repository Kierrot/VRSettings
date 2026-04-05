#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    fillList(ui->impLayers, regManager.getImplicitKeys(), true, regManager.getImpPath());
    fillList(ui->expLayers, regManager.getExplicitKeys(), false, regManager.getExpPath());
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::showDebugLog(QString message){
    ui->statusbar->showMessage(message);
}

QString cropPath(QString path){
    return QFileInfo(path).baseName();
}


void MainWindow::fillList(QListWidget *list, const QList<QPair<QString, int>> &registryMap, bool checksNeeded, const QString &branchPath){
    list->blockSignals(true);
    for (const auto &pair : registryMap){
        const QString &rkey = pair.first;
        QListWidgetItem *item = new QListWidgetItem(cropPath(rkey), list);
        item->setToolTip(rkey);
        item->setData(Qt::UserRole, branchPath);
        if(checksNeeded){
            item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
            item->setCheckState(pair.second == 0 ? Qt::Checked : Qt::Unchecked);
        }
    }
    list->blockSignals(false);
}

void MainWindow::on_impLayers_itemChanged(QListWidgetItem *item) {
    regManager.setRegistryValue(item->data(Qt::UserRole).toString(), item->toolTip(), item->checkState() == Qt::Checked ? 0 : 1);
}

void MainWindow::updateList(QListWidget *list, const QList<QPair<QString, int>> &registryMap){
    list->clear();
    fillList(list, registryMap, list == ui->impLayers, list == ui->impLayers ? regManager.getImpPath() : regManager.getExpPath());
}


