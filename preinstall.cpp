#include "preinstall.h"


PreInstall::PreInstall(QObject *parent) : QObject(parent)
{

}

QList<AECheckBox *> PreInstall::findAE()
{
    QList<AECheckBox *> aeVersions;
#ifdef Q_OS_WIN
    aeVersions = findAeVersions("C:\\Program Files\\Adobe\\");
    aeVersions.append(findAeVersions("C:\\Program Files (x86)\\Adobe\\"));
#endif
#ifdef Q_OS_MAC
    aeVersions = findAeVersions("/Applications");
#endif

    return aeVersions;
}

QList<AECheckBox *> PreInstall::findAeVersions(QString dir)
{
    QStringList filters("Adobe After Effects *");

    QDir adobeDir(dir);

    adobeDir.setNameFilters(filters);
    adobeDir.setFilter(QDir::Dirs);

    QFileInfoList versionPaths = adobeDir.entryInfoList();

    QList<AECheckBox *> aeVersions;

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
            aeVersions << aeButton;
        }
    }
    return aeVersions;
}

QString PreInstall::getPackagePath()
{
    QString packagePath = "packages/";
#ifdef Q_OS_MAC
    QDir bundle = QApplication::applicationDirPath();
    bundle.cdUp();
    packagePath = bundle.path() + "/Resources/packages/";
#endif

    return packagePath;
}

QDomNodeList PreInstall::loadPackages()
{
    QDomNodeList dupinList;
    QString packagePath = getPackagePath();

    QFile configFile(packagePath + "config.xml");
    if (!configFile.exists())
    {
        return dupinList;
    }

    //parse XML config
    QDomDocument configDoc;
    QDomElement  configDocElement;
    configFile.open(QIODevice::ReadOnly);
    if (!configDoc.setContent(configFile.readAll()))
    {
        configFile.close();
        return dupinList;
    }
    configFile.close();


    //load XML
    configDocElement = configDoc.documentElement();
    QDomElement dupinElt = configDocElement.firstChild().toElement();
    dupinList = dupinElt.childNodes();

    return dupinList;
}