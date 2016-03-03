#include "packageselector.h"

PackageSelector::PackageSelector(QWidget *parent) :
    QWidget(parent)
{
    setupUi(this);

    scriptUIFiles = QStringList();
    scriptsFiles = QStringList();
    presetEffects = "";
}

void PackageSelector::setName(QString n)
{
    nameLabel->setText(n);
}

void PackageSelector::setDescription(QString d)
{
    descriptionLabel->setText(d);
}

void PackageSelector::setVersion(QString v)
{
    versionLabel->setText(v);
}

void PackageSelector::on_checkBox_clicked(bool checked)
{
    emit select(checked,nameLabel->text());
}
