#ifndef INSTALLPSEUDOEFFECTS_H
#define INSTALLPSEUDOEFFECTS_H

#include <QThread>
#include <QStringList>
#include <QTime>

class InstallPseudoEffects : public QThread
{
    Q_OBJECT
public:
    InstallPseudoEffects(QObject * parent = 0);
    void run();
    void addVersion(QStringList v);
    void removeVersion(QStringList v);
    bool hasVersion();
    void addPackage(QString n);
    void removePackage(QString n);

signals:
    void log(QString);
    void finished(bool);

public slots:

private:
    bool updateSui(QString scriptUIpath);
    bool updatePresetEffects(QString presetsFileName);
    QList<QStringList> versions;
    QStringList suiFiles;
    QStringList sFiles;
    QStringList peFiles;
};

#endif // INSTALLPSEUDOEFFECTS_H
