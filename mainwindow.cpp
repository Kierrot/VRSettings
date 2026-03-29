#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    fillList(ui->impLayers, regManager.getImplicitKeys());
    fillList(ui->expLayers, regManager.getExplicitKeys());

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

void MainWindow::fillList(QListWidget *list, QMap<QString, int> registryMap){
    for (QMap<QString, int>::iterator i = registryMap.begin(); i != registryMap.end(); ++i) {
        QListWidgetItem *item = new QListWidgetItem(cropPath(i.key()), list);
        item->setToolTip(i.key());
        item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
        item->setCheckState(i.value() == 0 ? Qt::Checked : Qt::Unchecked);
    }
}

void MainWindow::updateList(QListWidget *list, QMap<QString, int> registryMap){
    list->clear();
    fillList(list, registryMap);
}


