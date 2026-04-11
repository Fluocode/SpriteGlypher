#include "StrokeEffectSettingsPanel.h"
#include "ui_StrokeEffectSettingsPanel.h"

StrokeEffectSettingsPanel::StrokeEffectSettingsPanel(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::StrokeEffectSettingsPanel)
{
    ui->setupUi(this);

    ui->rowOpacity->configureDouble(0.0, 1.0, 2, 0.05);
    ui->rowWidth->configureInt(0, 256, 1);

    // enabled, type, color, gradient
    ui->comboBoxBlend->addItems(SGFEffectTypes::BlendModeOptions());
    ui->segStrokePosition->setOptions(SGFEffectTypes::StrokePositionOptions());

    QObject::connect(ui->comboBoxBlend, SIGNAL(currentIndexChanged(int)), this, SLOT(subWidgetValueChanged()));
    QObject::connect(ui->rowOpacity, SIGNAL(valueChanged()), this, SLOT(subWidgetValueChanged()));
    QObject::connect(ui->segStrokePosition, SIGNAL(currentIndexChanged(int)), this, SLOT(subWidgetValueChanged()));
    QObject::connect(ui->colorSwatch, SIGNAL(colorValueChanged(QColor)), this, SLOT(subWidgetValueChanged()));
    QObject::connect(ui->rowWidth, SIGNAL(valueChanged()), this, SLOT(subWidgetValueChanged()));
}

StrokeEffectSettingsPanel::~StrokeEffectSettingsPanel()
{
    delete ui;
}


void StrokeEffectSettingsPanel::setValue(const SGFStrokeEffectSettings & value)
{
    mValue = value;
    mIsUpdatingGui = true;

    ui->comboBoxBlend->setCurrentText(SGFEffectTypes::BlendModeToString(mValue.blendMode));
    ui->rowOpacity->setDoubleValue(mValue.opacity);
    ui->segStrokePosition->setCurrentText(SGFEffectTypes::StrokePositionToString(mValue.position));
    ui->colorSwatch->setColorValue(mValue.color);
    ui->rowWidth->setIntValue(mValue.width);

    mIsUpdatingGui = false;
}


SGFStrokeEffectSettings StrokeEffectSettingsPanel::getValue() const
{
    return mValue;
}


void StrokeEffectSettingsPanel::subWidgetValueChanged()
{
    if ( mIsUpdatingGui ) {
        return;
    }

    mValue.blendMode = SGFEffectTypes::BlendModeFromString(ui->comboBoxBlend->currentText());
    mValue.opacity = ui->rowOpacity->doubleValue();
    mValue.position = SGFEffectTypes::StrokePositionFromString(ui->segStrokePosition->currentText());
    mValue.color = ui->colorSwatch->colorValue();
    mValue.width = ui->rowWidth->intValue();

    emit valueChanged(mValue);
}
