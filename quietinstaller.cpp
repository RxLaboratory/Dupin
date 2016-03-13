#include "quietinstaller.h"

QuietInstaller::QuietInstaller(QObject *parent) : QObject(parent)
{
    install = new InstallPseudoEffects();
    connect(install,SIGNAL(log(QString)),this,SLOT(log(QString)));
    connect(install,SIGNAL(finished(bool)),this,SLOT(finished(bool)));

    bool ok = getAeVersions();

    if (ok)
    {
        ok = getPackages();
    }
    else
    {
        log("Can't find After Effects");
        exit(-11);
    }

    if (ok)
    {
        install->run();
    }
    else
    {
        log("Can't find any package to install");
        exit(-12);
    }
}

bool QuietInstaller::getAeVersions()
{
    //get aeversions
    QList<AECheckBox *> aeVersions = PreInstall::findAE();
    if (aeVersions.count() == 0) return false;

    foreach(AECheckBox *v,aeVersions)
    {
        QStringList version;
        version << v->getPresetEffects() << v->getScriptUI() << v->getVersion();
        install->addVersion(version);
    }

    return true;
}

bool QuietInstaller::getPackages()
{
    //load packages
    QDomNodeList dupinList = PreInstall::loadPackages();
    if (dupinList.count() == 0) return false;

    for (int i =0;i<dupinList.length();i++)
    {
        QDomNode item = dupinList.item(i);
        if (!item.isElement()) continue;
        QDomElement itemElt = item.toElement();

        if (itemElt.tagName() == "package")
        {
            QDomNodeList packageElts = itemElt.childNodes();
            QString packagePath = PreInstall::getPackagePath();

            QString packageName = "";

            for (int j =0;j<dupinList.length();j++)
            {
                QDomNode packageItem = packageElts.item(j);
                if (!packageItem.isElement()) continue;
                QDomElement packageElt = packageItem.toElement();

                if (packageElt.tagName() == "name") packageName = packageElt.text();
            }

            if (packageName == "") continue;

            QDir packageDir(packagePath + packageName);
            if (!packageDir.exists())
            {
                continue;
            }

            install->addPackage(packageName);
        }
    }

    return true;
}

void QuietInstaller::log(QString l)
{
    qDebug() << l;
}

void QuietInstaller::finished(bool ok)
{
    if (ok)
    {
        log("\n# Duik successfully installed.\nMay the gods of animation be with you!");
        exit(0);
    }
    else
    {
        log("\n# Installation complete with some errors. See stderr for more information.\nDo you have administrator privileges?\nYou may need to manually install Duik.\nGo to http://www.duduf.net to get help.");
        exit(-2);
    }


}


QTextStream& QuietInstaller::qStdOut()
{
    static QTextStream ts( stdout );
    return ts;
}
