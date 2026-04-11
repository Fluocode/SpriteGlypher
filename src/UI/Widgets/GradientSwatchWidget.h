#ifndef GRADIENTSWATCHWIDGET_H
#define GRADIENTSWATCHWIDGET_H

#include <QtCore>
#include <QWidget>
#include "../../Model/SGFTypes.h"

class GradientSwatchWidget : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(SGFGradient gradient READ gradient WRITE setGradient)


    // Object Lifecycle
public:

    GradientSwatchWidget(QWidget * parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());
    ~GradientSwatchWidget();

public:

    /** Get the displayed gradient. */
    SGFGradient gradient() const;

    /** Set the displayed gradient and update the widget. */
    void setGradient(const SGFGradient& gradient);


protected:

    /** When the user clicks on this widget, we want to present a gradient builder dialog. */
    virtual void mousePressEvent(QMouseEvent *ev);

    /** Updated draw method as we want to be able to draw a nice background. */
    virtual void paintEvent(QPaintEvent *ev);


    // Signals
signals:
    void gradientChanged(SGFGradient);


    // Member Variables
private:
    SGFGradient mGradient;
    QPixmap mBackground;
};

#endif // GRADIENTSWATCHWIDGET_H
