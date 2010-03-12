#ifndef RESULTTABLE_H
#define RESULTTABLE_H

#include <QtGui/QWidget>
#include "ui_resulttable.h"

class QAbstractItemModel;

namespace pepdb
{
class ResultTable : public QWidget
{
    Q_OBJECT

public:
    ResultTable(QWidget *parent = 0);
    ~ResultTable();
    void	setModel ( QAbstractItemModel * model );

private:
    Ui::ResultTableClass ui;
};
}
#endif // RESULTTABLE_H
