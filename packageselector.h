#ifndef PACKAGESELECTOR_H
#define PACKAGESELECTOR_H

#include "ui_packageselector.h"
#include <QFile>

class PackageSelector : public QWidget, private Ui::PackageSelector
{
    Q_OBJECT

public:
    explicit PackageSelector(QWidget *parent = 0);
    void setName(QString n);
    void setDescription(QString d);
    void setVersion(QString v);

signals:
    void select(bool,QString);

private slots:
    void on_checkBox_clicked(bool checked);

private:
    QStringList scriptUIFiles;
    QStringList scriptsFiles;
    QString presetEffects;
};

#endif // PACKAGESELECTOR_H
