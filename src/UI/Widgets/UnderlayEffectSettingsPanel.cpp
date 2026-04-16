#include "UnderlayEffectSettingsPanel.h"
#include "ui_UnderlayEffectSettingsPanel.h"

UnderlayEffectSettingsPanel::UnderlayEffectSettingsPanel(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::UnderlayEffectSettingsPanel)
{
    ui->setupUi(this);

    ui->rowOpacity->configureDouble(0.0, 1.0, 2, 0.05);
    ui->rowAngle->configureInt(0, 360, 1);
    ui->rowDistance->configureInt(0, 512, 1);
    ui->rowSize->configureInt(0, 512, 1);

    ui->comboBoxBlend->addItems(SGFEffectTypes::BlendModeOptions());
    ui->segFillType->setOptions(QStringList() << "Color" << "Gradient");

    QObject::connect(ui->comboBoxBlend, SIGNAL(currentIndexChanged(int)), this, SLOT(subWidgetValueChanged()));
    QObject::connect(ui->rowOpacity, SIGNAL(valueChanged()), this, SLOT(subWidgetValueChanged()));
    QObject::connect(ui->segFillType, SIGNAL(currentIndexChanged(int)), this, SLOT(subWidgetValueChanged()));
    QObject::connect(ui->colorSwatch, SIGNAL(colorValueChanged(QColor)), this, SLOT(subWidgetValueChanged()));
    QObject::connect(ui->gradientSwatch, SIGNAL(gradientChanged(SGFGradient)), this, SLOT(subWidgetValueChanged()));
    QObject::connect(ui->rowAngle, SIGNAL(valueChanged()), this, SLOT(subWidgetValueChanged()));
    QObject::connect(ui->rowDistance, SIGNAL(valueChanged()), this, SLOT(subWidgetValueChanged()));
    QObject::connect(ui->rowSize, SIGNAL(valueChanged()), this, SLOT(subWidgetValueChanged()));
}

UnderlayEffectSettingsPanel::~UnderlayEffectSettingsPanel()
{
    delete ui;
}

void UnderlayEffectSettingsPanel::setValue(const SGFUnderlayEffectSettings &value)
{
    mValue = value;
    mIsUpdatingGui = true;

    ui->comboBoxBlend->setCurrentText(SGFEffectTypes::BlendModeToString(mValue.blendMode));
    ui->rowOpacity->setDoubleValue(mValue.opacity);

    if ( mValue.fillType == SGFFillType::Fill_Gradient ) {
        ui->segFillType->setCurrentText("Gradient");
    } else {
        ui->segFillType->setCurrentText("Color");
    }

    ui->colorSwatch->setColorValue(mValue.color);
    ui->gradientSwatch->setGradient(mValue.gradient);
    ui->rowAngle->setIntValue(static_cast<int>(mValue.angle));
    ui->rowDistance->setIntValue(static_cast<int>(mValue.distance));
    ui->rowSize->setIntValue(static_cast<int>(mValue.size));

    mIsUpdatingGui = false;
    refreshVisibleElements();
}

SGFUnderlayEffectSettings UnderlayEffectSettingsPanel::getValue() const
{
    return mValue;
}

void UnderlayEffectSettingsPanel::subWidgetValueChanged()
{
    if ( mIsUpdatingGui ) {
        return;
    }

    mValue.blendMode = SGFEffectTypes::BlendModeFromString(ui->comboBoxBlend->currentText());
    mValue.opacity = ui->rowOpacity->doubleValue();

    const QString ft = ui->segFillType->currentText();
    mValue.fillType = (ft.compare(QStringLiteral("Gradient"), Qt::CaseInsensitive) == 0)
        ? SGFFillType::Fill_Gradient
        : SGFFillType::Fill_Color;

    mValue.color = ui->colorSwatch->colorValue();
    mValue.gradient = ui->gradientSwatch->gradient();
    mValue.angle = static_cast<float>(ui->rowAngle->intValue());
    mValue.distance = static_cast<float>(ui->rowDistance->intValue());
    mValue.size = static_cast<float>(ui->rowSize->intValue());

    emit valueChanged(mValue);
    refreshVisibleElements();
}

void UnderlayEffectSettingsPanel::refreshVisibleElements()
{
    const bool grad = (mValue.fillType == SGFFillType::Fill_Gradient);
    if ( grad ) {
        ui->labelColor->hide();
        ui->colorSwatch->hide();
        ui->labelGradient->show();
        ui->gradientSwatch->show();
    } else {
        ui->labelColor->show();
        ui->colorSwatch->show();
        ui->labelGradient->hide();
        ui->gradientSwatch->hide();
    }
}

