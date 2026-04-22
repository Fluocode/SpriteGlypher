#include "GenerationSettingsPanel.h"
#include "ui_GenerationSettingsPanel.h"

#include <QCheckBox>

GenerationSettingsPanel::GenerationSettingsPanel(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GenerationSettingsPanel),
    mIsUpdatingGui(false)
{
    ui->setupUi(this);

    ui->rowPadding->configureInt(0, 512, 1);
    ui->rowSpacing->configureInt(0, 512, 1);

    // Size Combo Boxes
    QStringList sizeOptions = QStringList() << "Auto" << "128" << "256" << "512" << "1024" << "2048";
    ui->genComboBoxWidth->addItems(sizeOptions);
    ui->genComboBoxHeight->addItems(sizeOptions);

    QObject::connect(ui->genComboBoxWidth, SIGNAL(currentTextChanged(QString)), this, SLOT(subWidgetValueChanged()));
    QObject::connect(ui->genComboBoxHeight, SIGNAL(currentTextChanged(QString)), this, SLOT(subWidgetValueChanged()));
    QObject::connect(ui->rowPadding, SIGNAL(valueChanged()), this, SLOT(subWidgetValueChanged()));
    QObject::connect(ui->genColorSwatch, SIGNAL(colorValueChanged(QColor)), this, SLOT(subWidgetValueChanged()));
    QObject::connect(ui->rowSpacing, SIGNAL(valueChanged()), this, SLOT(subWidgetValueChanged()));
    QObject::connect(ui->checkPaginate, &QCheckBox::toggled, this, &GenerationSettingsPanel::subWidgetValueChanged);

    QObject::connect(ui->checkShowAllBounds, &QCheckBox::toggled, this, &GenerationSettingsPanel::showGlyphBoundsChanged);
}


GenerationSettingsPanel::~GenerationSettingsPanel()
{
    delete ui;
}


void GenerationSettingsPanel::setValue(const SGFGenerationSettings & value)
{
    mValue = value;
    mIsUpdatingGui = true;

    ui->genComboBoxWidth->setCurrentText((mValue.width == 0) ? "Auto" : QString::number(mValue.width));
    ui->genComboBoxHeight->setCurrentText((mValue.height == 0) ? "Auto" : QString::number(mValue.height));
    ui->rowPadding->setIntValue(mValue.padding);
    ui->rowSpacing->setIntValue(mValue.spacing);
    ui->genColorSwatch->setColorValue(mValue.color);
    ui->checkPaginate->setChecked(mValue.paginate);

    mIsUpdatingGui = false;
}


SGFGenerationSettings GenerationSettingsPanel::getValue() const
{
    return mValue;
}


void GenerationSettingsPanel::setShowGlyphBounds(bool on)
{
    const bool prev = ui->checkShowAllBounds->blockSignals(true);
    ui->checkShowAllBounds->setChecked(on);
    ui->checkShowAllBounds->blockSignals(prev);
}


void GenerationSettingsPanel::subWidgetValueChanged()
{
    if ( mIsUpdatingGui ) {
        return;
    }

    mValue.width = ui->genComboBoxWidth->currentText().toInt();
    mValue.height = ui->genComboBoxHeight->currentText().toInt();
    mValue.padding = ui->rowPadding->intValue();
    mValue.spacing = ui->rowSpacing->intValue();
    mValue.color = ui->genColorSwatch->colorValue();
    mValue.paginate = ui->checkPaginate->isChecked();

    emit valueChanged(mValue);
}
