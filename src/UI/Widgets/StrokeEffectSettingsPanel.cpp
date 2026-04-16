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
    ui->segStrokeType->setOptions(SGFEffectTypes::FillTypeOptions());
    ui->segStrokePosition->setOptions(SGFEffectTypes::StrokePositionOptions());

    QObject::connect(ui->comboBoxBlend, SIGNAL(currentIndexChanged(int)), this, SLOT(subWidgetValueChanged()));
    QObject::connect(ui->rowOpacity, SIGNAL(valueChanged()), this, SLOT(subWidgetValueChanged()));
    QObject::connect(ui->segStrokeType, SIGNAL(currentIndexChanged(int)), this, SLOT(subWidgetValueChanged()));
    QObject::connect(ui->segStrokePosition, SIGNAL(currentIndexChanged(int)), this, SLOT(subWidgetValueChanged()));
    QObject::connect(ui->colorSwatch, SIGNAL(colorValueChanged(QColor)), this, SLOT(subWidgetValueChanged()));
    QObject::connect(ui->gradientSwatch, SIGNAL(gradientChanged(SGFGradient)), this, SLOT(subWidgetValueChanged()));
    QObject::connect(ui->patternSwatch, SIGNAL(patternValueChanged(SGFPattern)), this, SLOT(subWidgetValueChanged()));
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
    ui->segStrokeType->setCurrentText(SGFEffectTypes::FillTypeToString(mValue.fillType));
    ui->segStrokePosition->setCurrentText(SGFEffectTypes::StrokePositionToString(mValue.position));
    ui->colorSwatch->setColorValue(mValue.color);
    ui->gradientSwatch->setGradient(mValue.gradient);
    ui->patternSwatch->setPatternValue(mValue.pattern);
    ui->rowWidth->setIntValue(mValue.width);

    mIsUpdatingGui = false;
    refreshVisibleElements();
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
    mValue.fillType = SGFEffectTypes::FillTypeFromString(ui->segStrokeType->currentText());
    mValue.position = SGFEffectTypes::StrokePositionFromString(ui->segStrokePosition->currentText());
    mValue.color = ui->colorSwatch->colorValue();
    mValue.gradient = ui->gradientSwatch->gradient();
    mValue.pattern = ui->patternSwatch->patternValue();
    mValue.width = ui->rowWidth->intValue();

    emit valueChanged(mValue);
    refreshVisibleElements();
}

void StrokeEffectSettingsPanel::refreshVisibleElements()
{
    if ( mValue.fillType == SGFFillType::Fill_Color )
    {
        ui->labelColor->show();
        ui->colorSwatch->show();
        ui->labelGradient->hide();
        ui->gradientSwatch->hide();
        ui->labelPattern->hide();
        ui->patternSwatch->hide();
    }
    else if ( mValue.fillType == SGFFillType::Fill_Gradient )
    {
        ui->labelColor->hide();
        ui->colorSwatch->hide();
        ui->labelGradient->show();
        ui->gradientSwatch->show();
        ui->labelPattern->hide();
        ui->patternSwatch->hide();
    }
    else if ( mValue.fillType == SGFFillType::Fill_Pattern )
    {
        ui->labelColor->hide();
        ui->colorSwatch->hide();
        ui->labelGradient->hide();
        ui->gradientSwatch->hide();
        ui->labelPattern->show();
        ui->patternSwatch->show();
    }
}
