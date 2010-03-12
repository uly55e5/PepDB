#include "exception.h"
namespace pepdb
{
Exception::Exception(const QString & message)
{
    _message = message;
}

Exception::~Exception()
{
}

QString Exception::message() const
{
    return _message;
}
}
