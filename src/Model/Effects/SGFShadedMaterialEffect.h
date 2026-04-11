#ifndef SGFSHADEDMATERIALEFFECT_H
#define SGFSHADEDMATERIALEFFECT_H

#include <QString>

#include "SGFEffect.h"

struct SGFShadedMaterialEffectSettings
{
    bool enabled;
    SGFBlendMode blendMode;
    float opacity;
    float lightAngle;
    float lightElevation;
    float bumpScale;
    float ambient;
    int smoothRadius;
    /** Mixes a dome-shaped height from the glyph bbox center so interiors catch light (0 = outline-only). */
    float interiorRelief;
    /** Thickens the height ridge along the glyph medial axis (balloon / inflated look). 0 = off. */
    float inflateAmount;
    /** Blinn–Phong-style highlight strength (0 = off). */
    float specularStrength;
    /** Specular exponent; higher = tighter highlights. */
    float specularPower;
    /** 0 = classic lighting only; 1 = full matcap tint (surface lit from sphere texture by normal). */
    float matcapMix;
    /** Absolute path to matcap image (square PNG/JPEG). Empty = built-in default. */
    QString matcapImagePath;
    QColor lightColor;
    QColor flatBaseColor;
    bool forceFlatBase;

    SGFShadedMaterialEffectSettings()
        : enabled(true)
        , blendMode(SGFBlendMode::Blend_Normal)
        , opacity(1.0f)
        , lightAngle(135.f)
        , lightElevation(42.f)
        , bumpScale(5.5f)
        , ambient(0.22f)
        , smoothRadius(2)
        , interiorRelief(0.88f)
        , specularStrength(0.45f)
        , specularPower(56.f)
        , lightColor(255, 255, 255, 255)
        , flatBaseColor(200, 200, 210, 255)
        , forceFlatBase(false)
    {
    }
};

Q_DECLARE_METATYPE(SGFShadedMaterialEffectSettings);


class SGFShadedMaterialEffect : public SGFEffect
{
public:
    static const QString kLightAngleKey;
    static const QString kLightElevationKey;
    static const QString kBumpScaleKey;
    static const QString kAmbientKey;
    static const QString kSmoothRadiusKey;
    static const QString kLightColorKey;
    static const QString kFlatBaseColorKey;
    static const QString kForceFlatBaseKey;
    static const QString kInteriorReliefKey;
    static const QString kSpecularStrengthKey;
    static const QString kSpecularPowerKey;
    static const QString kInflateAmountKey;
    static const QString kMatcapMixKey;
    static const QString kMatcapImagePathKey;

    SGFShadedMaterialEffect();

    SGFEffect::Ptr clone() override { return SGFEffect::Ptr(new SGFShadedMaterialEffect(*this)); }

    SGFPadding paddingRequiredForGlyph(SGFGlyph& /*glyph*/) override { return SGFPadding(0, 0, 0, 0); }

    void applyToGlyph(SGFGlyph& glyph) override;

    void willApplyEffectToGlyphs(const QVector<SGFGlyph>& glyphs) override;

    void setDefaultParameters() override;

    void scaleEffect(float factor) override;

    bool writeSubclassToXmlStream(QXmlStreamWriter& writer) override;

    void readSubclassFromXmlNode(const QDomElement& element) override;

    SGFShadedMaterialEffectSettings getSettings();
    void setSettings(SGFShadedMaterialEffectSettings settings);

    QPixmap getPreview(int width, int height, int pointSize) override;

private:
    static void blurHeightMap(QVector<float>& hmap, int w, int h, int radius);
    static float sampleH(const QVector<float>& hmap, int w, int h, int x, int y);
    static void applyInteriorDomeHeight(QVector<float>& hmap, int w, int h, const QImage& alphaRef, float strength);
    static void applyInflateHeight(QVector<float>& hmap, int w, int h, const QImage& alphaRef, float strength);

    static QImage defaultMatcapImage();
    static QImage loadMatcapImage(const QString& path);
    static QRgb sampleMatcapBilinear(const QImage& matcap, float u, float v);

    /** Loaded once per atlas pass (thread-safe read in applyToGlyph). */
    QImage mMatcapForPass;
};

#endif
