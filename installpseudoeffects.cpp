#include "installpseudoeffects.h"
#include "qfichier.h"
#include <QDirIterator>
#include <QtDebug>
#include <QApplication>

InstallPseudoEffects::InstallPseudoEffects(QObject *parent): QThread(parent)
{

}

void InstallPseudoEffects::run()
{
    bool ok = true;

    foreach(QStringList version, versions)
    {
        QString ver = version[2];
        emit log("\n\n" + QTime::currentTime().toString("[hh:mm:ss]") + " " + ver);

        //update presetEffects.xml
        if (!updatePresetEffects(version[0]))
        {
            ok = false;
            emit log("\n========= WARNING ===========");
            emit log("The pseudo effects needed could not be automatically installed in " + ver);
        }

        //add scriptUI files
        if (!updateSui(version[1]))
        {
            ok = false;
            emit log("\n========= WARNING ===========");
            emit log("ScriptUI Files could not be automatically copied to " + ver);
        }
    }

    emit finished(ok);
}

bool InstallPseudoEffects::updatePresetEffects(QString presetsFileName)
{   
    //loading old presetEffects, backuping

     emit log("\n\n# " + QTime::currentTime().toString("[hh:mm:ss]") + " Updating " + presetsFileName);
     emit log(QTime::currentTime().toString("[hh:mm:ss]") + " Backuping 'PresetEffects.xml' to 'PresetEffects.xml.bak");
     QFichier xmlDestination(presetsFileName);
     if (!xmlDestination.exists()) return false;
     //backup file
     QFichier backupFile(presetsFileName + ".bak");
     if (backupFile.exists()) backupFile.remove();
     QFile::copy(presetsFileName,presetsFileName + ".bak");
     if (!backupFile.exists()) return false;

    //update preset Effects
     foreach(QString pe,peFiles)
     {
        //get package name
         QString packageName = "";
         QStringList f = pe.split("/");
         if (f.length() >= 2)
         {
             packageName = f[f.length()-2];
         }
         if (packageName == "") continue;

         emit log(QTime::currentTime().toString("[hh:mm:ss]") + " Searching for older version of " + packageName);
         //find Duik, if exists, remove
         QStringList xml = xmlDestination.getLines(false);
         bool inDuik = false;
         for(int i = xml.size()-1;i>=0;i--)
         {
             QString line = xml[i];
             if (line.trimmed().startsWith("<!-- END " + packageName.toUpper()))
             {
                 inDuik = true;
             }
             if (inDuik) xml.removeAt(i);
             if (line.trimmed().startsWith("<!-- BEGIN " + packageName.toUpper()))
             {
                 inDuik = false;
             }
         }

          emit log(QTime::currentTime().toString("[hh:mm:ss]") + " Loading new version of " + packageName);
         //load Duik xml </effects>
         QFichier xmlSource(pe);
         QStringList xmlDuik = xmlSource.getLines();

         emit log(QTime::currentTime().toString("[hh:mm:ss]") + " Writing new 'PresetEffects.xml'");
         //write new file
         if (!xmlDestination.open(QIODevice::WriteOnly | QIODevice::Text))
         {
             emit log(QTime::currentTime().toString("[hh:mm:ss]") + " Cannot open 'PresetEffects.xml' for writing");
             return false;
         }
         QTextStream out(&xmlDestination);
         QString nextLine = "";
         for(int i = 0 ; i < xml.size();i++)
         {
             QString line = xml[i];
             if (i < xml.size()-1) nextLine = xml[i+1];
             else nextLine = "";
             out << line << endl;
             if (nextLine.trimmed().toLower().startsWith("</effects>"))
             {
                 foreach(QString duikLine,xmlDuik)
                 {
                     out << duikLine << endl;
                 }
             }
         }
         xmlDestination.close();
     }


    emit log(QTime::currentTime().toString("[hh:mm:ss]") + " 'PresetEffects.xml' succesfully patched");

    return true;
}

bool InstallPseudoEffects::updateSui(QString scriptUIpath)
{
    emit log("\n\n# " + QTime::currentTime().toString("[hh:mm:ss]") + " Updating ScriptUI Files: " + scriptUIpath);
    //remove existing Duik
    foreach(QString suiF,suiFiles)
    {
        QStringList f = suiF.split("/");
        QString name = "";
        if (f.length() >= 4)
        {
            name = f[f.length()-1];
        }
        if (name == "") continue;
        QFile old(scriptUIpath + "/" + name);
        if (old.exists()) old.remove();
        QFile newFile(suiF);

        emit log(QTime::currentTime().toString("[hh:mm:ss]") + " Copying " + name);

        newFile.copy(scriptUIpath + "/" + name);

    }
    return true;
}

void InstallPseudoEffects::addVersion(QStringList v)
{
    versions  << v;
}

void InstallPseudoEffects::removeVersion(QStringList v)
{
    for(int i = versions.length()-1; i >= 0 ;i--)
    {
        QStringList ver = versions[i];
        if (ver[0] == v[0])
        {
            versions.removeAt(i);
        }
    }
}

bool InstallPseudoEffects::hasVersion()
{
    if (versions.length() > 0) return true;
    else return false;
}

void InstallPseudoEffects::addPackage(QString n)
{
    QString packagePath = "packages/";
#ifdef Q_OS_MAC
    QDir bundle = QApplication::applicationDirPath();
    bundle.cdUp();
    packagePath = bundle.path() + "/Resources/packages/";
#endif


    //add files
    QDir scriptUIDir(packagePath + n + "/ScriptUI Panels");
    QStringList scriptUIFiles = scriptUIDir.entryList(QDir::Files);
    foreach(QString f,scriptUIFiles)
    {
        suiFiles << packagePath + n + "/ScriptUI Panels/" + f;
    }
    QDir scriptsDir(packagePath + n + "/Scripts");
    QStringList scriptsFiles = scriptsDir.entryList(QDir::Files);
    foreach(QString f,scriptsFiles)
    {
        sFiles << packagePath + n + "/Scripts/" + f;
    }
    QFile presetEffects(packagePath + n + "/PresetEffects.xml");
    if (presetEffects.exists())
    {
        peFiles << packagePath + n + "/PresetEffects.xml";
    }
}

void InstallPseudoEffects::removePackage(QString n)
{
    //remove files
    for(int i = suiFiles.length()-1 ; i >= 0 ; i--)
    {
        QStringList f = suiFiles[i].split("/");
        if (f.length() >= 2)
        {
            if (f[f.length()-3] == n) suiFiles.removeAt(i);
        }
    }

    for(int i = sFiles.length()-1 ; i >= 0 ; i--)
    {
        QStringList f = sFiles[i].split("/");
        if (f.length() >= 2)
        {
            if (f[f.length()-3] == n) sFiles.removeAt(i);
        }
    }

    for(int i = peFiles.length()-1 ; i >= 0 ; i--)
    {
        QStringList f = peFiles[i].split("/");
        if (f.length() >= 2)
        {
            if (f[f.length()-2] == n) peFiles.removeAt(i);
        }
    }

}
