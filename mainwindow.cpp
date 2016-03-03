#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
    setupUi(this);

    stackedWidget->setCurrentIndex(1);
    previousButton->hide();
    packageCount = 0;

    install = new InstallPseudoEffects();
    connect(install,SIGNAL(log(QString)),this,SLOT(log(QString)));
    connect(install,SIGNAL(finished(bool)),this,SLOT(finished(bool)));

    if (!loadPackages())
    {
        stackedWidget->setEnabled(false);
        installButton->setEnabled(false);
    }

    findAe();

}

void MainWindow::findAe()
{
    bool found = false;
#ifdef Q_OS_WIN
    found = findAeVersions("C:\\Program Files\\Adobe\\");
    bool found2 = findAeVersions("C:\\Program Files (x86)\\Adobe\\");
    if (!found) found = found2;
#endif
#ifdef Q_OS_MAC
    found = findAeVersions("/Applications");
#endif

    if (!found)
    {
        aeVersionsLabel->setText("The installer could not find any valid installation of After Effects.\nPlease select the folder where it is installed.");
        //add custom path
        CustomPathWidget *cpw = new CustomPathWidget;
        versionsLayout->addWidget(cpw);
        connect(cpw,SIGNAL(chosen(QString,QString,QString,bool)),this,SLOT(aeCheckBox_clicked(QString,QString,QString,bool)));
    }
    versionsLayout->addStretch();
}

bool MainWindow::loadPackages()
{
    QString packagePath = "packages/";
#ifdef Q_OS_MAC
    QDir bundle = QApplication::applicationDirPath();
    bundle.cdUp();
    packagePath = bundle.path() + "/Resources/packages/";
#endif

    QFile configFile(packagePath + "config.xml");
    if (!configFile.exists())
    {
        QMessageBox::critical(this,"Configuration file not found","Warning: could not find the configuration file needed to install the packages.");
        QMessageBox::critical(this,"Configuration file not found",configFile.fileName());
        return false;
    }

    //load images
    programIconLabel->setPixmap(QPixmap(packagePath + "image.png"));
    this->setWindowIcon(QIcon(packagePath + "icon.png"));

    //parse XML config
    QDomDocument configDoc;
    QDomElement  configDocElement;
    configFile.open(QIODevice::ReadOnly);
    if (!configDoc.setContent(configFile.readAll()))
    {
        QMessageBox::critical(this,"Invalid configuration file","Warning: cannot read configuration file. Make sure it is a valid XML Document.");
        configFile.close();
        return false;
    }
    configFile.close();


    //load XML
    configDocElement = configDoc.documentElement();
    QDomElement dupinElt = configDocElement.firstChild().toElement();
    QDomNodeList dupinList = dupinElt.childNodes();

    //get config
    QString softwareName = "";
    QString softwareDescription = "";
    QString softwareVersion = "";
    for (int i =0;i<dupinList.length();i++)
    {
        QDomNode item = dupinList.item(i);
        if (!item.isElement()) continue;
        QDomElement itemElt = item.toElement();

        if (itemElt.tagName() == "name") softwareName = itemElt.text();
        else if (itemElt.tagName() == "description") softwareDescription = itemElt.text();
        else if (itemElt.tagName() == "version") softwareVersion = itemElt.text();

        else if (itemElt.tagName() == "package")
        {
            QDomNodeList packageElts = itemElt.childNodes();

            QString packageName = "";
            QString packageDescription = "";
            QString packageVersion = "";

            for (int j =0;j<dupinList.length();j++)
            {
                QDomNode packageItem = packageElts.item(j);
                if (!packageItem.isElement()) continue;
                QDomElement packageElt = packageItem.toElement();

                if (packageElt.tagName() == "name") packageName = packageElt.text();
                else if (packageElt.tagName() == "description") packageDescription = packageElt.text();
                else if (packageElt.tagName() == "version") packageVersion = packageElt.text();
            }

            if (packageName == "") continue;

            QDir packageDir(packagePath + packageName);
            if (!packageDir.exists())
            {
                QMessageBox::warning(this,"Package not found","Warning: could not find files from package " + packageName + ".");
                return false;
            }
            PackageSelector *ps = new PackageSelector();
            ps->setName(packageName);
            ps->setDescription(packageDescription);
            ps->setVersion(packageVersion);
            connect(ps,SIGNAL(select(bool,QString)),this,SLOT(packageCheckBox_clicked(bool,QString)));

            install->addPackage(packageName);

            packagesLayout->addWidget(ps);
            packageCount = packageCount + 1;
        }
    }

    packagesLayout->addStretch();


    if (packageCount == 0)
    {
        QMessageBox::warning(this,"No package found","Cannot find any installable package.");
        return false;
    }

    //update UI
    topLabel->setText(topLabel->text().arg(softwareName));
    welcomeLabel->setText(welcomeLabel->text().arg(softwareName,softwareDescription));
    versionLabel->setText(softwareVersion);

    return true;
}

