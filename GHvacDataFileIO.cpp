#include "GHvacDataFileIO.h"
#include <QDebug>
#include <QDir>
#include "quazipfile.h"
#include <QTextStream>
#include "SACsvStream.h"
#include "SATable.h"
#include <QDateTime>
GHvacDataFileIO::GHvacDataFileIO() : mIsOpen(false)
{
}


GHvacDataFileIO::GHvacDataFileIO(const QString& filepath) : mZip(filepath)
    , mIsOpen(false)
{
}


void GHvacDataFileIO::setFileName(const QString& filepath)
{
    mZip.setZipName(filepath);
}


bool GHvacDataFileIO::open()
{
    mIsOpen = mZip.open(QuaZip::mdUnzip);
    if (!mIsOpen) {
        return (false);
    }
    mFilesList = mZip.getFileNameList();
    for (const QString& f : mFilesList)
    {
        qDebug() << f;
        if (f.toLower() == "system.csv") {
            mSys = std::make_shared<TableType>();
            mZip.setCurrentFile(f);
            QuaZipFile archived(&mZip);
            if (!archived.open(QIODevice::ReadOnly, nullptr)) {
                return (false);
            }
            QTextStream s(&archived);
            s.setCodec("UTF-8");
            SACsvStream csv(&s);
            QStringList header = csv.readCsvLine();
            int dt1 = header.indexOf(QStringLiteral("记录时间"));
            int dt2 = header.indexOf(QStringLiteral("接收时间"));
            mSys->setRowNames(header);
            QVector<double> rd;
            while (!csv.atEnd())
            {
                QStringList line = csv.readCsvLine();
                converStringListToDoubleList(line, rd);
                if (dt1 >= 0) {
                    rd[dt1] = QDateTime::fromString(line[dt1], "yyyy-MM-dd HH:mm:ss").toMSecsSinceEpoch();
                }
                if (dt2 >= 0) {
                    rd[dt2] = QDateTime::fromString(line[dt2], "yyyy-MM-dd HH:mm:ss").toMSecsSinceEpoch();
                }
                mSys->appendColumn(rd.begin(), rd.end());
            }
            qDebug() << *mSys;
        }
    }
    return (true);
}


GHvacDataFileIO::Error GHvacDataFileIO::getError() const
{
    int err = mZip.getZipError();

    switch (err)
    {
    case ZIP_PARAMERROR:
        return (ParamError);

    case ZIP_BADZIPFILE:
        return (BadDataFileError);

    case ZIP_INTERNALERROR:
        return (InternalError);

    default:
        break;
    }
    return (UnknowError);
}


void GHvacDataFileIO::converStringListToDoubleList(const QStringList& str, QVector<double>& d)
{
    if (d.size() != str.size()) {
        d.resize(str.size());
    }
    int s = str.size();
    double data = 0;
    bool isOK = false;

    for (int i = 0; i < s; ++i)
    {
        data = str[i].toDouble(&isOK);
        if (!isOK) {
            data = std::nan("");
        }
        d[i] = data;
    }
}
