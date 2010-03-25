/*
 * SqlQueryBuilder.h
 *
 *  Created on: 23.03.2010
 *      Author: bgdia
 */

#ifndef SQLQUERYBUILDER_H_
#define SQLQUERYBUILDER_H_

namespace pepdb
{

class SqlQueryBuilder
{
public:
    SqlQueryBuilder();
    virtual ~SqlQueryBuilder();

    QString tables();
    QString columns(const QString & table);
    QString uniqueValues(const Qtring & table, const QString & column);
    QString values()
};
}

#endif /* SQLQUERYBUILDER_H_ */

