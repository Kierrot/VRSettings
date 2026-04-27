#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QList>
#include <QListWidgetItem>
#include <QFileInfo>
#include <QComboBox>
#include "registrymanager.h"
using RegistryEntry = QPair<QString, int>;

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
    struct Item {
        RegistryManager::DataType type;
        QString displayName;
        QString registryKey;
        int isActive;
    };

    Ui::MainWindow *ui;
    RegistryManager regManager;
    QList<MainWindow::Item> fetchData(const QList<RegistryEntry> &registryMap, RegistryManager::DataType dataType);
    void fillListWidget(QListWidget *list, const QList<Item> &items, const QString &branchPath);
    void fillComboBox(QComboBox *combo, const QList<Item> &items);
    QList<RegistryEntry> getList(QListWidget *list);

    void fillList(QListWidget *list, const QList<RegistryEntry> &registryMap, bool checksNeeded, const QString &branchPath);
    void updateList(QListWidget *list, const QList<RegistryEntry> &registryMap);

private slots:
    void on_impLayers_itemChanged(QListWidgetItem *item);
};
#endif // MAINWINDOW_H
