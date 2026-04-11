#ifndef SGFEFFECTTYPES_H
#define SGFEFFECTTYPES_H

#include <QMetaType>
#include <QString>
#include <QStringList>

enum SGFEffectType
{
    Effect_Fill,
    Effect_Shadow,
    Effect_Stroke,
    Effect_ShadedMaterial,
    Effect_Unknown
};

Q_DECLARE_METATYPE(SGFEffectType)


enum SGFBlendMode
{
    Blend_Normal,
    Blend_Multiply,
    Blend_Screen,
    Blend_Overlay,
    Blend_Lighten,
    Blend_Darken,
    Blend_ColorDodge,
    Blend_ColorBurn,
    Blend_SoftLight,
    Blend_HardLight,
    Blend_Difference,
    Blend_Exclusion,
    Blend_Unknown
};

Q_DECLARE_METATYPE(SGFBlendMode)


enum SGFFillType
{
    Fill_Color,
    Fill_Gradient,
    Fill_Pattern,
    Fill_Unknown
};

Q_DECLARE_METATYPE(SGFFillType)


enum SGFStrokePosition
{
    Stroke_Center,
    Stroke_Inside,
    Stroke_Outside,
    Stroke_Unknown
};

Q_DECLARE_METATYPE(SGFStrokePosition)


enum SGFShadowType
{
    Shadow_Outer,
    Shadow_Inner,
    Shadow_Unknown
};

Q_DECLARE_METATYPE(SGFShadowType)


/**
 * @brief A class containing static methods for manipulating SGF Effect Types
 */
class SGFEffectTypes
{

public:
    static SGFEffectType EffectTypeFromString(const QString & value);
    static QString EffectTypeToString(SGFEffectType value);

    static QStringList BlendModeOptions();
    static SGFBlendMode BlendModeFromString(const QString & value);
    static QString BlendModeToString(SGFBlendMode value);

    static QStringList FillTypeOptions();
    static SGFFillType FillTypeFromString(const QString & value);
    static QString FillTypeToString(SGFFillType value);

    static QStringList StrokePositionOptions();
    static SGFStrokePosition StrokePositionFromString(const QString & value);
    static QString StrokePositionToString(SGFStrokePosition value);

    static QStringList ShadowTypeOptions();
    static SGFShadowType ShadowTypeFromString(const QString & value);
    static QString ShadowTypeToString(SGFShadowType value);

};


#endif // SGFEFFECTTYPES_H
