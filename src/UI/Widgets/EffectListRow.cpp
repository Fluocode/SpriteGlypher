#include "EffectListRow.h"
#include "ui_EffectListRow.h"


EffectListRow::EffectListRow(QWidget *parent, const QString &title, bool checked) :
    QWidget(parent),
    ui(new Ui::EffectListRow)
{
    ui->setupUi(this);

    setTitle(title);
    setChecked(checked);

    QObject::connect(ui->checkBoxEnabled, SIGNAL(stateChanged(int)), this, SLOT(checkBoxStateChanged(int)));
}

EffectListRow::~EffectListRow()
{
    delete ui;
}


const QString EffectListRow::title() const
{
    return ui->labelTitle->text();
}


void EffectListRow::setTitle(const QString &value)
{
    ui->labelTitle->setText(value);
}


QPixmap EffectListRow::icon() const
{
    return ui->labelIcon->pixmap();
}


void EffectListRow::setIcon(const QPixmap &value)
{
    ui->labelIcon->setPixmap(value);
}


bool EffectListRow::isChecked() const
{
    return ui->checkBoxEnabled->isChecked();
}


void EffectListRow::setChecked(bool value)
{
    ui->checkBoxEnabled->setChecked(value);
    emit checkedStateChanged(value);
}


//
// Slots
//


void EffectListRow::checkBoxStateChanged(int state)
{
    emit checkedStateChanged((state == 1));
}
