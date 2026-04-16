#include "OuterGlowEffectSettingsPanel.h"
#include "ui_OuterGlowEffectSettingsPanel.h"

OuterGlowEffectSettingsPanel::OuterGlowEffectSettingsPanel(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::OuterGlowEffectSettingsPanel)
{
    ui->setupUi(this);

    ui->rowOpacity->configureDouble(0.0, 1.0, 2, 0.05);
    ui->rowSize->configureInt(0, 512, 1);
    ui->rowBlur->configureInt(0, 256, 1);

    ui->comboBoxBlend->addItems(SGFEffectTypes::BlendModeOptions());

    QObject::connect(ui->comboBoxBlend, SIGNAL(currentIndexChanged(int)), this, SLOT(subWidgetValueChanged()));
    QObject::connect(ui->rowOpacity, SIGNAL(valueChanged()), this, SLOT(subWidgetValueChanged()));
    QObject::connect(ui->colorSwatch, SIGNAL(colorValueChanged(QColor)), this, SLOT(subWidgetValueChanged()));
    QObject::connect(ui->rowSize, SIGNAL(valueChanged()), this, SLOT(subWidgetValueChanged()));
    QObject::connect(ui->rowBlur, SIGNAL(valueChanged()), this, SLOT(subWidgetValueChanged()));
}

OuterGlowEffectSettingsPanel::~OuterGlowEffectSettingsPanel()
{
    delete ui;
}

void OuterGlowEffectSettingsPanel::setValue(const SGFOuterGlowEffectSettings &value)
{
    mValue = value;
    mIsUpdatingGui = true;
    ui->comboBoxBlend->setCurrentText(SGFEffectTypes::BlendModeToString(mValue.blendMode));
    ui->rowOpacity->setDoubleValue(mValue.opacity);
    ui->colorSwatch->setColorValue(mValue.color);
    ui->rowSize->setIntValue(static_cast<int>(mValue.size));
    ui->rowBlur->setIntValue(static_cast<int>(mValue.blur));
    mIsUpdatingGui = false;
}

SGFOuterGlowEffectSettings OuterGlowEffectSettingsPanel::getValue() const
{
    return mValue;
}

void OuterGlowEffectSettingsPanel::subWidgetValueChanged()
{
    if ( mIsUpdatingGui ) {
        return;
    }
    mValue.blendMode = SGFEffectTypes::BlendModeFromString(ui->comboBoxBlend->currentText());
    mValue.opacity = ui->rowOpacity->doubleValue();
    mValue.color = ui->colorSwatch->colorValue();
    mValue.size = static_cast<float>(ui->rowSize->intValue());
    mValue.blur = static_cast<float>(ui->rowBlur->intValue());
    emit valueChanged(mValue);
}

