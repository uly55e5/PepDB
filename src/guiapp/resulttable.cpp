#include "resulttable.h"

namespace pepdb
{
ResultTable::ResultTable(QWidget *parent)
    : QWidget(parent)
{
	ui.setupUi(this);
}

ResultTable::~ResultTable()
{

}

void	ResultTable::setModel ( QAbstractItemModel * model )
{
	ui.tableView->setModel(model);
}
}
