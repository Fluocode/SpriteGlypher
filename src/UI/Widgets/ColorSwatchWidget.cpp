#include "ColorSwatchWidget.h"

#include <QColorDialog>
#include <QDebug>
#include <QPainter>
#include <QPainterPath>


ColorSwatchWidget::ColorSwatchWidget(QWidget * parent, Qt::WindowFlags f) :
    QWidget(parent, f),
    mColorValue(QColor(255,255,0,255))
{
}


ColorSwatchWidget::~ColorSwatchWidget()
{

}


QColor ColorSwatchWidget::colorValue() const
{
    return mColorValue;
}


void ColorSwatchWidget::setColorValue(const QColor &color)
{
    mColorValue = color;
    update();
    emit colorValueChanged(color);
}


void ColorSwatchWidget::mousePressEvent(QMouseEvent *ev)
{
    Q_UNUSED(ev);
    if ( !isEnabled() ) {
        return;
    }

    QColor color = QColorDialog::getColor(mColorValue, this, "Select Color",  QColorDialog::ShowAlphaChannel);

    if ( color.isValid() )
    {
        setColorValue(color);
    }
}


void ColorSwatchWidget::paintEvent(QPaintEvent *ev)
{
    Q_UNUSED(ev);
    QVector<QPointF> bottomRight;
    bottomRight.push_back(QPointF(rect().width(), 0));
    bottomRight.push_back(QPointF(rect().width(), rect().height()));
    bottomRight.push_back(QPointF(0, rect().height()));

    QPainterPath rectPath;
    rectPath.addRect(rect());

    QPainterPath bottomRightPath;
    bottomRightPath.addPolygon(QPolygonF(bottomRight));

    QPen pen = QPen(Qt::black);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.fillPath(rectPath, Qt::black);
    painter.fillPath(bottomRightPath, Qt::white);
    painter.fillPath(rectPath, mColorValue);
    painter.strokePath(rectPath, pen);
    painter.end();
}
