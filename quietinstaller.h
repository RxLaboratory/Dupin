#ifndef QUIETINSTALLER_H
#define QUIETINSTALLER_H

#include <QObject>
#include "preinstall.h"
#include <installpseudoeffects.h>
#include <stdio.h>
#include <QtDebug>

class QuietInstaller : public QObject
{
    Q_OBJECT
public:
    explicit QuietInstaller(QObject *parent = 0);

signals:

public slots:
    void log(QString);
    void finished(bool);


private:
    bool getAeVersions();
    bool getPackages();
    InstallPseudoEffects *install;
    QTextStream& qStdOut();
};

#endif // QUIETINSTALLER_H
