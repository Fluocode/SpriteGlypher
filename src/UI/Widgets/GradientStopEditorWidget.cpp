#include "GradientStopEditorWidget.h"

#include <QColorDialog>
#include <QMouseEvent>

GradientStopEditorWidget::GradientStopEditorWidget(QWidget * parent) :
    QWidget(parent),
    mGradient()
{
}


GradientStopEditorWidget::~GradientStopEditorWidget()
{

}


SGFGradient GradientStopEditorWidget::gradient() const
{
    return mGradient;
}


void GradientStopEditorWidget::setGradient(const SGFGradient& gradient)
{
    mGradient = gradient;
    mSelectedStop = -1;
    update();
    emit gradientChanged(gradient);
}


void GradientStopEditorWidget::updateGradient(const SGFGradient& gradient)
{
    mGradient = gradient;

    if ( mSelectedStop >= mGradient.stops.size() ) {
        mSelectedStop = -1;
    }

    update();
}


int GradientStopEditorWidget::selectedGradientStop()
{
    return mSelectedStop;
}


void GradientStopEditorWidget::setSelectedGradientStop(int index)
{
    if ( index < 0 || index >= mGradient.stops.size() ) {
        mSelectedStop = -1;
    } else {
        mSelectedStop = index;
    }

    update();
    emit(selectedStopChanged(mSelectedStop));
}



void GradientStopEditorWidget::mousePressEvent(QMouseEvent *ev)
{
    if ( !isEnabled() ) {
        return;
    }

    if ( ev->button() == Qt::LeftButton )
    {
        StopEditorGeometry geom = createGeometry();
        int handle = stopHandleAtPoint(geom, ev->pos());

        if ( handle > -1 ) {
            setSelectedGradientStop(handle);
            mDraggingStop = handle;
        }
    }

    if ( ev->button() == Qt::RightButton )
    {
        if ( mGradient.stops.count() > 2 )
        {
            StopEditorGeometry geom = createGeometry();
            int handle = stopHandleAtPoint(geom, ev->pos());

            if ( handle > -1 ) {
                mGradient.stops.remove(handle);

                emit gradientChanged(mGradient);
                update();
            }
        }
    }
}


void GradientStopEditorWidget::mouseMoveEvent(QMouseEvent *ev)
{
    if ( !isEnabled() ) {
        return;
    }

    if ( mDraggingStop > -1 )
    {
        StopEditorGeometry geom = createGeometry();
        float value = stopPositionAtPoint(geom, ev->pos());

        mGradient.stops[mDraggingStop].position = value;
        update();
        emit gradientChanged(mGradient);
    }
}


void GradientStopEditorWidget::mouseReleaseEvent(QMouseEvent *ev)
{
    if ( ev->button() == Qt::LeftButton )
    {
        mDraggingStop = -1;
    }
}


void GradientStopEditorWidget::mouseDoubleClickEvent(QMouseEvent *ev)
{
    if ( !isEnabled() ) {
        return;
    }

    if ( ev->button() == Qt::LeftButton )
    {

        StopEditorGeometry geom = createGeometry();
        int handle = stopHandleAtPoint(geom, ev->pos());

        if ( handle > -1 )
        {
            showColorDialogForStop(handle);
        }
        else
        {
            float value = stopPositionAtPoint(geom, ev->pos());

            SGFGradientStop stop;
            stop.position = value;

            mGradient.stops.push_back(stop);
            emit gradientChanged(mGradient);
            update();
        }
    }
}


