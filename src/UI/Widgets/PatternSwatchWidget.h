#ifndef PATTERNSWATCHWIDGET_H
#define PATTERNSWATCHWIDGET_H

#include <QtCore>
#include <QWidget>
#include "../../Model/SGFTypes.h"

class PatternSwatchWidget : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(SGFPattern patternValue READ patternValue WRITE setPatternValue)


    // Object Lifecycle
public:

    PatternSwatchWidget(QWidget * parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());
    ~PatternSwatchWidget();

public:

    /** Get the pattern value. */
    SGFPattern patternValue() const;

    /** Set the pattern value and update the widget background. */
    void setPatternValue(const SGFPattern &pattern);


protected:

    /** When the user clicks on this widget, we want to present a color picker. */
    virtual void mousePressEvent(QMouseEvent *ev);

    /** Updated draw method as we want to be able to draw a nice background. */
    virtual void paintEvent(QPaintEvent *ev);


    // Signals
signals:
    void patternValueChanged(SGFPattern);

    // Member Variables
private:
    SGFPattern mPatternValue;
};

#endif // PATTERNSWATCHWIDGET_H
