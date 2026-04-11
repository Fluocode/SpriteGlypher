#ifndef SGFGRADIENT_H
#define SGFGRADIENT_H

#include <QColor>
#include <QGradient>
#include <QLinearGradient>
#include <QMetaType>
#include <QPainter>
#include <QPainterPath>
#include <QRadialGradient>
#include <QRectF>
#include <QString>
#include <QVector>


enum SGFGradientStyle
{
    Gradient_Unknown,
    Gradient_Linear,
    Gradient_Radial,
    Gradient_Conical
};


struct SGFGradientStop
{
    static const QString kColorKey;
    static const QString kPositionKey;

    QColor color;
    float position;

    SGFGradientStop();
    SGFGradientStop(const QString &descriptor);
    SGFGradientStop(const QColor& pColor, float pPosition);

    QString toString();

    void fromString(const QString &value);
};

Q_DECLARE_METATYPE(SGFGradientStop)


class SGFGradient
{
    //
    // Constants
    //
private:
    static const QString kGradientStyleUnknown;
    static const QString kGradientStyleLinear;
    static const QString kGradientStyleRadial;
    static const QString kGradientStyleConical;

    static const QString kStyleKey;
    static const QString kStopsKey;
    static const QString kAngleKey;
    static const QString kScaleKey;


    //
    // Static Methods
    //
public:
    /** Convert a gradient style enum value to a text representation. */
    static QString GradientStyleToString(const SGFGradientStyle &gradientStyle);

    /** Convert a text representation of a gradient style back to an enum value. */
    static SGFGradientStyle GradientStyleFromString(const QString &value);


    //
    // Object Lifecycle
    //
public:
    /** Construct a new gradient, which isn't valid to start with. */
    SGFGradient();

    /** Construct a new gradient, from a descriptive string. */
    SGFGradient(const QString &stringRepresentation);

    /** Copy Constructor. */
    SGFGradient(const SGFGradient &other);

    SGFGradient &operator=(const SGFGradient &other) = default;

    /** Set this gradient to be a default linear gradient. */
    void setDefaultLinearGradient();

    /** Is this gradient valid? */
    bool isValid();

    /** Get a drawable linear gradient to fill a horizontal 'gradient stop editor' (even if this isn't a linear gradient). */
    QLinearGradient linearGradientForEditor(const QRectF & rect);

    /** Get a drawable linear gradient to fill the given rect from this gradient. */
    QLinearGradient linearGradientForRect(const QRectF & rect);

    /** Get a drawable radial gradient to fill the given rect from this gradient. */
    QRadialGradient radialGradientForRect(const QRectF & rect);

    /** Apply this gradient, using the given painter, in the provided rect. */
    void fillRect(QPainter& painter, const QRectF & rect);

    /** Apply this gradient, using the given painter, in the provided path. */
    void fillPath(QPainter& painter, const QPainterPath & path);

    /** Convert a gradient to a string representation for serialization. */
    QString stringRepresentation();

    /** Read in the values for this gradient object from a descriptive string. */
    void fromStringRepresentation(const QString &value);


    // Public properties
public:
    SGFGradientStyle style;
    QVector<SGFGradientStop> stops;
    float angle;
    float scale;

};

Q_DECLARE_METATYPE(SGFGradient)

#endif // SGFGRADIENT_H
