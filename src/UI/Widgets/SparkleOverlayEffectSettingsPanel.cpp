#include "SparkleOverlayEffectSettingsPanel.h"
#include "ui_SparkleOverlayEffectSettingsPanel.h"

SparkleOverlayEffectSettingsPanel::SparkleOverlayEffectSettingsPanel(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::SparkleOverlayEffectSettingsPanel)
{
    ui->setupUi(this);

    ui->rowOpacity->configureDouble(0.0, 1.0, 2, 0.05);
    ui->rowScale->configureDouble(0.1, 8.0, 2, 0.05);
    ui->rowRotation->configureInt(0, 360, 1); // keep linear (angle-like)
    ui->rowOffsetX->configureInt(-512, 512, 1);
    ui->rowOffsetY->configureInt(-512, 512, 1);

    ui->comboBoxBlend->addItems(SGFEffectTypes::BlendModeOptions());

    QObject::connect(ui->comboBoxBlend, SIGNAL(currentIndexChanged(int)), this, SLOT(subWidgetValueChanged()));
    QObject::connect(ui->rowOpacity, SIGNAL(valueChanged()), this, SLOT(subWidgetValueChanged()));
    QObject::connect(ui->patternSwatch, SIGNAL(patternValueChanged(SGFPattern)), this, SLOT(subWidgetValueChanged()));
    QObject::connect(ui->rowScale, SIGNAL(valueChanged()), this, SLOT(subWidgetValueChanged()));
    QObject::connect(ui->rowRotation, SIGNAL(valueChanged()), this, SLOT(subWidgetValueChanged()));
    QObject::connect(ui->rowOffsetX, SIGNAL(valueChanged()), this, SLOT(subWidgetValueChanged()));
    QObject::connect(ui->rowOffsetY, SIGNAL(valueChanged()), this, SLOT(subWidgetValueChanged()));
}

SparkleOverlayEffectSettingsPanel::~SparkleOverlayEffectSettingsPanel()
{
    delete ui;
}

void SparkleOverlayEffectSettingsPanel::setValue(const SGFSparkleOverlayEffectSettings &value)
{
    mValue = value;
    mIsUpdatingGui = true;
    ui->comboBoxBlend->setCurrentText(SGFEffectTypes::BlendModeToString(mValue.blendMode));
    ui->rowOpacity->setDoubleValue(mValue.opacity);
    ui->patternSwatch->setPatternValue(mValue.pattern);
    ui->rowScale->setDoubleValue(mValue.scale);
    ui->rowRotation->setIntValue(static_cast<int>(mValue.rotation));
    ui->rowOffsetX->setIntValue(static_cast<int>(mValue.offsetX));
    ui->rowOffsetY->setIntValue(static_cast<int>(mValue.offsetY));
    mIsUpdatingGui = false;
}

SGFSparkleOverlayEffectSettings SparkleOverlayEffectSettingsPanel::getValue() const
{
    return mValue;
}

void SparkleOverlayEffectSettingsPanel::subWidgetValueChanged()
{
    if ( mIsUpdatingGui ) {
        return;
    }

    mValue.blendMode = SGFEffectTypes::BlendModeFromString(ui->comboBoxBlend->currentText());
    mValue.opacity = ui->rowOpacity->doubleValue();
    mValue.pattern = ui->patternSwatch->patternValue();
    mValue.scale = static_cast<float>(ui->rowScale->doubleValue());
    mValue.rotation = static_cast<float>(ui->rowRotation->intValue());
    mValue.offsetX = static_cast<float>(ui->rowOffsetX->intValue());
    mValue.offsetY = static_cast<float>(ui->rowOffsetY->intValue());

    emit valueChanged(mValue);
}

