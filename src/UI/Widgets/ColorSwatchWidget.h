#ifndef COLORSWATCHWIDGET_H
#define COLORSWATCHWIDGET_H

#include <QtCore>
#include <QWidget>

class ColorSwatchWidget : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(QColor colorValue READ colorValue WRITE setColorValue)


    // Object Lifecycle
public:

    ColorSwatchWidget(QWidget * parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());
    ~ColorSwatchWidget();

public:

    /** Get the color value. */
    QColor colorValue() const;

    /** Set the color value and update the widget background. */
    void setColorValue(const QColor &color);


protected:

    /** When the user clicks on this widget, we want to present a color picker. */
    virtual void mousePressEvent(QMouseEvent *ev);

    /** Updated draw method as we want to be able to draw a nice background. */
    virtual void paintEvent(QPaintEvent *ev);


    // Signals
signals:
    void colorValueChanged(QColor);

    // Member Variables
private:
    QColor mColorValue;
};

#endif // COLORSWATCHWIDGET_H
