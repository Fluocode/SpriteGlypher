#include "SGFEffectTypes.h"


// TODO: Convert all text strings to constants!


SGFEffectType SGFEffectTypes::EffectTypeFromString(const QString & value)
{
    QString lower = value.toLower();

    if      ( lower.compare("fill") == 0 )        { return SGFEffectType::Effect_Fill; }
    else if ( lower.compare("shadow") == 0 )      { return SGFEffectType::Effect_Shadow; }
    else if ( lower.compare("stroke") == 0 )      { return SGFEffectType::Effect_Stroke; }
    else if ( lower.compare("shadedmaterial") == 0 || lower.compare("shaded material") == 0 ) {
        return SGFEffectType::Effect_ShadedMaterial;
    }

    return SGFEffectType::Effect_Unknown;
}


QString SGFEffectTypes::EffectTypeToString(SGFEffectType value)
{
    switch(value) {
        case SGFEffectType::Effect_Fill:    return QString("Fill");
        case SGFEffectType::Effect_Shadow:  return QString("Shadow");
        case SGFEffectType::Effect_Stroke:  return QString("Stroke");
        case SGFEffectType::Effect_ShadedMaterial: return QString("Shaded Material");
        default: return QString("Unknown");
    }
}


QStringList SGFEffectTypes::BlendModeOptions()
{
    return QStringList() << "Normal" << "Multiply" << "Screen" << "Overlay" << "Lighten" << "Darken"
                         << "Color Dodge" << "Color Burn" << "Soft Light" << "Hard Light" << "Difference" << "Exclusion";
}


SGFBlendMode SGFEffectTypes::BlendModeFromString(const QString & value)
{
    QString lower = value.toLower();

    if      ( lower.compare("normal") == 0 )        { return SGFBlendMode::Blend_Normal; }
    else if ( lower.compare("multiply") == 0 )      { return SGFBlendMode::Blend_Multiply; }
    else if ( lower.compare("screen") == 0 )        { return SGFBlendMode::Blend_Screen; }
    else if ( lower.compare("overlay") == 0 )       { return SGFBlendMode::Blend_Overlay; }
    else if ( lower.compare("lighten") == 0 )       { return SGFBlendMode::Blend_Lighten; }
    else if ( lower.compare("darken") == 0 )        { return SGFBlendMode::Blend_Darken; }
    else if ( lower.compare("color dodge") == 0 )   { return SGFBlendMode::Blend_ColorDodge; }
    else if ( lower.compare("color burn") == 0 )    { return SGFBlendMode::Blend_ColorBurn; }
    else if ( lower.compare("soft light") == 0 )    { return SGFBlendMode::Blend_SoftLight; }
    else if ( lower.compare("hard light") == 0 )    { return SGFBlendMode::Blend_HardLight; }
    else if ( lower.compare("difference") == 0 )    { return SGFBlendMode::Blend_Difference; }
    else if ( lower.compare("exclusion") == 0 )     { return SGFBlendMode::Blend_Exclusion; }

    return SGFBlendMode::Blend_Unknown;
}


QString SGFEffectTypes::BlendModeToString(SGFBlendMode value)
{
    switch(value) {
        case SGFBlendMode::Blend_Normal:    return QString("Normal");
        case SGFBlendMode::Blend_Multiply:  return QString("Multiply");
        case SGFBlendMode::Blend_Screen:    return QString("Screen");
        case SGFBlendMode::Blend_Overlay:   return QString("Overlay");
        case SGFBlendMode::Blend_Lighten:   return QString("Lighten");
        case SGFBlendMode::Blend_Darken:    return QString("Darken");
        case SGFBlendMode::Blend_ColorDodge: return QString("Color Dodge");
        case SGFBlendMode::Blend_ColorBurn: return QString("Color Burn");
        case SGFBlendMode::Blend_SoftLight: return QString("Soft Light");
        case SGFBlendMode::Blend_HardLight: return QString("Hard Light");
        case SGFBlendMode::Blend_Difference: return QString("Difference");
        case SGFBlendMode::Blend_Exclusion: return QString("Exclusion");
        default: return QString("Unknown");
    }
}


QStringList SGFEffectTypes::FillTypeOptions()
{
    return QStringList() << "Color" << "Gradient" << "Pattern";
}


SGFFillType SGFEffectTypes::FillTypeFromString(const QString & value)
{
    QString lower = value.toLower();

    if ( lower.compare("color") == 0 ) {
        return SGFFillType::Fill_Color;
    } else if ( lower.compare("gradient") == 0 ) {
        return SGFFillType::Fill_Gradient;
    } else if ( lower.compare("pattern") == 0 ) {
        return SGFFillType::Fill_Pattern;
    }

    return SGFFillType::Fill_Unknown;
}


QString SGFEffectTypes::FillTypeToString(SGFFillType value)
{
    switch(value) {
        case SGFFillType::Fill_Color: return QString("Color");
        case SGFFillType::Fill_Gradient: return QString("Gradient");
        case SGFFillType::Fill_Pattern: return QString("Pattern");
        default: return QString("Unknown");
    }
}


QStringList SGFEffectTypes::StrokePositionOptions()
{
    return QStringList() << "Center" << "Inside" << "Outside";
}


SGFStrokePosition SGFEffectTypes::StrokePositionFromString(const QString & value)
{
    QString lower = value.toLower();

    if      ( lower.compare("center") == 0 )    { return SGFStrokePosition::Stroke_Center; }
    else if ( lower.compare("inside") == 0 )    { return SGFStrokePosition::Stroke_Inside; }
    else if ( lower.compare("outside") == 0 )   { return SGFStrokePosition::Stroke_Outside; }

    return SGFStrokePosition::Stroke_Unknown;
}


QString SGFEffectTypes::StrokePositionToString(SGFStrokePosition value)
{
    switch(value) {
        case SGFStrokePosition::Stroke_Center:  return QString("Center");
        case SGFStrokePosition::Stroke_Inside:  return QString("Inside");
        case SGFStrokePosition::Stroke_Outside: return QString("Outside");
        default: return QString("Unknown");
    }
}


QStringList SGFEffectTypes::ShadowTypeOptions()
{
    return QStringList() << "Outer" << "Inner";
}


SGFShadowType SGFEffectTypes::ShadowTypeFromString(const QString & value)
{
    QString lower = value.toLower();

    if      ( lower.compare("outer") == 0 )    { return SGFShadowType::Shadow_Outer; }
    else if ( lower.compare("inner") == 0 )    { return SGFShadowType::Shadow_Inner; }

    return SGFShadowType::Shadow_Unknown;
}


QString SGFEffectTypes::ShadowTypeToString(SGFShadowType value)
{
    switch(value) {
        case SGFShadowType::Shadow_Outer:  return QString("Outer");
        case SGFShadowType::Shadow_Inner:  return QString("Inner");
        default: return QString("Unknown");
    }
}
