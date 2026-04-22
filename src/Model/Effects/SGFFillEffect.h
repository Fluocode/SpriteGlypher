#ifndef SGFFILLEFFECT_H
#define SGFFILLEFFECT_H

#include "SGFEffect.h"
#include "../SGFTypes.h"

struct SGFFillEffectSettings
{
    bool enabled;               //< Is this effect enabled?
    SGFBlendMode blendMode;     //< Blend mode for this effect.
    float opacity;              //< Blend opacity for this effect.
    SGFFillType fillType;       //< Color, Gradient or Pattern Fill?
    QColor color;               //< Color - Used only for Color Fill Mode.
    SGFGradient gradient;       //< Gradient - Used only for Gradient Fill Mode.
    SGFPattern pattern;         //< Pattern Info - Used only for Pattern Fill Mode.
    int inset;                  //< Shrink the path by N pixels before filling (shine effects).
    int bevelAmount;            //< 0 = off; inner bevel strength (0–100).
    float bevelAngle;           //< Light direction (degrees).
    float bevelBlur;            //< Softens the bevel mask (pixels).
    float bevelIntensity;       //< 100 = default; higher = stronger relief.

    SGFFillEffectSettings() :
        enabled(true),
        blendMode(SGFBlendMode::Blend_Normal),
        opacity(1.0f),
        fillType(SGFFillType::Fill_Gradient),
        color(QColor(QStringLiteral("#d38e00"))),
        gradient(),
        pattern(),
        inset(0),
        bevelAmount(0),
        bevelAngle(135.0f),
        bevelBlur(0.0f)
    {
        gradient.setDefaultLinearGradient();
    }
};

Q_DECLARE_METATYPE(SGFFillEffectSettings);


class SGFFillEffect : public SGFEffect
{
public:
    typedef std::shared_ptr<SGFFillEffect> Ptr;

private:
    static const QString kPatternElementKey;

public:
    static const QString kFillTypeKey;
    static const QString kColorKey;
    static const QString kGradientKey;
    static const QString kPatternKey;
    static const QString kPatternScaleKey;
    static const QString kInsetKey;
    static const QString kBevelAmountKey;
    static const QString kBevelAngleKey;
    static const QString kBevelBlurKey;
    static const QString kBevelIntensityKey;



public:
    SGFFillEffect();


    // Abstract Methods
public:

    ///** Get the minimum size required to render this effect on the given glyph. */
    //virtual QSize sizeRequiredForGlyph(SGFGlyph& glyph) = 0;

    /** Return a clone (deep copy) of this effect so that we can duplicate effects on the object. */
    virtual SGFEffect::Ptr clone() { return SGFEffect::Ptr(new SGFFillEffect(*this)); }

    /** Get the amount of padding required over the base size for this glyph to accomodate for this effect. */
    virtual SGFPadding paddingRequiredForGlyph(SGFGlyph& glyph);

    /** Apply this effect to the given glyph. */
    virtual void applyToGlyph(SGFGlyph& glyph);

    /** Before we apply this effect, resize the input pattern if in pattern mode. */
    virtual void willApplyEffectToGlyphs(const QVector<SGFGlyph> &glyphs);

    /** Clear up the cached image. */
    virtual void didApplyEffectToGlyphs();

    /** Set default parameters for this effect. */
    virtual void setDefaultParameters();

    /** Scale the settings for this effect when the document is scaled to ensure stroke width etc are scaled appropriately. */
    virtual void scaleEffect(float factor);


protected:

    /** Write the parameters for this effect to the given XML stream. */
    virtual bool writeSubclassToXmlStream(QXmlStreamWriter &writer);

    /** Read the parameters for this effect from the given Xml Attributes. */
    virtual void readSubclassFromXmlNode(const QDomElement &element);


    // Internal Methods
public:
    /** Get a settings struct from the the effect's parameter hashmap. */
    SGFFillEffectSettings getSettings();

    /** Set settings from struct. */
    void setSettings(SGFFillEffectSettings settings);


    // Private Properties
private:
    QImage mCachedPatternImage;
};

#endif // SGFFILLEFFECT_H
