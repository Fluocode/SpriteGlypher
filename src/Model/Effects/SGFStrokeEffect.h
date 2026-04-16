#ifndef SGFSTROKEEFFECT_H
#define SGFSTROKEEFFECT_H

#include "SGFEffect.h"
#include "../SGFTypes.h"


struct SGFStrokeEffectSettings
{
    bool enabled;               //< Is this effect enabled?
    SGFBlendMode blendMode;     //< Blend mode for this effect.
    float opacity;              //< Blend opacity for this effect.
    float width;                //< Stroke Width.
    SGFFillType fillType;       //< Color, Gradient or Pattern Stroke?
    QColor color;               //< Color - Used only for Color mode.
    SGFGradient gradient;       //< Gradient - Used only for Gradient mode.
    SGFPattern pattern;         //< Pattern - Used only for Pattern mode.
    SGFStrokePosition position; //< Stroke position.

    SGFStrokeEffectSettings() :
        enabled(true),
        blendMode(SGFBlendMode::Blend_Normal),
        opacity(1.0f),
        width(3),
        fillType(SGFFillType::Fill_Color),
        color(QColor(QStringLiteral("#876500"))),
        gradient(),
        pattern(),
        position(SGFStrokePosition::Stroke_Outside)
    {
        gradient.setDefaultLinearGradient();
    }
};

Q_DECLARE_METATYPE(SGFStrokeEffectSettings);


class SGFStrokeEffect : public SGFEffect
{
public:
    typedef std::shared_ptr<SGFStrokeEffect> Ptr;

public:
    static const QString kWidthKey;
    static const QString kFillTypeKey;
    static const QString kColorKey;
    static const QString kGradientKey;
    static const QString kPatternElementKey;
    static const QString kPatternScaleKey;
    static const QString kPositionKey;

public:
    SGFStrokeEffect();


    // Abstract Methods
public:

    ///** Get the minimum size required to render this effect on the given glyph. */
    //virtual QSize sizeRequiredForGlyph(SGFGlyph& glyph) = 0;

    /** Return a clone (deep copy) of this effect so that we can duplicate effects on the object. */
    virtual SGFEffect::Ptr clone() { return SGFEffect::Ptr(new SGFStrokeEffect(*this)); }

    /** Get the amount of padding required over the base size for this glyph to accomodate for this effect. */
    virtual SGFPadding paddingRequiredForGlyph(SGFGlyph& glyph);

    /** Apply this effect to the given glyph. */
    virtual void applyToGlyph(SGFGlyph& glyph);

    /** Set default parameters for this effect. */
    virtual void setDefaultParameters();

    /** Scale the settings for this effect when the document is scaled to ensure stroke width etc are scaled appropriately. */
    virtual void scaleEffect(float factor);


protected:

    /** Write the parameters for this effect to the given XML stream. */
    virtual bool writeSubclassToXmlStream(QXmlStreamWriter &writer);

    /** Read the parameters for this effect from the given Xml Attributes. */
    virtual void readSubclassFromXmlNode(const QDomElement &element);


    // Settings
public:

    /** Get a settings struct from the the effect's parameter hashmap. */
    SGFStrokeEffectSettings getSettings();

    /** Set settings from struct. */
    void setSettings(SGFStrokeEffectSettings settings);

};

#endif // SGFSTROKEEFFECT_H
