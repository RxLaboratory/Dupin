#ifndef PREINSTALL_H
#define PREINSTALL_H

#include <QObject>

class PreInstall : public QObject
{
    Q_OBJECT
public:
    explicit PreInstall(QObject *parent = 0);
    static QStringList findAE();
    QStringList findAEVersions(QString dir);

signals:

public slots:

private:
    QStringList findAEVersions(QString dir);
};

#endif // PREINSTALL_H
