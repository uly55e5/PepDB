/*
 * SearchWidget.h
 *
 *  Created on: 23.03.2010
 *      Author: bgdia
 */

#ifndef SEARCHWIDGET_H_
#define SEARCHWIDGET_H_

#include <QWidget>
#include "ui_searchwidget.h"

namespace pepdb
{

class SearchWidget: public QWidget
{
    enum SearchType {
        FeatureSearch,
        BatchSearch,
        SimpleSearch
    };
public:
    SearchWidget(QWidget * parent=0);
    virtual ~SearchWidget();
private:
    Ui::SearchWidgetClass ui;
};

}

#endif /* SEARCHWIDGET_H_ */
