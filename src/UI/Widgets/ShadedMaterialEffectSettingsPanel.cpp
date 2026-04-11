#include "ShadedMaterialEffectSettingsPanel.h"
#include "ui_ShadedMaterialEffectSettingsPanel.h"

#include <cmath>

#include <QFileDialog>
#include <QFileInfo>
#include <QFontMetrics>

ShadedMaterialEffectSettingsPanel::ShadedMaterialEffectSettingsPanel(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ShadedMaterialEffectSettingsPanel)
{
    ui->setupUi(this);

    ui->rowOpacity->configureDouble(0.0, 1.0, 2, 0.05);
    ui->rowLightAngle->configureInt(0, 360, 1);
    ui->rowElevation->configureDouble(5.0, 89.0, 1, 1.0);
    ui->rowBump->configureDouble(0.2, 24.0, 2, 0.1);
    ui->rowAmbient->configureDouble(0.0, 1.0, 2, 0.02);
    ui->rowSmooth->configureInt(0, 8, 1);
    ui->rowInteriorRelief->configureDouble(0.0, 1.0, 2, 0.02);
    ui->rowInflate->configureDouble(0.0, 1.0, 2, 0.02);
    ui->rowSpecular->configureDouble(0.0, 1.0, 2, 0.02);
    ui->rowShininess->configureDouble(4.0, 256.0, 0, 2.0);
    ui->rowMatcapMix->configureDouble(0.0, 1.0, 2, 0.02);

    ui->comboBoxBlend->addItems(SGFEffectTypes::BlendModeOptions());

    QObject::connect(ui->comboBoxBlend, SIGNAL(currentIndexChanged(int)), this, SLOT(subWidgetValueChanged()));
    QObject::connect(ui->rowOpacity, SIGNAL(valueChanged()), this, SLOT(subWidgetValueChanged()));
    QObject::connect(ui->rowLightAngle, SIGNAL(valueChanged()), this, SLOT(subWidgetValueChanged()));
    QObject::connect(ui->rowElevation, SIGNAL(valueChanged()), this, SLOT(subWidgetValueChanged()));
    QObject::connect(ui->rowBump, SIGNAL(valueChanged()), this, SLOT(subWidgetValueChanged()));
    QObject::connect(ui->rowAmbient, SIGNAL(valueChanged()), this, SLOT(subWidgetValueChanged()));
    QObject::connect(ui->rowSmooth, SIGNAL(valueChanged()), this, SLOT(subWidgetValueChanged()));
    QObject::connect(ui->colorLight, SIGNAL(colorValueChanged(QColor)), this, SLOT(subWidgetValueChanged()));
    QObject::connect(ui->colorFlatBase, SIGNAL(colorValueChanged(QColor)), this, SLOT(subWidgetValueChanged()));
    QObject::connect(ui->checkFlatOnly, SIGNAL(toggled(bool)), this, SLOT(subWidgetValueChanged()));
    QObject::connect(ui->rowInteriorRelief, SIGNAL(valueChanged()), this, SLOT(subWidgetValueChanged()));
    QObject::connect(ui->rowInflate, SIGNAL(valueChanged()), this, SLOT(subWidgetValueChanged()));
    QObject::connect(ui->rowSpecular, SIGNAL(valueChanged()), this, SLOT(subWidgetValueChanged()));
    QObject::connect(ui->rowShininess, SIGNAL(valueChanged()), this, SLOT(subWidgetValueChanged()));
    QObject::connect(ui->rowMatcapMix, SIGNAL(valueChanged()), this, SLOT(subWidgetValueChanged()));
    QObject::connect(ui->buttonMatcapBrowse, SIGNAL(clicked(bool)), this, SLOT(onMatcapBrowse()));
    QObject::connect(ui->buttonMatcapClear, SIGNAL(clicked(bool)), this, SLOT(onMatcapUseDefault()));
    QObject::connect(ui->buttonPresetMatte, SIGNAL(clicked(bool)), this, SLOT(onPresetMatte()));
    QObject::connect(ui->buttonPresetGlossy, SIGNAL(clicked(bool)), this, SLOT(onPresetGlossy()));
    QObject::connect(ui->buttonPresetChrome, SIGNAL(clicked(bool)), this, SLOT(onPresetChrome()));
}

ShadedMaterialEffectSettingsPanel::~ShadedMaterialEffectSettingsPanel()
{
    delete ui;
}

void ShadedMaterialEffectSettingsPanel::setValue(const SGFShadedMaterialEffectSettings &value)
{
    mValue = value;
    mIsUpdatingGui = true;

    ui->comboBoxBlend->setCurrentText(SGFEffectTypes::BlendModeToString(mValue.blendMode));
    ui->rowOpacity->setDoubleValue(mValue.opacity);
    ui->rowLightAngle->setIntValue(static_cast<int>(std::lround(mValue.lightAngle)));
    ui->rowElevation->setDoubleValue(mValue.lightElevation);
    ui->rowBump->setDoubleValue(mValue.bumpScale);
    ui->rowAmbient->setDoubleValue(mValue.ambient);
    ui->rowSmooth->setIntValue(mValue.smoothRadius);
    ui->rowInteriorRelief->setDoubleValue(mValue.interiorRelief);
    ui->rowInflate->setDoubleValue(mValue.inflateAmount);
    ui->rowSpecular->setDoubleValue(mValue.specularStrength);
    ui->rowShininess->setDoubleValue(mValue.specularPower);
    ui->colorLight->setColorValue(mValue.lightColor);
    ui->colorFlatBase->setColorValue(mValue.flatBaseColor);
    ui->checkFlatOnly->setChecked(mValue.forceFlatBase);
    ui->rowMatcapMix->setDoubleValue(mValue.matcapMix);
    refreshMatcapPathLabel();

    mIsUpdatingGui = false;
}

