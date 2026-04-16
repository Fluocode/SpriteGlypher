#ifndef SGFOUTERGLOWEFFECT_H
#define SGFOUTERGLOWEFFECT_H

#include "SGFEffect.h"

struct SGFOuterGlowEffectSettings
{
    bool enabled;
    SGFBlendMode blendMode;
    float opacity;
    QColor color;
    float size;   // px grow
    float blur;   // px blur radius

    SGFOuterGlowEffectSettings()
        : enabled(true)
        , blendMode(SGFBlendMode::Blend_Screen)
        , opacity(1.0f)
        , color(QColor(255, 200, 64, 255))
        , size(6.0f)
        , blur(8.0f)
    {}
};

Q_DECLARE_METATYPE(SGFOuterGlowEffectSettings);

class SGFOuterGlowEffect : public SGFEffect
{
public:
    typedef std::shared_ptr<SGFOuterGlowEffect> Ptr;

public:
    static const QString kColorKey;
    static const QString kSizeKey;
    static const QString kBlurKey;

public:
    SGFOuterGlowEffect();

    SGFEffect::Ptr clone() override { return SGFEffect::Ptr(new SGFOuterGlowEffect(*this)); }
    bool affectsAdvance() const override { return false; }

    SGFPadding paddingRequiredForGlyph(SGFGlyph& glyph) override;
    void applyToGlyph(SGFGlyph& glyph) override;
    void setDefaultParameters() override;
    void scaleEffect(float factor) override;

protected:
    bool writeSubclassToXmlStream(QXmlStreamWriter &writer) override;
    void readSubclassFromXmlNode(const QDomElement &element) override;

public:
    SGFOuterGlowEffectSettings getSettings();
    void setSettings(SGFOuterGlowEffectSettings settings);

private:
    QImage blurImage(const QImage& image, int radius, bool alphaOnly = false);
};

#endif // SGFOUTERGLOWEFFECT_H

