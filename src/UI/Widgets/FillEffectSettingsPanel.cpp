#include "FillEffectSettingsPanel.h"
#include "ui_FillEffectSettingsPanel.h"

FillEffectSettingsPanel::FillEffectSettingsPanel(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FillEffectSettingsPanel),
    mIsUpdatingGui(false),
    mValue()
{
    ui->setupUi(this);

    ui->rowOpacity->configureDouble(0.0, 1.0, 2, 0.05);
    ui->rowInset->configureInt(0, 256, 1);

    // enabled, type, color, gradient
    ui->comboBoxBlend->addItems(SGFEffectTypes::BlendModeOptions());
    ui->segFillType->setOptions(SGFEffectTypes::FillTypeOptions());

    QObject::connect(ui->comboBoxBlend, SIGNAL(currentIndexChanged(int)), this, SLOT(subWidgetValueChanged()));
    QObject::connect(ui->rowOpacity, SIGNAL(valueChanged()), this, SLOT(subWidgetValueChanged()));
    QObject::connect(ui->segFillType, SIGNAL(currentIndexChanged(int)), this, SLOT(subWidgetValueChanged()));
    QObject::connect(ui->colorSwatch, SIGNAL(colorValueChanged(QColor)), this, SLOT(subWidgetValueChanged()));
    QObject::connect(ui->gradientSwatch, SIGNAL(gradientChanged(SGFGradient)), this, SLOT(subWidgetValueChanged()));
    QObject::connect(ui->patternSwatch, SIGNAL(patternValueChanged(SGFPattern)), this, SLOT(subWidgetValueChanged()));
    QObject::connect(ui->rowInset, SIGNAL(valueChanged()), this, SLOT(subWidgetValueChanged()));
}

FillEffectSettingsPanel::~FillEffectSettingsPanel()
{
    delete ui;
}


void FillEffectSettingsPanel::setValue(const SGFFillEffectSettings & value)
{
    mValue = value;
    mIsUpdatingGui = true;

    ui->comboBoxBlend->setCurrentText(SGFEffectTypes::BlendModeToString(mValue.blendMode));
    ui->rowOpacity->setDoubleValue(mValue.opacity);
    ui->rowInset->setIntValue(mValue.inset);
    ui->segFillType->setCurrentText(SGFEffectTypes::FillTypeToString(mValue.fillType));
    ui->colorSwatch->setColorValue(mValue.color);
    ui->gradientSwatch->setGradient(mValue.gradient);
    ui->patternSwatch->setPatternValue(mValue.pattern);

    mIsUpdatingGui = false;
    refreshVisibleElements();
}


SGFFillEffectSettings FillEffectSettingsPanel::getValue() const
{
    return mValue;
}


void FillEffectSettingsPanel::subWidgetValueChanged()
{
    if ( mIsUpdatingGui ) {
        return;
    }

    mValue.blendMode = SGFEffectTypes::BlendModeFromString(ui->comboBoxBlend->currentText());
    mValue.opacity = ui->rowOpacity->doubleValue();
    mValue.fillType = SGFEffectTypes::FillTypeFromString(ui->segFillType->currentText());
    mValue.color = ui->colorSwatch->colorValue();
    mValue.gradient = ui->gradientSwatch->gradient();
    mValue.pattern = ui->patternSwatch->patternValue();
    mValue.inset = ui->rowInset->intValue();

    emit valueChanged(mValue);

    refreshVisibleElements();
}


void FillEffectSettingsPanel::refreshVisibleElements()
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
