#include "SGValueSliderRow.h"

#include <QHBoxLayout>
#include <QDoubleSpinBox>
#include <QSlider>
#include <QSpinBox>
#include <QAbstractSpinBox>
#include <QtGlobal>

SGValueSliderRow::SGValueSliderRow(QWidget *parent)
    : QWidget(parent)
{
    m_slider = new QSlider(Qt::Horizontal, this);
    m_slider->setTracking(true);
    m_slider->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    m_slider->setMinimumHeight(22);

    m_intSpin = new QSpinBox(this);
    m_intSpin->setButtonSymbols(QAbstractSpinBox::NoButtons);
    m_intSpin->setFixedWidth(52);
    m_intSpin->setAlignment(Qt::AlignCenter);

    m_doubleSpin = new QDoubleSpinBox(this);
    m_doubleSpin->setButtonSymbols(QAbstractSpinBox::NoButtons);
    m_doubleSpin->setFixedWidth(56);
    m_doubleSpin->setAlignment(Qt::AlignCenter);
    m_doubleSpin->hide();

    auto *lay = new QHBoxLayout(this);
    lay->setContentsMargins(0, 0, 0, 0);
    lay->setSpacing(6);
    lay->addWidget(m_slider, 1);
    lay->addWidget(m_intSpin, 0);
    lay->addWidget(m_doubleSpin, 0);

    setMode(IntMode);

    connect(m_slider, &QSlider::valueChanged, this, &SGValueSliderRow::onSliderIntChanged);
    connect(m_intSpin, QOverload<int>::of(&QSpinBox::valueChanged), this, &SGValueSliderRow::onSpinIntChanged);
    connect(m_slider, &QSlider::valueChanged, this, &SGValueSliderRow::onSliderDoubleChanged);
    connect(m_doubleSpin, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &SGValueSliderRow::onSpinDoubleChanged);
}

void SGValueSliderRow::setMode(Mode mode)
{
    m_mode = mode;
    if ( mode == IntMode ) {
        m_intSpin->show();
        m_doubleSpin->hide();
    } else {
        m_intSpin->hide();
        m_doubleSpin->show();
    }
}

void SGValueSliderRow::configureInt(int min, int max, int step)
{
    setMode(IntMode);
    m_block = true;
    m_slider->setRange(min, max);
    m_slider->setSingleStep(step);
    m_slider->setPageStep(step * 10);
    m_intSpin->setRange(min, max);
    m_intSpin->setSingleStep(step);
    m_block = false;
    syncIntFromSpin();
}

void SGValueSliderRow::configureDouble(double min, double max, int decimals, double singleStep)
{
    setMode(DoubleMode);
    m_dMin = min;
    m_dMax = max;
    m_block = true;
    m_slider->setRange(0, kSliderDoubleSteps);
    m_slider->setSingleStep(1);
    m_slider->setPageStep(10);
    m_doubleSpin->setDecimals(decimals);
    m_doubleSpin->setRange(min, max);
    m_doubleSpin->setSingleStep(singleStep);
    m_block = false;
    syncDoubleFromSpin();
}

int SGValueSliderRow::intValue() const
{
    return m_intSpin->value();
}

void SGValueSliderRow::setIntValue(int v)
{
    m_block = true;
    m_intSpin->setValue(v);
    m_slider->setValue(v);
    m_block = false;
}

double SGValueSliderRow::doubleValue() const
{
    return m_doubleSpin->value();
}

void SGValueSliderRow::setDoubleValue(double v)
{
    m_block = true;
    m_doubleSpin->setValue(v);
    m_slider->setValue(doubleToSlider(v));
    m_block = false;
}

void SGValueSliderRow::setReadOnly(bool ro)
{
    m_slider->setEnabled(!ro);
    m_intSpin->setReadOnly(ro);
    m_doubleSpin->setReadOnly(ro);
}

void SGValueSliderRow::onSliderIntChanged(int v)
{
    if ( m_mode != IntMode || m_block ) {
        return;
    }
    m_block = true;
    m_intSpin->setValue(v);
    m_block = false;
    emit valueChanged();
}

void SGValueSliderRow::onSpinIntChanged(int v)
{
    if ( m_mode != IntMode || m_block ) {
        return;
    }
    m_block = true;
    m_slider->setValue(v);
    m_block = false;
    emit valueChanged();
}

void SGValueSliderRow::onSliderDoubleChanged(int sliderPos)
{
    if ( m_mode != DoubleMode || m_block ) {
        return;
    }
    m_block = true;
    m_doubleSpin->setValue(sliderToDouble(sliderPos));
    m_block = false;
    emit valueChanged();
}

void SGValueSliderRow::onSpinDoubleChanged(double val)
{
    if ( m_mode != DoubleMode || m_block ) {
        return;
    }
    m_block = true;
    m_slider->setValue(doubleToSlider(val));
    m_block = false;
    emit valueChanged();
}

void SGValueSliderRow::syncIntFromSpin()
{
    m_block = true;
    m_slider->setValue(m_intSpin->value());
    m_block = false;
}

double SGValueSliderRow::sliderToDouble(int sliderPos) const
{
    if ( m_dMax <= m_dMin ) {
        return m_dMin;
    }
    return m_dMin + (static_cast<double>(sliderPos) / static_cast<double>(kSliderDoubleSteps)) * (m_dMax - m_dMin);
}

int SGValueSliderRow::doubleToSlider(double val) const
{
    if ( m_dMax <= m_dMin ) {
        return 0;
    }
    const double t = (val - m_dMin) / (m_dMax - m_dMin);
    const int s = qRound(qBound(0.0, t, 1.0) * static_cast<double>(kSliderDoubleSteps));
    return s;
}

void SGValueSliderRow::syncDoubleFromSpin()
{
    m_block = true;
    m_slider->setValue(doubleToSlider(m_doubleSpin->value()));
    m_block = false;
}