void GradientStopEditorWidget::paintEvent(QPaintEvent *ev)
{
    Q_UNUSED(ev);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // Get the geometry for the current state of the widget
    StopEditorGeometry geom = createGeometry();

    // Start by drawing the gradient linearly in the top half
    QLinearGradient linearGradient = mGradient.linearGradientForEditor(geom.gradientRect);
    QPen blackPen = QPen(Qt::black);

    painter.fillPath(geom.gradientStripPath, linearGradient);
    painter.strokePath(geom.gradientStripPath, blackPen);

    // Now draw the arrows underneath
    for( int i = 0; i < mGradient.stops.size(); ++i )
    {
        SGFGradientStop stop = mGradient.stops[i];

        if ( i == mSelectedStop ) {
            painter.fillPath(geom.stopArrowPaths[i], Qt::white);
        } else {
            painter.fillPath(geom.stopArrowPaths[i], Qt::lightGray);
        }

        painter.fillPath(geom.stopBoxPaths[i], stop.color);

        painter.strokePath(geom.stopArrowPaths[i], blackPen);
        painter.strokePath(geom.stopBoxPaths[i], blackPen);
    }
}


StopEditorGeometry GradientStopEditorWidget::createGeometry()
{
    StopEditorGeometry geom;

    // Input Variables
    float xPad = 6;         // Pad the widget rect to allow arrows to stick out from gradient strip
    float yPad = 2;         // Y-Pad the widget rect a little to allow strokes not to be clipped
    float stopWidth = 10;    // Width of a stop point (arrow + color box)
    float stopHeight = 20;  // Height of a stop point (arrow + color box)
    float arrowHeight = 10;   // Height and Width of stop point arrow

    geom.bounds = QRectF(rect().x() + xPad, rect().y() + yPad, rect().width() - (2*xPad), rect().height() - (2*yPad));
    geom.gradientRect = QRectF(geom.bounds.x(), geom.bounds.y(), geom.bounds.width(), geom.bounds.height() - stopHeight);
    geom.gradientStripPath.addRect(geom.gradientRect);
    geom.stopAreaRect = QRectF(geom.bounds.x(), geom.bounds.y() + geom.bounds.height() - stopHeight, geom.bounds.width(), stopHeight);

    QVector<QPoint> arrowVerts = QVector<QPoint>() << QPoint(0,0) << QPoint(-stopWidth*0.5f, arrowHeight) << QPoint(stopWidth*0.5f, arrowHeight) << QPoint(0,0);

    for( auto it = mGradient.stops.begin(); it != mGradient.stops.end(); ++it )
    {
        SGFGradientStop stop = (*it);

        float x = geom.bounds.x() + (stop.position * geom.bounds.width());
        float y = geom.stopAreaRect.top();

        QPainterPath arrowPath;
        arrowPath.addPolygon(QPolygon(arrowVerts));
        arrowPath.translate(x, y);

        QPainterPath boxPath;
        boxPath.addRect(QRectF(x-(stopWidth * 0.5f), y+arrowHeight, stopWidth, stopHeight-arrowHeight));

        geom.stopBounds.push_back(QRectF(x-(stopWidth * 0.5f), y, stopWidth, stopHeight));
        geom.stopArrowPaths.push_back(arrowPath);
        geom.stopBoxPaths.push_back(boxPath);
    }

    return geom;
}


int GradientStopEditorWidget::stopHandleAtPoint(StopEditorGeometry &geom, QPoint globalPoint)
{
    QPoint p = mapTo(this, globalPoint);

    for( int i = 0; i < geom.stopBounds.size(); ++i )
    {
        if ( geom.stopBounds[i].contains(p) )
        {
            return i;
        }
    }

    return -1;
}


float GradientStopEditorWidget::stopPositionAtPoint(StopEditorGeometry &geom, QPoint globalPoint)
{
    QPoint mouse = mapTo(this, globalPoint);

    float value = (mouse.x() - geom.stopAreaRect.left()) / geom.stopAreaRect.width();
    value = qMax(qMin(value, 1.0f), 0.0f);

    return value;
}


void GradientStopEditorWidget::showColorDialogForStop(int index)
{
    SGFGradientStop & stop = mGradient.stops[index];

    QColor color = QColorDialog::getColor(stop.color, this, "Select Color",  QColorDialog::ShowAlphaChannel);

    if ( color.isValid() )
    {
        stop.color = color;
        emit gradientChanged(mGradient);
        update();
    }
}

