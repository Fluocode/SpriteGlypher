#ifndef SGFSHADOWEFFECT_H
#define SGFSHADOWEFFECT_H

#include "SGFEffect.h"

struct SGFShadowEffectSettings
{
    bool enabled;               //< Is this effect enabled?
    SGFBlendMode blendMode;     //< Blend mode for this effect.
    float opacity;              //< Blend opacity for this effect.
    QColor color;               //< Shadow Color.
    SGFShadowType shadowType;   //< Inner or outer shadow?
    float angle;                //< Angle in degrees.
    float distance;             //< Distance in pixels.
    float blur;                 //< Blur amount in pixels.
    float size;                 //< Size of shadow in pixels.

    SGFShadowEffectSettings() :
        enabled(true),
        blendMode(SGFBlendMode::Blend_Normal),
        opacity(1.0f),
        color(QColor(0,0,0,255)),
        shadowType(SGFShadowType::Shadow_Outer),
        angle(45),
        distance(1),
        blur(1),
        size(0)
    {

    }
};

Q_DECLARE_METATYPE(SGFShadowEffectSettings);


class SGFShadowEffect : public SGFEffect
{
public:
    typedef std::shared_ptr<SGFShadowEffect> Ptr;

public:
    static const QString kColorKey;
    static const QString kShadowTypeKey;
    static const QString kAngleKey;
    static const QString kDistanceKey;
    static const QString kBlurKey;
    static const QString kSizeKey;

public:
    SGFShadowEffect();

    bool affectsAdvance() const override { return false; }


    // Abstract Methods
public:

    ///** Get the minimum size required to render this effect on the given glyph. */
    //virtual QSize sizeRequiredForGlyph(SGFGlyph& glyph) = 0;

    /** Return a clone (deep copy) of this effect so that we can duplicate effects on the object. */
    virtual SGFEffect::Ptr clone() { return SGFEffect::Ptr(new SGFShadowEffect(*this)); }

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
    SGFShadowEffectSettings getSettings();

    /** Set settings from struct. */
    void setSettings(SGFShadowEffectSettings settings);


    // Internal Methods
protected:
    /** Get a blurred verison of the provided image. */
    QImage blurImage(const QImage& image, int radius, bool alphaOnly = false);

};

#endif // SGFSHADOWEFFECT_H
