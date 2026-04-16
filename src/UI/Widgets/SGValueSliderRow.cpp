#include "SGValueSliderRow.h"

#include <QHBoxLayout>
#include <QDoubleSpinBox>
#include <QSlider>
#include <QSpinBox>
#include <QAbstractSpinBox>
#include <QtGlobal>
#include <cmath>

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
    m_iMin = min;
    m_iMax = max;
    m_iStep = (step <= 0) ? 1 : step;

    // Automatic non-linear slider for large ranges (more precision near low values).
    // Keep angles linear (common 0..360 case).
    const bool looksLikeAngle = (m_iMin == 0 && m_iMax == 360);
    const int span = (m_iMax - m_iMin);
    m_intNonLinear = (!looksLikeAngle && span >= 50);

    if ( m_intNonLinear ) {
        m_slider->setRange(0, kSliderIntSteps);
        m_slider->setSingleStep(1);
        m_slider->setPageStep(10);
    } else {
        m_slider->setRange(min, max);
        m_slider->setSingleStep(m_iStep);
        m_slider->setPageStep(m_iStep * 10);
    }
    m_intSpin->setRange(min, max);
    m_intSpin->setSingleStep(m_iStep);
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
    m_slider->setValue(intToSlider(v));
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
    m_intSpin->setValue(sliderToInt(v));
    m_block = false;
    emit valueChanged();
}

void SGValueSliderRow::onSpinIntChanged(int v)
{
    if ( m_mode != IntMode || m_block ) {
        return;
    }
    m_block = true;
    m_slider->setValue(intToSlider(v));
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
    m_slider->setValue(intToSlider(m_intSpin->value()));
    m_block = false;
}

int SGValueSliderRow::sliderToInt(int sliderPos) const
{
    if ( !m_intNonLinear ) {
        return sliderPos;
    }
    if ( m_iMax <= m_iMin ) {
        return m_iMin;
    }

    double t = static_cast<double>(sliderPos) / static_cast<double>(kSliderIntSteps);
    t = qBound(0.0, t, 1.0);

    // Gamma curve: more precision near 0.
    constexpr double gamma = 2.2;
    const double curved = std::pow(t, gamma);
    const double raw = static_cast<double>(m_iMin) + curved * static_cast<double>(m_iMax - m_iMin);

    int v = qRound(raw);
    if ( m_iStep > 1 ) {
        v = m_iMin + qRound(static_cast<double>(v - m_iMin) / static_cast<double>(m_iStep)) * m_iStep;
    }
    return qBound(m_iMin, v, m_iMax);
}

int SGValueSliderRow::intToSlider(int val) const
{
    if ( !m_intNonLinear ) {
        return val;
    }
    if ( m_iMax <= m_iMin ) {
        return 0;
    }

    const double t = (static_cast<double>(val) - static_cast<double>(m_iMin))
        / static_cast<double>(m_iMax - m_iMin);
    const double clamped = qBound(0.0, t, 1.0);

    // Inverse gamma.
    constexpr double gamma = 2.2;
    const double inv = std::pow(clamped, 1.0 / gamma);
    return qRound(inv * static_cast<double>(kSliderIntSteps));
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
