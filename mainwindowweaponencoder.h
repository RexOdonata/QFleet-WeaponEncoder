#ifndef MAINWINDOWWEAPONENCODER_H
#define MAINWINDOWWEAPONENCODER_H

#include <QMainWindow>
#include <QStringListModel>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindowWeaponEncoder; }
QT_END_NAMESPACE

class MainWindowWeaponEncoder : public QMainWindow
{
    Q_OBJECT

public:
    MainWindowWeaponEncoder(QWidget *parent = nullptr);
    ~MainWindowWeaponEncoder();

private slots:


    void on_SaveWeaponButton_clicked();

    void on_specialAdd_clicked();

    void on_specialRest_clicked();

    void on_loadButton_clicked();

    void on_DeleteButton_clicked();

    void on_actionSave_triggered();

    void on_actionOpen_triggered();

    void on_specialReset_clicked();

private:
    Ui::MainWindowWeaponEncoder *ui;

    QStringListModel * listModelWeapons;

    QVector<QJsonObject> weaponData;

    QVector<QString> specialRules;

    QStringListModel * listModelSpecial;

    void fileSave();

    void fileOpen();

    void refreshWeaponList();

    void refreshSpecialList();

    QString AssembleSpecialRules();

};
#endif // MAINWINDOWWEAPONENCODER_H
