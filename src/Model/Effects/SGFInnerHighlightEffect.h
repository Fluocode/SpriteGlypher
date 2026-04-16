#ifndef SGFINNERHIGHLIGHTEFFECT_H
#define SGFINNERHIGHLIGHTEFFECT_H

#include "SGFEffect.h"
#include "../SGFGradient.h"

struct SGFInnerHighlightEffectSettings
{
    bool enabled;
    SGFBlendMode blendMode;
    float opacity;

    SGFFillType fillType;   // Color or Gradient
    QColor color;
    SGFGradient gradient;

    int inset;              // shrink path before painting
    float heightFrac;       // 0..1 fraction of glyph height to apply highlight to (from top)
    float soften;           // 0..1 softness on the cutoff edge

    SGFInnerHighlightEffectSettings()
        : enabled(true)
        , blendMode(SGFBlendMode::Blend_Screen)
        , opacity(0.85f)
        , fillType(SGFFillType::Fill_Gradient)
        , color(QColor(255,255,255,255))
        , gradient()
        , inset(2)
        , heightFrac(0.35f)
        , soften(0.25f)
    {
        gradient.setDefaultLinearGradient();
    }
};

Q_DECLARE_METATYPE(SGFInnerHighlightEffectSettings);

class SGFInnerHighlightEffect : public SGFEffect
{
public:
    typedef std::shared_ptr<SGFInnerHighlightEffect> Ptr;

public:
    static const QString kFillTypeKey;
    static const QString kColorKey;
    static const QString kGradientKey;
    static const QString kInsetKey;
    static const QString kHeightFracKey;
    static const QString kSoftenKey;

public:
    SGFInnerHighlightEffect();

    SGFEffect::Ptr clone() override { return SGFEffect::Ptr(new SGFInnerHighlightEffect(*this)); }
    bool affectsAdvance() const override { return false; }

    SGFPadding paddingRequiredForGlyph(SGFGlyph& glyph) override;
    void applyToGlyph(SGFGlyph& glyph) override;
    void setDefaultParameters() override;
    void scaleEffect(float factor) override;

protected:
    bool writeSubclassToXmlStream(QXmlStreamWriter &writer) override;
    void readSubclassFromXmlNode(const QDomElement &element) override;

public:
    SGFInnerHighlightEffectSettings getSettings();
    void setSettings(SGFInnerHighlightEffectSettings settings);
};

#endif // SGFINNERHIGHLIGHTEFFECT_H

