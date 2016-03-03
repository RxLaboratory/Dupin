#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "ui_mainwindow.h"
#include <QFileDialog>
#include "qfichier.h"
#include "aecheckbox.h"
#include <QtDebug>
#include <QStandardPaths>
#include <QCheckBox>
#include <QMessageBox>
#include <QDirIterator>
#include <installpseudoeffects.h>
#include <QScrollBar>
#include <QDomDocument>
#include "custompathwidget.h"
#include "packageselector.h"


class MainWindow : public QMainWindow, private Ui::MainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);

private slots:
    void on_installButton_clicked();
    void on_cancelButton_clicked();
    void aeCheckBox_clicked(QString pe, QString s, QString ver, bool checked);
    void log(QString l);
    void finished(bool o);
    void on_previousButton_clicked();
    void packageCheckBox_clicked(bool checked,QString name);

private:
    bool findAeVersions(QString dir);
    void findAe();
    bool loadPackages();
    void launchInstallation();
    bool updatePresetEffects(QString presetsFileName);
    bool updateDuik(QString scriptUIpath);
    bool ok;
    int packageCount;
    InstallPseudoEffects *install;
};

#endif // MAINWINDOW_H
