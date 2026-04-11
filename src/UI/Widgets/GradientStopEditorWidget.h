#ifndef GRADIENTSTOPEDITORWIDGET_H
#define GRADIENTSTOPEDITORWIDGET_H

#include <QWidget>
#include "../../Model/SGFTypes.h"


struct StopEditorGeometry {
    QRectF bounds;
    QRectF gradientRect;
    QPainterPath gradientStripPath;
    QRectF stopAreaRect;
    QVector<QRectF> stopBounds;
    QVector<QPainterPath> stopArrowPaths;
    QVector<QPainterPath> stopBoxPaths;
};


class GradientStopEditorWidget : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(SGFGradient gradient READ gradient WRITE setGradient)

public:
    explicit GradientStopEditorWidget(QWidget *parent = 0);
    ~GradientStopEditorWidget();


    // PUblic Methods
public:
    /** Get the displayed gradient. */
    SGFGradient gradient() const;

    /** Set the displayed gradient and update the widget. */
    void setGradient(const SGFGradient& gradient);

    /** Update the current gradient, without resetting the selected stop etc. */
    void updateGradient(const SGFGradient& gradient);

    /** Get the selected gradient stop. */
    int selectedGradientStop();

    /** Select the given gradient stop. */
    void setSelectedGradientStop(int index);




    // Widget Events
protected:

    /** When the user clicks on this widget, we want to present a gradient builder dialog. */
    virtual void mousePressEvent(QMouseEvent *ev);

    /** Support dragging of stops to different positions. */
    virtual void mouseMoveEvent(QMouseEvent *ev);

    /** When releasing the left mouse button, stop dragging a stop. */
    virtual void mouseReleaseEvent(QMouseEvent *ev);

    /** When the user double clicks on a color swatch, pop up a color change dialog. */
    virtual void mouseDoubleClickEvent(QMouseEvent *ev);

    /** Updated draw method as we want to be able to draw a nice background. */
    virtual void paintEvent(QPaintEvent *ev);


    // Internal Methods
protected:

    /** Generate the current geometry for the stop editor for drawing / click detection. */
    StopEditorGeometry createGeometry();

    /** Given some geometry, determine if the given point is under a 'stop handle'. */
    int stopHandleAtPoint(StopEditorGeometry& geom, QPoint globalPoint);

    /** Given some geometry and a point, determine what position the mouse is at horizontally on the strip. */
    float stopPositionAtPoint(StopEditorGeometry& geom, QPoint globalPoint);

    /** Present a color dialog to change the color of the given stop. */
    void showColorDialogForStop(int index);


    // Signals
signals:
    void gradientChanged(SGFGradient);
    void selectedStopChanged(int);


    // Member Variables
private:
    SGFGradient mGradient;
    int mSelectedStop;
    int mDraggingStop;

};

#endif // GRADIENTSTOPEDITORWIDGET_H
