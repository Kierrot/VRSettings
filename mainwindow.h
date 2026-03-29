#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QList>
#include <QListWidgetItem>
#include <QFileInfo>
#include "registrymanager.h"

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
    void updateLayersLists();


private:
    Ui::MainWindow *ui;
    RegistryManager regManager;
    void fillList(QListWidget *list, QMap<QString, int> registryMap);
    void updateList(QListWidget *list, QMap<QString, int> registryMap);
};
#endif // MAINWINDOW_H
