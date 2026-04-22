#ifndef SGFSPARKLEOVERLAYEFFECT_H
#define SGFSPARKLEOVERLAYEFFECT_H

#include "SGFEffect.h"
#include "../SGFTypes.h"

struct SGFSparkleOverlayEffectSettings
{
    bool enabled;
    SGFBlendMode blendMode;
    float opacity;

    SGFPattern pattern;      // sparkle texture
    float scale;             // additional scale factor
    float rotation;          // degrees
    float offsetX;           // px
    float offsetY;           // px
    /** Widen the clip mask so pattern pixels near the ink edge are not hard-cut (px). */
    float edgeBleed;

    SGFSparkleOverlayEffectSettings()
        : enabled(true)
        , blendMode(SGFBlendMode::Blend_Screen)
        , opacity(0.9f)
        , pattern()
        , scale(1.0f)
        , rotation(0.0f)
        , offsetX(0.0f)
        , offsetY(0.0f)
        , edgeBleed(0.0f)
    {}
};

Q_DECLARE_METATYPE(SGFSparkleOverlayEffectSettings);

class SGFSparkleOverlayEffect : public SGFEffect
{
public:
    typedef std::shared_ptr<SGFSparkleOverlayEffect> Ptr;

public:
    static const QString kPatternElementKey;
    static const QString kPatternScaleKey;

    static const QString kOpacityKey2;
    static const QString kScaleKey;
    static const QString kRotationKey;
    static const QString kOffsetXKey;
    static const QString kOffsetYKey;
    static const QString kEdgeBleedKey;

public:
    SGFSparkleOverlayEffect();

    SGFEffect::Ptr clone() override { return SGFEffect::Ptr(new SGFSparkleOverlayEffect(*this)); }
    bool affectsAdvance() const override { return false; }

    SGFPadding paddingRequiredForGlyph(SGFGlyph& glyph) override;
    void applyToGlyph(SGFGlyph& glyph) override;
    void setDefaultParameters() override;
    void scaleEffect(float factor) override;

protected:
    bool writeSubclassToXmlStream(QXmlStreamWriter &writer) override;
    void readSubclassFromXmlNode(const QDomElement &element) override;

public:
    SGFSparkleOverlayEffectSettings getSettings();
    void setSettings(SGFSparkleOverlayEffectSettings settings);
};

#endif // SGFSPARKLEOVERLAYEFFECT_H

