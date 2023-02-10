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

#include <QRegularExpression>


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

    ui->randomAttackDiceType->addItem("D3");
    ui->randomAttackDiceType->addItem("D6");


    ui->lockButton->initMenu();
    ui->facingButton->initMenu();
    ui->specialSelect->initMenu();    
    ui->randomAttackDiceType->initMenu();

    listModelWeapons = new QStringListModel(this);
    ui->weaponView->setModel(listModelWeapons);

    listModelSpecial = new QStringListModel(this);
    ui->specialView->setModel(listModelSpecial);
}

MainWindowWeaponEncoder::~MainWindowWeaponEncoder()
{
    delete ui;
}


QString MainWindowWeaponEncoder::AssembleSpecialRules()
{
    QString output="";

    for (auto& rule : specialRules)
    {
        output.append(rule + ",");
    }

    if (output.length() > 1 && output.back() == ',')
        output.resize(output.size()-1);
}

void MainWindowWeaponEncoder::on_SaveWeaponButton_clicked()
{
    QJsonObject newWeapon;

    QString id = ui->IdEdit->text();
    QString name = ui->NameEdit->text();
    QString lock = ui->lockButton->getItemValue();
    QString facing = ui->facingButton->getItemValue();
    QString baseAttacks;
    QString randomAttacks= "";
    QString attacks="";

    //logic is a bit tricky here
    if (ui->randomAttacks->isChecked())
    {
        // first, only add the number of dice to the display string if using more than one
        if (ui->randomAttackDiceCount->value()> 1)
           attacks.append(ui->randomAttackDiceCount->text());

        attacks.append(ui->randomAttackDiceType->getItemValue());

        // only add the base attacks to display string if greater than 0
        if (ui->AttackEdit->value() > 0)
            attacks.append("+"  + ui->AttackEdit->text());

        //actually set the values for internal tracking
        randomAttacks=  ui->randomAttackDiceCount->text() + ui->randomAttackDiceType->getItemValue();
        baseAttacks = ui->AttackEdit->text();
    }
    else
    {
        baseAttacks = ui->AttackEdit->text();
        attacks = baseAttacks;
    }




    QString damage;
    if (ui->VarDmgCheck->isChecked())
        damage = "X";
    else
        damage = ui->DmgEdit->text();

    QString special = AssembleSpecialRules();

    // ATTACKS is the actual displayed value that is passed alone
    // baseATTACKS and randomATTACKs are used for internal purposes only

    newWeapon.insert("id",id);
    newWeapon.insert("name",name);
    newWeapon.insert("lock",lock);
    newWeapon.insert("facing",facing);
    newWeapon.insert("attacksDisplay",attacks);
    newWeapon.insert("randomAttacks",randomAttacks);
    newWeapon.insert("baseAttacks",baseAttacks);
    newWeapon.insert("damage",damage);
    newWeapon.insert("specialDisplay",special);

    QJsonArray specialRulesArray;

    for (auto& rule : specialRules)
    {
        specialRulesArray.push_back(rule);
    }

    newWeapon.insert("specialList",specialRulesArray);

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

    refreshWeaponList();

}


void MainWindowWeaponEncoder::on_specialAdd_clicked()
{
    QString special = ui->specialSelect->getItemValue();

    if (special.contains("(X)"))
    {
        special.replace("(X)",ui->specialX->text());
    }

    specialRules.push_back(special);

    refreshSpecialList();

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

    refreshWeaponList();
}

void MainWindowWeaponEncoder::refreshWeaponList()
{
    QStringList list;

    for (auto& element : weaponData)
    {
        QString name = element["id"].toString();
        list.push_back(name);
    }

    listModelWeapons->setStringList(list);
}

void MainWindowWeaponEncoder::refreshSpecialList()
{
    QStringList list;

    for (auto& element : specialRules)
        list.push_back(element);

    listModelSpecial->setStringList(list);
}

void MainWindowWeaponEncoder::on_loadButton_clicked()
{
    if (ui->weaponView->selectionModel()->selectedIndexes().size() > 0)
    {
        auto index = ui->weaponView->selectionModel()->selectedIndexes().front().row();

        QJsonObject weapon = weaponData.at(index);

        ui->IdEdit->clear();
        ui->IdEdit->insert(weapon["id"].toString());

        ui->NameEdit->clear();
        ui->NameEdit->insert(weapon["name"].toString());

        ui->lockButton->setValue(weapon["lock"].toString());
        ui->facingButton->setValue(weapon["facing"].toString());

        if (weapon["randomAttacks"].toVariant().toString().length() > 0)
        {
            QString randomAttacks = weapon["randomAttacks"].toVariant().toString();
            QStringList properties = randomAttacks.split("D");
            int val = properties[0].toInt();
            ui->randomAttackDiceCount->setValue(val);
            ui->randomAttackDiceType->setValue("D" + properties[1]);
            ui->randomAttacks->setCheckState(Qt::Checked);
        }
        else
            ui->randomAttacks->setCheckState(Qt::Unchecked);


        ui->AttackEdit->setValue(weapon["baseAttacks"].toVariant().toInt());

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

        specialRules.clear();
        QJsonArray specialRulesList = weapon["specialList"].toArray();

        for (auto rule : specialRulesList)
            specialRules.push_back(rule.toVariant().toString());

        refreshSpecialList();
    }
}


void MainWindowWeaponEncoder::on_DeleteButton_clicked()
{
    if (ui->weaponView->selectionModel()->selectedIndexes().size() > 0)
    {
        auto index = ui->weaponView->selectionModel()->selectedIndexes().front().row();

        weaponData.remove(index);

        refreshWeaponList();
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


void MainWindowWeaponEncoder::on_specialReset_clicked()
{
    specialRules.clear();
    refreshSpecialList();
}

