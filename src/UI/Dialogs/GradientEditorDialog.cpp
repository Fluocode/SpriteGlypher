#include "GradientEditorDialog.h"
#include "ui_GradientEditorDialog.h"


//
// Static Methods
//

SGFGradient GradientEditorDialog::getGradient(const SGFGradient &initial, QWidget *parent, const QString &title)
{
    GradientEditorDialog dialog(parent);
    dialog.setWindowTitle(title);
    dialog.setGradient(initial);
    dialog.exec();

    return dialog.getGradient();
}


//
// Object Lifecycle
//

GradientEditorDialog::GradientEditorDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GradientEditorDialog)
{
    ui->setupUi(this);

    ui->rowStopPosition->configureDouble(0.0, 1.0, 3, 0.01);
    ui->rowAngle->configureInt(0, 359, 1);
    ui->rowScale->configureInt(1, 1000, 1);

    QObject::connect(ui->gradientStopEditor, SIGNAL(gradientChanged(SGFGradient)), this, SLOT(updateGuiFromModel()));
    QObject::connect(ui->gradientStopEditor, SIGNAL(selectedStopChanged(int)), this, SLOT(updateGuiFromModel()));

    QObject::connect(ui->stopColorSwatch, SIGNAL(colorValueChanged(QColor)), this, SLOT(guiValueChanged()));
    QObject::connect(ui->rowStopPosition, SIGNAL(valueChanged()), this, SLOT(guiValueChanged()));

    QObject::connect(ui->comboBoxStyle, SIGNAL(currentIndexChanged(int)), this, SLOT(guiValueChanged()));
    QObject::connect(ui->rowAngle, SIGNAL(valueChanged()), this, SLOT(guiValueChanged()));
    QObject::connect(ui->rowScale, SIGNAL(valueChanged()), this, SLOT(guiValueChanged()));


    QStringList styleOptions = QStringList() << "Linear" << "Radial" << "Conical";
    ui->comboBoxStyle->addItems(styleOptions);
}


GradientEditorDialog::~GradientEditorDialog()
{
    delete ui;
}



//
// Get / Set Gradient
//


SGFGradient GradientEditorDialog::getGradient()
{
    return ui->gradientStopEditor->gradient();
}


void GradientEditorDialog::setGradient(const SGFGradient &gradient)
{
    ui->gradientStopEditor->setGradient(gradient);
    updatePreview();
    updateGuiFromModel();
}


void GradientEditorDialog::updatePreview()
{
    ui->gradientSwatchPreview->setGradient(ui->gradientStopEditor->gradient());
}


void GradientEditorDialog::updateGuiFromModel()
{
    mSuppressGuiUpdates = true;

    // Get data from stop editor
    SGFGradient gradient = ui->gradientStopEditor->gradient();
    int stopIndex = ui->gradientStopEditor->selectedGradientStop();

    // Update Gradient Fields
    ui->rowAngle->setIntValue(gradient.angle);
    ui->rowScale->setIntValue(qRound(gradient.scale * 100.f));

    switch( gradient.style ) {
        case SGFGradientStyle::Gradient_Linear: ui->comboBoxStyle->setCurrentIndex(0); break;
        case SGFGradientStyle::Gradient_Radial: ui->comboBoxStyle->setCurrentIndex(1); break;
        case SGFGradientStyle::Gradient_Conical: ui->comboBoxStyle->setCurrentIndex(2); break;
        case SGFGradientStyle::Gradient_Unknown: ui->comboBoxStyle->setCurrentIndex(0); break;
    }

    // Update Selected Stop Fields
    if ( stopIndex < 0 || stopIndex >= gradient.stops.size() )
    {
        ui->stopColorSwatch->setColorValue(QColor(0,0,0,255));
        //ui->stopColorSwatch->setEnabled(false);

        ui->rowStopPosition->setDoubleValue(0.0);
        ui->rowStopPosition->setEnabled(false);
    }
    else
    {
        SGFGradientStop stop = gradient.stops[stopIndex];

        //ui->stopColorSwatch->setEnabled(true);
        ui->stopColorSwatch->setColorValue(stop.color);

        ui->rowStopPosition->setEnabled(true);
        ui->rowStopPosition->setDoubleValue(stop.position);
    }

    updatePreview();
    mSuppressGuiUpdates = false;
}


void GradientEditorDialog::updateModelFromGui()
{
    int stopIndex = ui->gradientStopEditor->selectedGradientStop();
    SGFGradient gradient = ui->gradientStopEditor->gradient();

    gradient.angle = ui->rowAngle->intValue();
    gradient.scale = static_cast<float>(ui->rowScale->intValue()) * 0.01f;

    switch( ui->comboBoxStyle->currentIndex() ) {
        case 0: gradient.style = SGFGradientStyle::Gradient_Linear; break;
        case 1: gradient.style = SGFGradientStyle::Gradient_Radial; break;
        case 2: gradient.style = SGFGradientStyle::Gradient_Conical; break;
    }

    if ( stopIndex >= 0 && stopIndex < gradient.stops.size() )
    {
        SGFGradientStop& stop = gradient.stops[stopIndex];
        stop.color = ui->stopColorSwatch->colorValue();
        stop.position = ui->rowStopPosition->doubleValue();
    }

    ui->gradientStopEditor->updateGradient(gradient);
    updatePreview();
}


void GradientEditorDialog::guiValueChanged()
{
    if ( !mSuppressGuiUpdates )
    {
        updateModelFromGui();
    }
}
