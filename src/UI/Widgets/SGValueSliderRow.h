#ifndef SGVALUE_SLIDER_ROW_H
#define SGVALUE_SLIDER_ROW_H

#include <QWidget>

class QSlider;
class QSpinBox;
class QDoubleSpinBox;

/**
 * Horizontal row: [ QSlider | value box ] — replaces stepped spinboxes.
 * Integer mode maps slider 1:1; double mode maps slider 0…kSliderDoubleSteps to [min,max].
 */
class SGValueSliderRow : public QWidget
{
    Q_OBJECT

public:
    enum Mode
    {
        IntMode,
        DoubleMode
    };

    explicit SGValueSliderRow(QWidget *parent = nullptr);

    void setMode(Mode mode);
    Mode mode() const { return m_mode; }

    void configureInt(int min, int max, int step = 1);
    void configureDouble(double min, double max, int decimals, double singleStep);

    int intValue() const;
    void setIntValue(int v);

    double doubleValue() const;
    void setDoubleValue(double v);

    void setReadOnly(bool ro);

signals:
    void valueChanged();

private slots:
    void onSliderIntChanged(int v);
    void onSpinIntChanged(int v);
    void onSliderDoubleChanged(int v);
    void onSpinDoubleChanged(double v);

private:
    void syncIntFromSpin();
    void syncDoubleFromSpin();
    double sliderToDouble(int sliderPos) const;
    int doubleToSlider(double val) const;
    int sliderToInt(int sliderPos) const;
    int intToSlider(int val) const;

    Mode m_mode = IntMode;
    QSlider *m_slider = nullptr;
    QSpinBox *m_intSpin = nullptr;
    QDoubleSpinBox *m_doubleSpin = nullptr;

    int m_iMin = 0;
    int m_iMax = 100;
    int m_iStep = 1;
    bool m_intNonLinear = false;

    double m_dMin = 0.0;
    double m_dMax = 1.0;
    bool m_block = false;

    static constexpr int kSliderDoubleSteps = 1000;
    static constexpr int kSliderIntSteps = 1000;
};

#endif // SGVALUE_SLIDER_ROW_H
