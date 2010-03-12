#ifndef EXCEPTION_H
#define EXCEPTION_H

#include <QString>

namespace pepdb
{
class Exception
{
public:
    Exception(const QString & message);
    virtual ~Exception();
    QString message() const;
private:
    QString _message;
};
}
#endif // EXCEPTION_H
