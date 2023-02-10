#include "dropdownbutton.h"
#include "qmenu.h"

dropdownButton::dropdownButton(QWidget * parent) : QToolButton(parent)
{
    setPopupMode(QToolButton::MenuButtonPopup);
}


void dropdownButton::addItem(QString name, QString value)
{
    items.push_back(QPair<QString, QString>(name,value));
}

void dropdownButton::addItem(QString name)
{
    items.push_back(QPair<QString, QString>(name,name));
}

QString dropdownButton::getItemName()
{
    return items[currentItem].first;
}

QString dropdownButton::getItemValue()
{
    return items[currentItem].second;
}

void dropdownButton::createActions()
{
    size_t counter = 0;
    for (auto& item : items)
    {
        QAction * newAction = new QAction(item.first, this);

        auto fx = [this, counter]()
        {
            this->update(counter);
        };

        QObject::connect(newAction, &QAction::triggered, this, fx);
        counter+=1;
        actions.push_back(newAction);
    }

    menu = new QMenu;

    for (auto& action : actions)
    {
        menu->addAction(action);
    }
}

void dropdownButton::update(const size_t index)
{
    currentItem = index;
    setText(this->getItemName());
}

void dropdownButton::initMenu()
{
    this->createActions();
    this->setMenu(menu);
    this->update(0);
}

 void dropdownButton::setValue(const QString set)
 {
    size_t index = 0;
    for (auto itr = items.begin(); itr != items.end(); itr++)
    {
        if (itr->second == set)
        {
            index = itr - items.begin();
            break;
        }
    }

    update(index);

 }