SGFShadedMaterialEffectSettings ShadedMaterialEffectSettingsPanel::getValue() const
{
    if ( !mIsUpdatingGui ) {
        const_cast<ShadedMaterialEffectSettingsPanel*>(this)->pullFromWidgets();
    }
    return mValue;
}

void ShadedMaterialEffectSettingsPanel::pullFromWidgets()
{
    mValue.blendMode = SGFEffectTypes::BlendModeFromString(ui->comboBoxBlend->currentText());
    mValue.opacity = ui->rowOpacity->doubleValue();
    mValue.lightAngle = static_cast<float>(ui->rowLightAngle->intValue());
    mValue.lightElevation = static_cast<float>(ui->rowElevation->doubleValue());
    mValue.bumpScale = static_cast<float>(ui->rowBump->doubleValue());
    mValue.ambient = static_cast<float>(ui->rowAmbient->doubleValue());
    mValue.smoothRadius = ui->rowSmooth->intValue();
    mValue.interiorRelief = static_cast<float>(ui->rowInteriorRelief->doubleValue());
    mValue.inflateAmount = static_cast<float>(ui->rowInflate->doubleValue());
    mValue.specularStrength = static_cast<float>(ui->rowSpecular->doubleValue());
    mValue.specularPower = static_cast<float>(ui->rowShininess->doubleValue());
    mValue.lightColor = ui->colorLight->colorValue();
    mValue.flatBaseColor = ui->colorFlatBase->colorValue();
    mValue.forceFlatBase = ui->checkFlatOnly->isChecked();
    mValue.matcapMix = static_cast<float>(ui->rowMatcapMix->doubleValue());
}

void ShadedMaterialEffectSettingsPanel::refreshMatcapPathLabel()
{
    const QFontMetrics fm(ui->labelMatcapPath->font());
    const int w = qMax(24, ui->labelMatcapPath->maximumWidth() - 2);

    if ( mValue.matcapImagePath.trimmed().isEmpty() ) {
        ui->labelMatcapPath->setText(tr("Built-in"));
        ui->labelMatcapPath->setToolTip(QString());
        return;
    }

    const QString base = QFileInfo(mValue.matcapImagePath).fileName();
    ui->labelMatcapPath->setText(fm.elidedText(base, Qt::ElideMiddle, w));
    ui->labelMatcapPath->setToolTip(mValue.matcapImagePath);
}

void ShadedMaterialEffectSettingsPanel::onMatcapBrowse()
{
    if ( mIsUpdatingGui ) {
        return;
    }
    pullFromWidgets();
    const QString path = QFileDialog::getOpenFileName(
        this,
        tr("Open matcap image"),
        QString(),
        tr("Images (*.png *.jpg *.jpeg *.bmp);;All files (*)"));
    if ( path.isEmpty() ) {
        return;
    }
    mValue.matcapImagePath = path;
    refreshMatcapPathLabel();
    emit valueChanged(mValue);
}

void ShadedMaterialEffectSettingsPanel::onMatcapUseDefault()
{
    if ( mIsUpdatingGui ) {
        return;
    }
    pullFromWidgets();
    mValue.matcapImagePath.clear();
    refreshMatcapPathLabel();
    emit valueChanged(mValue);
}

void ShadedMaterialEffectSettingsPanel::subWidgetValueChanged()
{
    if ( mIsUpdatingGui ) {
        return;
    }

    pullFromWidgets();
    emit valueChanged(mValue);
}

void ShadedMaterialEffectSettingsPanel::onPresetMatte()
{
    if ( mIsUpdatingGui ) {
        return;
    }
    pullFromWidgets();
    mValue.interiorRelief = 0.45f;
    mValue.ambient = 0.48f;
    mValue.specularStrength = 0.06f;
    mValue.specularPower = 18.f;
    mValue.bumpScale = 3.2f;
    mValue.smoothRadius = 2;
    setValue(mValue);
    emit valueChanged(mValue);
}

void ShadedMaterialEffectSettingsPanel::onPresetGlossy()
{
    if ( mIsUpdatingGui ) {
        return;
    }
    pullFromWidgets();
    mValue.interiorRelief = 0.85f;
    mValue.inflateAmount = 0.52f;
    mValue.ambient = 0.2f;
    mValue.specularStrength = 0.42f;
    mValue.specularPower = 52.f;
    mValue.bumpScale = 5.5f;
    mValue.smoothRadius = 2;
    setValue(mValue);
    emit valueChanged(mValue);
}

void ShadedMaterialEffectSettingsPanel::onPresetChrome()
{
    if ( mIsUpdatingGui ) {
        return;
    }
    pullFromWidgets();
    mValue.interiorRelief = 0.94f;
    mValue.inflateAmount = 0.88f;
    mValue.ambient = 0.1f;
    mValue.specularStrength = 0.82f;
    mValue.specularPower = 92.f;
    mValue.bumpScale = 7.f;
    mValue.smoothRadius = 2;
    mValue.lightColor = QColor(255, 252, 245, 255);
    mValue.matcapMix = 0.48f;
    mValue.matcapImagePath.clear();
    setValue(mValue);
    emit valueChanged(mValue);
}
