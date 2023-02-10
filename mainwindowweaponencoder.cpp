#include "mainwindowweaponencoder.h"
#include "./ui_mainwindowweaponencoder.h"
#include "dropdownbutton.h"

#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QFile>

#include <QStringList>

#include <QStringView>
#include <QMessageBox>

#include <QFileDialog>


MainWindowWeaponEncoder::MainWindowWeaponEncoder(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindowWeaponEncoder)
{
    ui->setupUi(this);

    ui->lockButton->addItem("Lock : 2+","2+");
    ui->lockButton->addItem("Lock : 3+","3+");
    ui->lockButton->addItem("Lock : 4+","4+");
    ui->lockButton->addItem("Lock : 5+","5+");
    ui->lockButton->addItem("Lock : 6+","6+");
    ui->lockButton->addItem("Lock : X+","X+");

    ui->facingButton->addItem("F(N)","F(N)");
    ui->facingButton->addItem("F","F");
    ui->facingButton->addItem("F/S","F/S");
    ui->facingButton->addItem("F/S(R)","F/S(R)");
    ui->facingButton->addItem("F/S(L)","F/S(L)");
    ui->facingButton->addItem("F/S/R","F/S/R");

    ui->specialSelect->addItem("Air to Air");
    ui->specialSelect->addItem("Alt-X","Alt-(X)");
    ui->specialSelect->addItem("Bloom");
    ui->specialSelect->addItem("Bombardment");
    ui->specialSelect->addItem("Burnthrough-X", "Burnthrough-(X)");
    ui->specialSelect->addItem("Caliber-L");
    ui->specialSelect->addItem("Caliber-HS");
    ui->specialSelect->addItem("Close Action");
    ui->specialSelect->addItem("Close Action(Swarmer)");
    ui->specialSelect->addItem("Close Action(Beam)");
    ui->specialSelect->addItem("Corruptor");
    ui->specialSelect->addItem("Crippling");
    ui->specialSelect->addItem("Distortion");
    ui->specialSelect->addItem("Escape Velocity");
    ui->specialSelect->addItem("Flash");
    ui->specialSelect->addItem("Fusillade-X","Fusillade-(X)");
    ui->specialSelect->addItem("Impel", "Impel-(X)");
    ui->specialSelect->addItem("Limited", "Limited-(X)");
    ui->specialSelect->addItem("Link", "Link-(X)");
    ui->specialSelect->addItem("Low Power");
    ui->specialSelect->addItem("Mauler");
    ui->specialSelect->addItem("Overcharge");
    ui->specialSelect->addItem("Particle");
    ui->specialSelect->addItem("Re-Entry");
    ui->specialSelect->addItem("Scald");
    ui->specialSelect->addItem("Squadron-X","Squadron-(X)");
    ui->specialSelect->addItem("Unstable");


    ui->lockButton->initMenu();
    ui->facingButton->initMenu();
    ui->specialSelect->initMenu();

    listModel = new QStringListModel(this);
    ui->listView->setModel(listModel);
}

MainWindowWeaponEncoder::~MainWindowWeaponEncoder()
{
    delete ui;
}


void MainWindowWeaponEncoder::on_SaveWeaponButton_clicked()
{
    QJsonObject newWeapon;

    QString id = ui->IdEdit->text();
    QString name = ui->NameEdit->text();
    QString lock = ui->lockButton->getItemValue();
    QString facing = ui->facingButton->getItemValue();

    QString attacks;
    if (ui->VarAtkCheck->isChecked())
        attacks = "X";
    else
        attacks = ui->AttackEdit->text();

    QString damage;
    if (ui->VarDmgCheck->isChecked())
        damage = "X";
    else
        damage = ui->DmgEdit->text();

    QString special = ui->SpecialEdit->toPlainText();

    special.replace("\n",",");
    if (special.length() > 0 && special.back() == '\n')
        special.erase(special.constEnd()-1, special.constEnd());

    newWeapon.insert("id",id);
    newWeapon.insert("name",name);
    newWeapon.insert("lock",lock);
    newWeapon.insert("facing",facing);
    newWeapon.insert("attacks",attacks);
    newWeapon.insert("damage",damage);
    newWeapon.insert("special",special);

    bool exists = false;

    QJsonObject * itemRef = NULL;
    for (auto& element : weaponData)
    {
        if (id == element["id"].toString())
        {
            exists = true;
            itemRef = &element;
        }
    }

    if (exists)
    {
        *itemRef = newWeapon;
    }
    else
        weaponData.push_back(newWeapon);

    refreshList();

}