bool MainWindow::findAeVersions(QString dir)
{
    QStringList filters("Adobe After Effects *");

    QDir adobeDir(dir);

    adobeDir.setNameFilters(filters);
    adobeDir.setFilter(QDir::Dirs);

    QFileInfoList versionPaths = adobeDir.entryInfoList();

    bool found = false;

    foreach(QFileInfo path,versionPaths)
    {
        QDir aePath(path.absoluteFilePath());

        #ifdef Q_OS_WIN
        QFile presetEffectsFile(path.absoluteFilePath() + "/Support Files/PresetEffects.xml");
        QDir scriptUI(path.absoluteFilePath() + "/Support Files/Scripts/ScriptUI Panels/");
        #endif
        #ifdef Q_OS_MAC
        QFile presetEffectsFile(aePath.absolutePath() + "/" + aePath.dirName() + ".app/Contents/Resources/PresetEffects.xml");
        QDir scriptUI(path.absoluteFilePath() + "/Scripts/ScriptUI Panels/");
        #endif

        if (presetEffectsFile.exists() && scriptUI.exists())
        {
            AECheckBox *aeButton = new AECheckBox(aePath.dirName());
            aeButton->setVersion(aePath.dirName());
            aeButton->setPresetEffects(presetEffectsFile.fileName());
            aeButton->setScriptUI(scriptUI.absolutePath());
            versionsLayout->addWidget(aeButton);
            connect(aeButton,SIGNAL(chosen(QString,QString,QString,bool)),this,SLOT(aeCheckBox_clicked(QString,QString,QString,bool)));
            found = true;
        }
    }
    return found;
}

void MainWindow::on_installButton_clicked()
{
    //home
    if (stackedWidget->currentIndex() == 0)
    {
        stackedWidget->setCurrentIndex(1);
        installButton->setEnabled(false);
        previousButton->show();
        if (packageCount == 1) installButton->setText("Install");
    }
    //ae version
    else if (stackedWidget->currentIndex() == 1)
    {
        previousButton->show();
        if (packageCount == 1) launchInstallation();
        else
        {
            installButton->setText("Install");
            stackedWidget->setCurrentIndex(2);
        }
    }
    //package selection
    else if (stackedWidget->currentIndex() == 2)
    {
        launchInstallation();
    }
}

void MainWindow::launchInstallation()
{
    previousButton->hide();
    installButton->hide();
    cancelButton->setEnabled(false);
    stackedWidget->setCurrentIndex(3);
    ok = true;
    log("# Beginning installation");
    install->run();
}

void MainWindow::on_cancelButton_clicked()
{
    qApp->quit();
}

void MainWindow::aeCheckBox_clicked(QString pe, QString s, QString ver, bool checked)
{
    QStringList v;
    v << pe << s << ver;
    if (checked)
    {
        install->addVersion(v);
    }
    else
    {
        install->removeVersion(v);
    }
    installButton->setEnabled(install->hasVersion());
}

void MainWindow::log(QString l)
{
    logEdit->setText(logEdit->toPlainText() + l + "\n");
    logEdit->verticalScrollBar()->setValue( logEdit->verticalScrollBar()->maximum() );
    this->repaint();
    qApp->processEvents();
}

void MainWindow::finished(bool o)
{
    if (o)
    {
        log("\n# Duik successfully installed.\nMay the gods of animation be with you!");
    }
    else
    {
        log("\n# Installation complete with some errors. See this log for more information.\nYou may need to manually install Duik.\nGo to http://www.duduf.net to get help.");
    }

    cancelButton->setText("Close");
    cancelButton->setEnabled(true);
}

void MainWindow::on_previousButton_clicked()
{
    //ae version
    if (stackedWidget->currentIndex() == 1)
    {
       stackedWidget->setCurrentIndex(0);
        installButton->setText("Next...");
        installButton->setEnabled(true);
        previousButton->hide();
    }
    //package selection
    else if (stackedWidget->currentIndex() == 2)
    {
        stackedWidget->setCurrentIndex(1);
        installButton->setText("Next...");
        previousButton->hide();
    }
}

void MainWindow::packageCheckBox_clicked(bool checked,QString name)
{
    if (checked) install->addPackage(name);
    else install->removePackage(name);
}
