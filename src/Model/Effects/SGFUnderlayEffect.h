#ifndef SGFUNDERLAYEFFECT_H
#define SGFUNDERLAYEFFECT_H

#include "SGFEffect.h"
#include "../SGFGradient.h"

struct SGFUnderlayEffectSettings
{
    bool enabled;
    SGFBlendMode blendMode;
    float opacity;

    // Underlay fill (solid or gradient). Pattern intentionally not supported.
    SGFFillType fillType;
    QColor color;
    SGFGradient gradient;

    // Geometry (outer-only, no blur).
    float angle;      // degrees
    float distance;   // px
    float size;       // px (grow/shrink amount)
    int bevelAmount;  // 0 = off; inner bevel on underlay (0–100)
    float bevelAngle;
    float bevelBlur;
    float bevelIntensity;

    SGFUnderlayEffectSettings()
        : enabled(true)
        , blendMode(SGFBlendMode::Blend_Normal)
        , opacity(1.0f)
        , fillType(SGFFillType::Fill_Color)
        , color(QColor(0,0,0,255))
        , gradient()
        , angle(45.0f)
        , distance(1.0f)
        , size(0.0f)
        , bevelAmount(0)
        , bevelAngle(135.0f)
        , bevelBlur(0.0f)
        , bevelIntensity(100.0f)
    {
        gradient.setDefaultLinearGradient();
    }
};

Q_DECLARE_METATYPE(SGFUnderlayEffectSettings);

class SGFUnderlayEffect : public SGFEffect
{
public:
    typedef std::shared_ptr<SGFUnderlayEffect> Ptr;

public:
    static const QString kFillTypeKey;
    static const QString kColorKey;
    static const QString kGradientKey;
    static const QString kAngleKey;
    static const QString kDistanceKey;
    static const QString kSizeKey;
    static const QString kBevelAmountKey;
    static const QString kBevelAngleKey;
    static const QString kBevelBlurKey;
    static const QString kBevelIntensityKey;

public:
    SGFUnderlayEffect();

    SGFEffect::Ptr clone() override { return SGFEffect::Ptr(new SGFUnderlayEffect(*this)); }

    SGFPadding paddingRequiredForGlyph(SGFGlyph& glyph) override;
    void applyToGlyph(SGFGlyph& glyph) override;
    void setDefaultParameters() override;
    void scaleEffect(float factor) override;

protected:
    bool writeSubclassToXmlStream(QXmlStreamWriter &writer) override;
    void readSubclassFromXmlNode(const QDomElement &element) override;

public:
    SGFUnderlayEffectSettings getSettings();
    void setSettings(SGFUnderlayEffectSettings settings);
};

#endif // SGFUNDERLAYEFFECT_H

