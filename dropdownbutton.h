#ifndef DROPDOWNBUTTON_H
#define DROPDOWNBUTTON_H

#include <QToolButton>
#include <QVector>
#include <QString>
#include <QPair>


class dropdownButton : public QToolButton
{
    Q_OBJECT
public:
    dropdownButton(QWidget * Parent = 0);
    void addItem(QString name, QString value);
    void addItem(QString name);
    QString getItemName();
    QString getItemValue();

    void update(const size_t);
    void setValue(const QString);

    void initMenu();
    void createActions();

private:
      QVector<QPair<QString,QString>> items;
      QVector<QAction*> actions;
      size_t currentItem;
      QMenu * menu;
};

#endif // DROPDOWNBUTTON_H
