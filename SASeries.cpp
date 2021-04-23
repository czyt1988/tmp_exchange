#include "SASeries.h"

SASeries::SASeries() : SAVector<QVariant>()
{
}


SASeries::SASeries(int size) : SAVector<QVariant>(size)
{
}


SASeries::SASeries(int size, const QVariant& t) : SAVector<QVariant>(size, t)
{
}


SASeries::SASeries(std::initializer_list<QVariant> args) : SAVector<QVariant>(args)
{
}


SASeries::SASeries(const QString& n) : SAVector<QVariant>(n)
{
}


SASeries::~SASeries()
{
}
