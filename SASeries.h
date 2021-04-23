#ifndef SASERIES_H
#define SASERIES_H

#include <QtCore/qglobal.h>
#include "SATable.h"
#include "QVariant"

class SASeries : public SAVector<QVariant>
{
public:
    SASeries();
    SASeries(int size);
    SASeries(int size, const QVariant& t);
    SASeries(std::initializer_list<QVariant> args);
    SASeries(const QString& n);
    virtual ~SASeries();
    virtual int dtype() const = 0;
};


#endif // SASERIES_H
