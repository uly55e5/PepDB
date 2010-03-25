/*
 * SearchWidget.cpp
 *
 *  Created on: 23.03.2010
 *      Author: bgdia
 */

#include "searchwidget.h"

namespace pepdb
{

SearchWidget::SearchWidget(QWidget * parent): QWidget(parent)
{
    ui.setupUi(this);

    connect(ui.boxType,SIGNAL(currentIndexChanged),SLOT(newSearch()));
    // TODO Auto-generated constructor stub

}

SearchWidget::~SearchWidget()
{
    // TODO Auto-generated destructor stub
}

}