void MainWindowWeaponEncoder::on_specialAdd_clicked()
{
    QString special = ui->specialSelect->getItemValue();

    if (special.contains("(X)"))
    {
        special.replace("(X)",ui->specialX->text());
    }

    ui->SpecialEdit->append(special);

}


void MainWindowWeaponEncoder::on_specialRest_clicked()
{
     ui->SpecialEdit->clear();
}

void MainWindowWeaponEncoder::fileSave()
{

    QString filename = QFileDialog::getSaveFileName(this, "save content", QDir::currentPath());

    QFile file(filename);

    QJsonArray jsonData;

    for (auto& element : weaponData)
    {
        jsonData.append(QJsonValue(element));

    }

    if (file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate))
    {
        QJsonDocument json(jsonData);

        QByteArray bytes = json.toJson(QJsonDocument::Indented);

        QTextStream istream(&file);
        istream << bytes;
        file.close();
    }
}

void MainWindowWeaponEncoder::fileOpen()
{
    weaponData.clear();

    QString filename = QFileDialog::getOpenFileName(this, "open content", QDir::currentPath());

    QFile file(filename);

    if (file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QByteArray data = file.readAll();

        file.close();

        QJsonParseError err;

        QJsonDocument jsonData = QJsonDocument::fromJson(data, &err);

        QJsonArray weapons = jsonData.array();

        for (auto weapon : weapons)
        {
            weaponData.push_back(weapon.toObject());
        }

    }

    refreshList();
}

void MainWindowWeaponEncoder::refreshList()
{
    QStringList list;

    for (auto& element : weaponData)
    {
        QString name = element["id"].toString();
        list.push_back(name);
    }

    listModel->setStringList(list);


}

void MainWindowWeaponEncoder::on_loadButton_clicked()
{
    if (ui->listView->selectionModel()->selectedIndexes().size() > 0)
    {
        auto index = ui->listView->selectionModel()->selectedIndexes().front().row();

        QJsonObject weapon = weaponData.at(index);

        ui->IdEdit->clear();
        ui->IdEdit->insert(weapon["id"].toString());

        ui->NameEdit->clear();
        ui->NameEdit->insert(weapon["name"].toString());

        ui->lockButton->setValue(weapon["lock"].toString());
        ui->facingButton->setValue(weapon["facing"].toString());

        if (weapon["attacks"].toString() == "X")
        {
            ui->VarAtkCheck->setCheckState(Qt::Checked);
        }
        else
        {
            ui->VarAtkCheck->setCheckState(Qt::Unchecked);
            int val = weapon["attacks"].toVariant().toInt();
            ui->AttackEdit->setValue(val);
        }

        if (weapon["damage"].toString() == "X")
        {
            ui->VarDmgCheck->setCheckState(Qt::Checked);
        }
        else
        {
            ui->VarDmgCheck->setCheckState(Qt::Unchecked);
            int val = weapon["damage"].toVariant().toInt();
            ui->DmgEdit->setValue(val);
        }

        QString special = weapon["special"].toString();
        ui->SpecialEdit->clear();
        ui->SpecialEdit->insertPlainText(special);
    }
}


void MainWindowWeaponEncoder::on_DeleteButton_clicked()
{
    if (ui->listView->selectionModel()->selectedIndexes().size() > 0)
    {
        auto index = ui->listView->selectionModel()->selectedIndexes().front().row();

        weaponData.remove(index);

        refreshList();
    }
}


void MainWindowWeaponEncoder::on_actionSave_triggered()
{
    fileSave();
}


void MainWindowWeaponEncoder::on_actionOpen_triggered()
{
    fileOpen();
}

