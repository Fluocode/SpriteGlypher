#include "PatternSwatchWidget.h"

#include <QColorDialog>
#include <QDebug>
#include <QPainter>
#include <QPainterPath>
#include <QFileDialog>


PatternSwatchWidget::PatternSwatchWidget(QWidget * parent, Qt::WindowFlags f) :
    QWidget(parent, f),
    mPatternValue(SGFPattern())
{

}


PatternSwatchWidget::~PatternSwatchWidget()
{

}


SGFPattern PatternSwatchWidget::patternValue() const
{
    return mPatternValue;
}


void PatternSwatchWidget::setPatternValue(const SGFPattern &pattern)
{
    mPatternValue = pattern;
    update();
    emit patternValueChanged(mPatternValue);
}


void PatternSwatchWidget::mousePressEvent(QMouseEvent *)
{
    if ( !isEnabled() ) {
        return;
    }

    QString path = QFileDialog::getOpenFileName(this, tr("Open Image File"), QString(), tr("JPEG Image (*.jpg);;PNG (*.png)"));

    if ( !path.isNull() )
    {
        QImage image(path);

        if ( !image.isNull() )
        {
            if ( image.width() > 512 || image.height() > 512 ) {
                if ( image.width() > image.height() ) {
                    image = image.scaledToWidth(512, Qt::SmoothTransformation);
                } else {
                    image = image.scaledToHeight(512, Qt::SmoothTransformation);
                }
            }

            mPatternValue.image = image;
            emit patternValueChanged(mPatternValue);
            update();
        }
    }
}


void PatternSwatchWidget::paintEvent(QPaintEvent *)
{
    QPainterPath rectPath;
    rectPath.addRect(rect());

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    if ( !mPatternValue.image.isNull() ) {
        painter.drawImage(rect(), mPatternValue.image);
    }

    painter.strokePath(rectPath, QPen(QColor(64,64,64,255)));
    painter.end();
}
