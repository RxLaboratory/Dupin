#ifndef PREINSTALL_H
#define PREINSTALL_H

#include <QObject>
#include <QDir>
#include "aecheckbox.h"
#include <QDomDocument>

class PreInstall : public QObject
{
    Q_OBJECT
public:
    explicit PreInstall(QObject *parent = 0);
    static QList<AECheckBox *> findAE();
    static QString getPackagePath();
    static QDomNodeList loadPackages();

signals:

public slots:

private:
    static QList<AECheckBox *> findAeVersions(QString dir);
};

#endif // PREINSTALL_H
