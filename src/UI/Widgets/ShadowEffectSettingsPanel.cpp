#include "ShadowEffectSettingsPanel.h"
#include "ui_ShadowEffectSettingsPanel.h"

ShadowEffectSettingsPanel::ShadowEffectSettingsPanel(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ShadowEffectSettingsPanel)
{
    ui->setupUi(this);

    ui->rowOpacity->configureDouble(0.0, 1.0, 2, 0.05);
    ui->rowAngle->configureInt(0, 360, 1);
    ui->rowDistance->configureInt(0, 512, 1);
    ui->rowBlur->configureInt(0, 256, 1);
    ui->rowSize->configureInt(0, 512, 1);

    // enabled, type, color, gradient
    ui->comboBoxBlend->addItems(SGFEffectTypes::BlendModeOptions());
    ui->segShadowType->setOptions(SGFEffectTypes::ShadowTypeOptions());

    QObject::connect(ui->comboBoxBlend, SIGNAL(currentIndexChanged(int)), this, SLOT(subWidgetValueChanged()));
    QObject::connect(ui->rowOpacity, SIGNAL(valueChanged()), this, SLOT(subWidgetValueChanged()));
    QObject::connect(ui->segShadowType, SIGNAL(currentIndexChanged(int)), this, SLOT(subWidgetValueChanged()));
    QObject::connect(ui->colorSwatch, SIGNAL(colorValueChanged(QColor)), this, SLOT(subWidgetValueChanged()));
    QObject::connect(ui->rowAngle, SIGNAL(valueChanged()), this, SLOT(subWidgetValueChanged()));
    QObject::connect(ui->rowDistance, SIGNAL(valueChanged()), this, SLOT(subWidgetValueChanged()));
    QObject::connect(ui->rowBlur, SIGNAL(valueChanged()), this, SLOT(subWidgetValueChanged()));
    QObject::connect(ui->rowSize, SIGNAL(valueChanged()), this, SLOT(subWidgetValueChanged()));
}

ShadowEffectSettingsPanel::~ShadowEffectSettingsPanel()
{
    delete ui;
}


void ShadowEffectSettingsPanel::setValue(const SGFShadowEffectSettings & value)
{
    mValue = value;
    mIsUpdatingGui = true;

    ui->comboBoxBlend->setCurrentText(SGFEffectTypes::BlendModeToString(mValue.blendMode));
    ui->rowOpacity->setDoubleValue(mValue.opacity);
    ui->segShadowType->setCurrentText(SGFEffectTypes::ShadowTypeToString(mValue.shadowType));
    ui->colorSwatch->setColorValue(mValue.color);
    ui->rowAngle->setIntValue(mValue.angle);
    ui->rowDistance->setIntValue(mValue.distance);
    ui->rowBlur->setIntValue(mValue.blur);
    ui->rowSize->setIntValue(mValue.size);

    mIsUpdatingGui = false;
}


SGFShadowEffectSettings ShadowEffectSettingsPanel::getValue() const
{
    return mValue;
}


void ShadowEffectSettingsPanel::subWidgetValueChanged()
{
    if ( mIsUpdatingGui ) {
        return;
    }

    mValue.blendMode = SGFEffectTypes::BlendModeFromString(ui->comboBoxBlend->currentText());
    mValue.opacity = ui->rowOpacity->doubleValue();
    mValue.shadowType = SGFEffectTypes::ShadowTypeFromString(ui->segShadowType->currentText());
    mValue.color = ui->colorSwatch->colorValue();
    mValue.angle = ui->rowAngle->intValue();
    mValue.distance = ui->rowDistance->intValue();
    mValue.blur = ui->rowBlur->intValue();
    mValue.size = ui->rowSize->intValue();

    emit valueChanged(mValue);
}
