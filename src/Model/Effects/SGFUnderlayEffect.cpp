#include "SGFUnderlayEffect.h"
#include "SGFInnerBevel.h"

#include <cmath>
#include <QtMath>

const QString SGFUnderlayEffect::kFillTypeKey = QString("underlayFillType");
const QString SGFUnderlayEffect::kColorKey = QString("underlayColor");
const QString SGFUnderlayEffect::kGradientKey = QString("underlayGradient");
const QString SGFUnderlayEffect::kAngleKey = QString("underlayAngle");
const QString SGFUnderlayEffect::kDistanceKey = QString("underlayDistance");
const QString SGFUnderlayEffect::kSizeKey = QString("underlaySize");
const QString SGFUnderlayEffect::kBevelAmountKey = QString("underlayBevelAmount");
const QString SGFUnderlayEffect::kBevelAngleKey = QString("underlayBevelAngle");
const QString SGFUnderlayEffect::kBevelBlurKey = QString("underlayBevelBlur");
const QString SGFUnderlayEffect::kBevelIntensityKey = QString("underlayBevelIntensity");

SGFUnderlayEffect::SGFUnderlayEffect()
{
    mEffectType = SGFEffectType::Effect_Underlay;
}

void SGFUnderlayEffect::scaleEffect(float factor)
{
    SGFUnderlayEffectSettings settings = getSettings();
    settings.distance *= factor;
    settings.size *= factor;
    settings.bevelBlur *= factor;
    setSettings(settings);
}

SGFPadding SGFUnderlayEffect::paddingRequiredForGlyph(SGFGlyph& /*glyph*/)
{
    SGFUnderlayEffectSettings settings = getSettings();

    const float padding = settings.size; // no blur
    const float radians = qDegreesToRadians(settings.angle);
    const float dx = std::cos(radians) * settings.distance;
    const float dy = std::sin(radians) * settings.distance;

    // Same convention as ShadowEffect outer.
    return SGFPadding(-dy + padding, dx + padding, dy + padding, -dx + padding);
}

void SGFUnderlayEffect::applyToGlyph(SGFGlyph& glyph)
{
    SGFUnderlayEffectSettings settings = getSettings();
    QPainterPath path = glyph.path;

    if ( settings.size > 0 ) {
        path = growPath(path, settings.size);
    }

    if ( settings.distance != 0 ) {
        const float radians = qDegreesToRadians(settings.angle);
        const float dx = std::cos(radians) * settings.distance;
        const float dy = std::sin(radians) * settings.distance;
        path.translate(dx, dy);
    }

    QPainter painter(&glyph.backgroundImage);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setOpacity(settings.opacity);
    painter.setCompositionMode(compositionForBlendMode(mBlendMode));

    if ( settings.fillType == SGFFillType::Fill_Gradient ) {
        settings.gradient.fillPath(painter, path);
    } else {
        painter.fillPath(path, settings.color);
    }
    painter.end();

    if ( settings.bevelAmount > 0 ) {
        SGFInnerBevel::applyToImage(
            glyph.backgroundImage,
            path,
            static_cast<float>(settings.bevelAmount),
            settings.bevelAngle,
            settings.bevelBlur,
            settings.opacity,
            settings.bevelIntensity);
    }
}

void SGFUnderlayEffect::setDefaultParameters()
{
    setSettings(SGFUnderlayEffectSettings());
}

SGFUnderlayEffectSettings SGFUnderlayEffect::getSettings()
{
    SGFUnderlayEffectSettings settings;
    settings.enabled = mEnabled;
    settings.blendMode = mBlendMode;
    settings.opacity = mOpacity;
    settings.fillType = mParameters[kFillTypeKey].value<SGFFillType>();
    settings.color = mParameters[kColorKey].value<QColor>();
    settings.gradient = mParameters[kGradientKey].value<SGFGradient>();
    settings.angle = mParameters[kAngleKey].value<float>();
    settings.distance = mParameters[kDistanceKey].value<float>();
    settings.size = mParameters[kSizeKey].value<float>();
    settings.bevelAmount = mParameters.value(kBevelAmountKey, 0).toInt();
    settings.bevelAngle = mParameters.value(kBevelAngleKey, 135.0f).toFloat();
    settings.bevelBlur = mParameters.value(kBevelBlurKey, 0.0f).toFloat();
    settings.bevelIntensity = mParameters.value(kBevelIntensityKey, 100.0f).toFloat();
    return settings;
}

