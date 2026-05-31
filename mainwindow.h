#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>
#include <QList>
#include <QListWidgetItem>
#include <QFileInfo>
#include <QComboBox>
#include <QFileDialog>
#include "datamanager.h"
#include "filemanager.h"
#include "ui_mainwindow.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void showDebugLog(QString message);

private:
    Ui::MainWindow *ui;
    DataManager dm;
    FileManager fm;
    void updateUI();
    void blockAllSignals(bool flag);
    void fillListWidget(QListWidget *list, const QList<DataManager::Item> &items, const QString &branchPath);
    void fillComboBox(QComboBox *combo, const QList<DataManager::Item> &items);


private slots:
    void on_impLayers_itemChanged(QListWidgetItem *item);
};
#endif // MAINWINDOW_H
