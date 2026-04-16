#include "InnerHighlightEffectSettingsPanel.h"
#include "ui_InnerHighlightEffectSettingsPanel.h"

InnerHighlightEffectSettingsPanel::InnerHighlightEffectSettingsPanel(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::InnerHighlightEffectSettingsPanel)
{
    ui->setupUi(this);

    ui->rowOpacity->configureDouble(0.0, 1.0, 2, 0.05);
    ui->rowInset->configureInt(0, 256, 1);
    ui->rowHeight->configureDouble(0.0, 1.0, 2, 0.01);
    ui->rowSoften->configureDouble(0.0, 1.0, 2, 0.01);

    ui->comboBoxBlend->addItems(SGFEffectTypes::BlendModeOptions());
    ui->segFillType->setOptions(QStringList() << "Color" << "Gradient");

    QObject::connect(ui->comboBoxBlend, SIGNAL(currentIndexChanged(int)), this, SLOT(subWidgetValueChanged()));
    QObject::connect(ui->rowOpacity, SIGNAL(valueChanged()), this, SLOT(subWidgetValueChanged()));
    QObject::connect(ui->segFillType, SIGNAL(currentIndexChanged(int)), this, SLOT(subWidgetValueChanged()));
    QObject::connect(ui->colorSwatch, SIGNAL(colorValueChanged(QColor)), this, SLOT(subWidgetValueChanged()));
    QObject::connect(ui->gradientSwatch, SIGNAL(gradientChanged(SGFGradient)), this, SLOT(subWidgetValueChanged()));
    QObject::connect(ui->rowInset, SIGNAL(valueChanged()), this, SLOT(subWidgetValueChanged()));
    QObject::connect(ui->rowHeight, SIGNAL(valueChanged()), this, SLOT(subWidgetValueChanged()));
    QObject::connect(ui->rowSoften, SIGNAL(valueChanged()), this, SLOT(subWidgetValueChanged()));
}

InnerHighlightEffectSettingsPanel::~InnerHighlightEffectSettingsPanel()
{
    delete ui;
}

void InnerHighlightEffectSettingsPanel::setValue(const SGFInnerHighlightEffectSettings &value)
{
    mValue = value;
    mIsUpdatingGui = true;

    ui->comboBoxBlend->setCurrentText(SGFEffectTypes::BlendModeToString(mValue.blendMode));
    ui->rowOpacity->setDoubleValue(mValue.opacity);
    ui->rowInset->setIntValue(mValue.inset);
    ui->rowHeight->setDoubleValue(mValue.heightFrac);
    ui->rowSoften->setDoubleValue(mValue.soften);

    ui->segFillType->setCurrentText((mValue.fillType == SGFFillType::Fill_Gradient) ? "Gradient" : "Color");
    ui->colorSwatch->setColorValue(mValue.color);
    ui->gradientSwatch->setGradient(mValue.gradient);

    mIsUpdatingGui = false;
    refreshVisibleElements();
}

SGFInnerHighlightEffectSettings InnerHighlightEffectSettingsPanel::getValue() const
{
    return mValue;
}

void InnerHighlightEffectSettingsPanel::subWidgetValueChanged()
{
    if ( mIsUpdatingGui ) {
        return;
    }

    mValue.blendMode = SGFEffectTypes::BlendModeFromString(ui->comboBoxBlend->currentText());
    mValue.opacity = ui->rowOpacity->doubleValue();
    mValue.inset = ui->rowInset->intValue();
    mValue.heightFrac = static_cast<float>(ui->rowHeight->doubleValue());
    mValue.soften = static_cast<float>(ui->rowSoften->doubleValue());

    const QString ft = ui->segFillType->currentText();
    mValue.fillType = (ft.compare(QStringLiteral("Gradient"), Qt::CaseInsensitive) == 0)
        ? SGFFillType::Fill_Gradient
        : SGFFillType::Fill_Color;
    mValue.color = ui->colorSwatch->colorValue();
    mValue.gradient = ui->gradientSwatch->gradient();

    emit valueChanged(mValue);
    refreshVisibleElements();
}

void InnerHighlightEffectSettingsPanel::refreshVisibleElements()
{
    const bool grad = (mValue.fillType == SGFFillType::Fill_Gradient);
    ui->labelColor->setVisible(!grad);
    ui->colorSwatch->setVisible(!grad);
    ui->labelGradient->setVisible(grad);
    ui->gradientSwatch->setVisible(grad);
}