void SGFUnderlayEffect::setSettings(SGFUnderlayEffectSettings settings)
{
    mEnabled = settings.enabled;
    mBlendMode = settings.blendMode;
    mOpacity = settings.opacity;

    mParameters[kFillTypeKey] = QVariant::fromValue<SGFFillType>(settings.fillType);
    mParameters[kColorKey] = QVariant::fromValue<QColor>(settings.color);
    mParameters[kGradientKey] = QVariant::fromValue<SGFGradient>(settings.gradient);
    mParameters[kAngleKey] = QVariant::fromValue<float>(settings.angle);
    mParameters[kDistanceKey] = QVariant::fromValue<float>(settings.distance);
    mParameters[kSizeKey] = QVariant::fromValue<float>(settings.size);
    mParameters[kBevelAmountKey] = QVariant::fromValue<int>(settings.bevelAmount);
    mParameters[kBevelAngleKey] = QVariant::fromValue<float>(settings.bevelAngle);
    mParameters[kBevelBlurKey] = QVariant::fromValue<float>(settings.bevelBlur);
    mParameters[kBevelIntensityKey] = QVariant::fromValue<float>(settings.bevelIntensity);
}

bool SGFUnderlayEffect::writeSubclassToXmlStream(QXmlStreamWriter &writer)
{
    SGFUnderlayEffectSettings settings = getSettings();

    // Persist only Color/Gradient; pattern is not supported.
    SGFFillType ft = settings.fillType;
    if ( ft != SGFFillType::Fill_Gradient ) {
        ft = SGFFillType::Fill_Color;
    }

    writer.writeAttribute(kFillTypeKey, SGFEffectTypes::FillTypeToString(ft));
    writer.writeAttribute(kColorKey, settings.color.name(QColor::HexArgb));
    writer.writeAttribute(kGradientKey, settings.gradient.stringRepresentation());
    writer.writeAttribute(kAngleKey, QString::number(settings.angle));
    writer.writeAttribute(kDistanceKey, QString::number(settings.distance));
    writer.writeAttribute(kSizeKey, QString::number(settings.size));
    writer.writeAttribute(kBevelAmountKey, QString::number(settings.bevelAmount));
    writer.writeAttribute(kBevelAngleKey, QString::number(settings.bevelAngle));
    writer.writeAttribute(kBevelBlurKey, QString::number(settings.bevelBlur));
    writer.writeAttribute(kBevelIntensityKey, QString::number(settings.bevelIntensity));
    return true;
}

void SGFUnderlayEffect::readSubclassFromXmlNode(const QDomElement &element)
{
    SGFUnderlayEffectSettings settings = getSettings();

    const QString xmlFillType = element.attribute(kFillTypeKey);
    const QString xmlColor = element.attribute(kColorKey);
    const QString xmlGradient = element.attribute(kGradientKey);
    const QString xmlAngle = element.attribute(kAngleKey);
    const QString xmlDistance = element.attribute(kDistanceKey);
    const QString xmlSize = element.attribute(kSizeKey);
    const QString xmlBevelAmount = element.attribute(kBevelAmountKey);
    const QString xmlBevelAngle = element.attribute(kBevelAngleKey);
    const QString xmlBevelBlur = element.attribute(kBevelBlurKey);
    const QString xmlBevelIntensity = element.attribute(kBevelIntensityKey);

    if ( !xmlFillType.isEmpty() ) { settings.fillType = SGFEffectTypes::FillTypeFromString(xmlFillType); }
    if ( settings.fillType != SGFFillType::Fill_Gradient ) {
        settings.fillType = SGFFillType::Fill_Color;
    }
    if ( !xmlColor.isEmpty() ) { settings.color = QColor(xmlColor); }
    if ( !xmlGradient.isEmpty() ) { settings.gradient = SGFGradient(xmlGradient); }
    if ( !xmlAngle.isEmpty() ) { settings.angle = xmlAngle.toFloat(); }
    if ( !xmlDistance.isEmpty() ) { settings.distance = xmlDistance.toFloat(); }
    if ( !xmlSize.isEmpty() ) { settings.size = xmlSize.toFloat(); }
    if ( !xmlBevelAmount.isEmpty() ) { settings.bevelAmount = xmlBevelAmount.toInt(); }
    if ( !xmlBevelAngle.isEmpty() ) { settings.bevelAngle = xmlBevelAngle.toFloat(); }
    if ( !xmlBevelBlur.isEmpty() ) { settings.bevelBlur = xmlBevelBlur.toFloat(); }
    if ( !xmlBevelIntensity.isEmpty() ) { settings.bevelIntensity = xmlBevelIntensity.toFloat(); }

    setSettings(settings);
}

