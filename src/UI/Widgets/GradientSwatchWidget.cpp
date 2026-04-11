#include "GradientSwatchWidget.h"

#include <QColorDialog>
#include <QDebug>
#include <QPainter>
#include <QPainterPath>

#include "../Dialogs/GradientEditorDialog.h"


GradientSwatchWidget::GradientSwatchWidget(QWidget * parent, Qt::WindowFlags f) :
    QWidget(parent, f),
    mGradient()
{
    mBackground = QPixmap(10, 10);

    QPainter pmp(&mBackground);
    pmp.fillRect(0, 0, 5, 5, Qt::white);
    pmp.fillRect(5, 5, 5, 5, Qt::white);
    pmp.fillRect(0, 5, 5, 5, QColor(220,220,220,255));
    pmp.fillRect(5, 0, 5, 5, QColor(220,220,220,255));
    pmp.end();
}


GradientSwatchWidget::~GradientSwatchWidget()
{

}


SGFGradient GradientSwatchWidget::gradient() const
{
    return mGradient;
}


void GradientSwatchWidget::setGradient(const SGFGradient& gradient)
{
    mGradient = gradient;
    update();
    emit gradientChanged(gradient);
}


void GradientSwatchWidget::mousePressEvent(QMouseEvent */*ev*/)
{
    if ( !isEnabled() ) {
        return;
    }

    SGFGradient gradient = GradientEditorDialog::getGradient(mGradient, this, "Gradient Editor");

    if ( gradient.isValid() )
    {
        setGradient(gradient);
    }
}


void GradientSwatchWidget::paintEvent(QPaintEvent */*ev*/)
{
    QPainterPath rectPath;
    rectPath.addRect(rect());

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);    
    painter.fillRect(rect(), mBackground);

    mGradient.fillRect(painter, rect());

    QPen pen = QPen(Qt::black);
    painter.strokePath(rectPath, pen);
}
