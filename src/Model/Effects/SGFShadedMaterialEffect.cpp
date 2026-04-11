#include "SGFShadedMaterialEffect.h"

#include <QFileInfo>
#include <QPainter>
#include <QPoint>
#include <QQueue>
#include <cmath>

const QString SGFShadedMaterialEffect::kLightAngleKey = QStringLiteral("shadedLightAngle");
const QString SGFShadedMaterialEffect::kLightElevationKey = QStringLiteral("shadedLightElevation");
const QString SGFShadedMaterialEffect::kBumpScaleKey = QStringLiteral("shadedBumpScale");
const QString SGFShadedMaterialEffect::kAmbientKey = QStringLiteral("shadedAmbient");
const QString SGFShadedMaterialEffect::kSmoothRadiusKey = QStringLiteral("shadedSmoothRadius");
const QString SGFShadedMaterialEffect::kLightColorKey = QStringLiteral("shadedLightColor");
const QString SGFShadedMaterialEffect::kFlatBaseColorKey = QStringLiteral("shadedFlatBaseColor");
const QString SGFShadedMaterialEffect::kForceFlatBaseKey = QStringLiteral("shadedForceFlatBase");
const QString SGFShadedMaterialEffect::kInteriorReliefKey = QStringLiteral("shadedInteriorRelief");
const QString SGFShadedMaterialEffect::kSpecularStrengthKey = QStringLiteral("shadedSpecularStrength");
const QString SGFShadedMaterialEffect::kSpecularPowerKey = QStringLiteral("shadedSpecularPower");
const QString SGFShadedMaterialEffect::kInflateAmountKey = QStringLiteral("shadedInflateAmount");
const QString SGFShadedMaterialEffect::kMatcapMixKey = QStringLiteral("shadedMatcapMix");
const QString SGFShadedMaterialEffect::kMatcapImagePathKey = QStringLiteral("shadedMatcapImagePath");

QImage SGFShadedMaterialEffect::defaultMatcapImage()
{
    const int dim = 256;
    QImage img(dim, dim, QImage::Format_ARGB32);
    img.fill(0);
    const float cx = (dim - 1) * 0.5f;
    const float cy = (dim - 1) * 0.5f;
    const float R = (dim - 1) * 0.48f;
    for ( int y = 0; y < dim; ++y ) {
        QRgb* line = reinterpret_cast<QRgb*>(img.scanLine(y));
        for ( int x = 0; x < dim; ++x ) {
            const float dx = (static_cast<float>(x) - cx) / R;
            const float dy = (static_cast<float>(y) - cy) / R;
            const float r2 = dx * dx + dy * dy;
            if ( r2 > 1.f ) {
                line[x] = qRgba(0, 0, 0, 0);
                continue;
            }
            const float z = std::sqrt(qMax(0.f, 1.f - r2));
            const float lx = 0.55f;
            const float ly = -0.35f;
            const float lz = 0.76f;
            float nd = dx * lx + dy * ly + z * lz;
            nd = qBound(0.f, nd, 1.f);
            const float spec = std::pow(nd, 48.f) * 0.85f + std::pow(nd, 6.f) * 0.35f;
            const float amb = 0.12f;
            const float diff = 0.88f * nd;
            float r = amb + diff * 0.72f + spec * 1.1f;
            float g = amb + diff * 0.74f + spec * 1.08f;
            float b = amb + diff * 0.82f + spec * 1.15f;
            r = qBound(0.f, r, 1.f);
            g = qBound(0.f, g, 1.f);
            b = qBound(0.f, b, 1.f);
            line[x] = qRgba(
                static_cast<int>(r * 255.f),
                static_cast<int>(g * 255.f),
                static_cast<int>(b * 255.f),
                255);
        }
    }
    return img;
}

QImage SGFShadedMaterialEffect::loadMatcapImage(const QString& path)
{
    if ( path.trimmed().isEmpty() ) {
        return defaultMatcapImage();
    }
    if ( !QFileInfo::exists(path) ) {
        return defaultMatcapImage();
    }
    QImage loaded(path);
    if ( loaded.isNull() ) {
        return defaultMatcapImage();
    }
    return loaded.convertToFormat(QImage::Format_ARGB32);
}

QRgb SGFShadedMaterialEffect::sampleMatcapBilinear(const QImage& matcap, float u, float v)
{
    const int w = matcap.width();
    const int h = matcap.height();
    if ( w < 1 || h < 1 ) {
        return qRgb(255, 255, 255);
    }
    u = qBound(0.f, u, 1.f);
    v = qBound(0.f, v, 1.f);
    const float fx = u * static_cast<float>(w - 1);
    const float fy = v * static_cast<float>(h - 1);
    const int x0 = static_cast<int>(fx);
    const int y0 = static_cast<int>(fy);
    const int x1 = qMin(x0 + 1, w - 1);
    const int y1 = qMin(y0 + 1, h - 1);
    const float tx = fx - static_cast<float>(x0);
    const float ty = fy - static_cast<float>(y0);

    const QRgb c00 = matcap.pixel(x0, y0);
    const QRgb c10 = matcap.pixel(x1, y0);
    const QRgb c01 = matcap.pixel(x0, y1);
    const QRgb c11 = matcap.pixel(x1, y1);

    auto lerpCh = [&]( float a00, float a10, float a01, float a11 ) {
        const float i0 = a00 * (1.f - tx) + a10 * tx;
        const float i1 = a01 * (1.f - tx) + a11 * tx;
        return i0 * (1.f - ty) + i1 * ty;
    };
    const int rr = static_cast<int>(lerpCh(qRed(c00), qRed(c10), qRed(c01), qRed(c11)));
    const int gg = static_cast<int>(lerpCh(qGreen(c00), qGreen(c10), qGreen(c01), qGreen(c11)));
    const int bb = static_cast<int>(lerpCh(qBlue(c00), qBlue(c10), qBlue(c01), qBlue(c11)));
    const int aa = static_cast<int>(lerpCh(qAlpha(c00), qAlpha(c10), qAlpha(c01), qAlpha(c11)));
    return qRgba(qBound(0, rr, 255), qBound(0, gg, 255), qBound(0, bb, 255), qBound(0, aa, 255));
}

SGFShadedMaterialEffect::SGFShadedMaterialEffect()
{
    mEffectType = SGFEffectType::Effect_ShadedMaterial;
}

void SGFShadedMaterialEffect::willApplyEffectToGlyphs(const QVector<SGFGlyph>& /*glyphs*/)
{
    const SGFShadedMaterialEffectSettings settings = getSettings();
    const float mcapMix = qBound(0.f, settings.matcapMix, 1.f);
    if ( mcapMix > 0.001f ) {
        mMatcapForPass = loadMatcapImage(settings.matcapImagePath);
    } else {
        mMatcapForPass = QImage();
    }
}

void SGFShadedMaterialEffect::scaleEffect(float factor)
{
    SGFShadedMaterialEffectSettings s = getSettings();
    s.bumpScale *= factor;
    const int r = static_cast<int>(std::lround(static_cast<float>(s.smoothRadius) * factor));
    s.smoothRadius = qBound(0, r, 16);
    setSettings(s);
}

float SGFShadedMaterialEffect::sampleH(const QVector<float>& hmap, int w, int h, int x, int y)
{
    x = qBound(0, x, w - 1);
    y = qBound(0, y, h - 1);
    return hmap[y * w + x];
}

void SGFShadedMaterialEffect::applyInteriorDomeHeight(QVector<float>& hmap, int w, int h, const QImage& alphaRef, float strength)
{
    strength = qBound(0.f, strength, 1.f);
    if ( strength <= 0.001f || w < 1 || h < 1 ) {
        return;
    }

    int minX = w;
    int minY = h;
    int maxX = -1;
    int maxY = -1;

    for ( int y = 0; y < h; ++y ) {
        const QRgb* line = reinterpret_cast<const QRgb*>(alphaRef.constScanLine(y));
        for ( int x = 0; x < w; ++x ) {
            if ( qAlpha(line[x]) > 8 ) {
                minX = qMin(minX, x);
                minY = qMin(minY, y);
                maxX = qMax(maxX, x);
                maxY = qMax(maxY, y);
            }
        }
    }

    if ( maxX < minX || maxY < minY ) {
        return;
    }

    const float cx = (minX + maxX) * 0.5f;
    const float cy = (minY + maxY) * 0.5f;
    float maxR = 1.f;
    const float c[4][2] = {
        { static_cast<float>(minX), static_cast<float>(minY) },
        { static_cast<float>(maxX), static_cast<float>(minY) },
        { static_cast<float>(minX), static_cast<float>(maxY) },
        { static_cast<float>(maxX), static_cast<float>(maxY) },
    };
    for ( int i = 0; i < 4; ++i ) {
        const float dx = c[i][0] - cx;
        const float dy = c[i][1] - cy;
        maxR = qMax(maxR, std::sqrt(dx * dx + dy * dy));
    }

    for ( int y = 0; y < h; ++y ) {
        const QRgb* aLine = reinterpret_cast<const QRgb*>(alphaRef.constScanLine(y));
        for ( int x = 0; x < w; ++x ) {
            if ( qAlpha(aLine[x]) < 2 ) {
                continue;
            }
            const int idx = y * w + x;
            const float baseH = hmap[idx];
            const float dx = static_cast<float>(x) - cx;
            const float dy = static_cast<float>(y) - cy;
            const float d = qMin(1.f, std::sqrt(dx * dx + dy * dy) / maxR);
            const float dome = 1.f - d * d;
            const float shaped = baseH * (0.1f + 0.9f * dome);
            hmap[idx] = baseH * (1.f - strength) + shaped * strength;
        }
    }
}

void SGFShadedMaterialEffect::applyInflateHeight(QVector<float>& hmap, int w, int h, const QImage& alphaRef, float strength)
{
    strength = qBound(0.f, strength, 1.f);
    if ( strength <= 0.001f || w < 1 || h < 1 ) {
        return;
    }

    const int wh = w * h;
    // Match soft glyph edges (same as dome / shading); 40 was too strict — thin strokes had maxDist 0.
    const int alphaThr = 8;

    QVector<char> isFg(wh, 0);
    for ( int y = 0; y < h; ++y ) {
        const QRgb* line = reinterpret_cast<const QRgb*>(alphaRef.constScanLine(y));
        for ( int x = 0; x < w; ++x ) {
            if ( qAlpha(line[x]) > alphaThr ) {
                isFg[y * w + x] = 1;
            }
        }
    }

    const int INF = w + h + 64;
    QVector<int> dist(wh, INF);
    QQueue<QPoint> queue;

    auto isBoundary = [&]( int x, int y ) -> bool {
        const int idx = y * w + x;
        if ( !isFg[idx] ) {
            return false;
        }
        if ( x <= 0 || x >= w - 1 || y <= 0 || y >= h - 1 ) {
            return true;
        }
        return !isFg[idx - 1] || !isFg[idx + 1] || !isFg[idx - w] || !isFg[idx + w]
            || !isFg[idx - w - 1] || !isFg[idx - w + 1] || !isFg[idx + w - 1] || !isFg[idx + w + 1];
    };

    for ( int y = 0; y < h; ++y ) {
        for ( int x = 0; x < w; ++x ) {
            const int idx = y * w + x;
            if ( !isFg[idx] ) {
                continue;
            }
            if ( isBoundary(x, y) ) {
                dist[idx] = 0;
                queue.enqueue(QPoint(x, y));
            }
        }
    }

    // 8-neighbour (uniform step-1): thicker “core” and higher maxDist on diagonal strokes.
    const int dx[8] = { -1, 1, 0, 0, -1, -1, 1, 1 };
    const int dy[8] = { 0, 0, -1, 1, -1, 1, -1, 1 };

    while ( !queue.isEmpty() ) {
        const QPoint p = queue.dequeue();
        const int idx = p.y() * w + p.x();
        const int d = dist[idx];
        for ( int k = 0; k < 8; ++k ) {
            const int nx = p.x() + dx[k];
            const int ny = p.y() + dy[k];
            if ( nx < 0 || nx >= w || ny < 0 || ny >= h ) {
                continue;
            }
            const int nidx = ny * w + nx;
            if ( !isFg[nidx] ) {
                continue;
            }
            if ( dist[nidx] > d + 1 ) {
                dist[nidx] = d + 1;
                queue.enqueue(QPoint(nx, ny));
            }
        }
    }

    int maxDist = 0;
    for ( int i = 0; i < wh; ++i ) {
        if ( isFg[i] && dist[i] < INF ) {
            maxDist = qMax(maxDist, dist[i]);
        }
    }

    for ( int y = 0; y < h; ++y ) {
        const QRgb* aLine = reinterpret_cast<const QRgb*>(alphaRef.constScanLine(y));
        for ( int x = 0; x < w; ++x ) {
            const int idx = y * w + x;
            if ( qAlpha(aLine[x]) < 2 ) {
                continue;
            }
            float m = 1.f;
            if ( maxDist > 0 && isFg[idx] && dist[idx] < INF ) {
                const float f = qBound(0.f, static_cast<float>(dist[idx]) / static_cast<float>(maxDist), 1.f);
                // Quarter sine: smooth dome from edge (0) toward interior — less “needle” at skeleton junctions than steep pow().
                const float dome01 = std::sin(f * 1.57079633f); // sin(f * π/2), 0→0, 1→1
                const float balloon = 0.05f + 0.95f * dome01;
                m = (1.f - strength) + balloon * strength;
            }
            hmap[idx] *= m;
        }
    }
}

void SGFShadedMaterialEffect::blurHeightMap(QVector<float>& hmap, int w, int h, int radius)
{
    if ( radius <= 0 || w < 1 || h < 1 ) {
        return;
    }

    QVector<float> tmp(w * h);
    const int dia = 2 * radius + 1;

    for ( int y = 0; y < h; ++y ) {
        for ( int x = 0; x < w; ++x ) {
            float sum = 0.f;
            for ( int dx = -radius; dx <= radius; ++dx ) {
                sum += sampleH(hmap, w, h, x + dx, y);
            }
            tmp[y * w + x] = sum / static_cast<float>(dia);
        }
    }

    for ( int y = 0; y < h; ++y ) {
        for ( int x = 0; x < w; ++x ) {
            float sum = 0.f;
            for ( int dy = -radius; dy <= radius; ++dy ) {
                sum += sampleH(tmp, w, h, x, y + dy);
            }
            hmap[y * w + x] = sum / static_cast<float>(dia);
        }
    }
}

void SGFShadedMaterialEffect::applyToGlyph(SGFGlyph& glyph)
{
    QImage& img = glyph.image;
    if ( img.isNull() || img.width() < 1 || img.height() < 1 ) {
        return;
    }

    const SGFShadedMaterialEffectSettings settings = getSettings();

    QImage heightSource;
    if ( !glyph.maskImage.isNull() && glyph.maskImage.size() == img.size() ) {
        heightSource = glyph.maskImage;
    } else {
        heightSource = img;
    }

    if ( heightSource.format() != QImage::Format_ARGB32 ) {
        heightSource = heightSource.convertToFormat(QImage::Format_ARGB32);
    }

    const int w = img.width();
    const int h = img.height();
    QVector<float> hmap(w * h);

    for ( int y = 0; y < h; ++y ) {
        const QRgb* line = reinterpret_cast<const QRgb*>(heightSource.constScanLine(y));
        for ( int x = 0; x < w; ++x ) {
            hmap[y * w + x] = qAlpha(line[x]) * (1.f / 255.f);
        }
    }

    blurHeightMap(hmap, w, h, settings.smoothRadius);
    // Inflate before bbox dome so medial-axis ridge survives global dome shaping.
    applyInflateHeight(hmap, w, h, heightSource, settings.inflateAmount);
    if ( settings.inflateAmount > 0.02f ) {
        // Soften DT ridge spikes (stroke intersections) before interior dome / normals.
        blurHeightMap(hmap, w, h, 1);
    }
    applyInteriorDomeHeight(hmap, w, h, heightSource, settings.interiorRelief);

    const float radA = qDegreesToRadians(settings.lightAngle);
    const float radE = qDegreesToRadians(qBound(1.f, settings.lightElevation, 89.f));
    float Lx = std::cos(radA) * std::cos(radE);
    float Ly = std::sin(radA) * std::cos(radE);
    float Lz = std::sin(radE);
    const float invLen = 1.f / std::sqrt(Lx * Lx + Ly * Ly + Lz * Lz);
    Lx *= invLen;
    Ly *= invLen;
    Lz *= invLen;

    const float Vx = 0.f;
    const float Vy = 0.f;
    const float Vz = 1.f;
    float Hx = Lx + Vx;
    float Hy = Ly + Vy;
    float Hz = Lz + Vz;
    const float invHlen = 1.f / std::sqrt(Hx * Hx + Hy * Hy + Hz * Hz);
    Hx *= invHlen;
    Hy *= invHlen;
    Hz *= invHlen;

    const float bump = qMax(0.05f, settings.bumpScale);
    const float specStr = qBound(0.f, settings.specularStrength, 1.f);
    const float specPow = qBound(1.f, settings.specularPower, 256.f);
    const float amb = qBound(0.f, settings.ambient, 1.f);
    const float lr = settings.lightColor.redF();
    const float lg = settings.lightColor.greenF();
    const float lb = settings.lightColor.blueF();
    const float fbr = settings.flatBaseColor.redF();
    const float fbg = settings.flatBaseColor.greenF();
    const float fbb = settings.flatBaseColor.blueF();

    const float mcapMix = qBound(0.f, settings.matcapMix, 1.f);
    QImage matcapLocal;
    const QImage* matcapPtr = nullptr;
    if ( mcapMix > 0.001f ) {
        if ( !mMatcapForPass.isNull() ) {
            matcapPtr = &mMatcapForPass;
        } else {
            matcapLocal = loadMatcapImage(settings.matcapImagePath);
            matcapPtr = &matcapLocal;
        }
    }

    QImage base = img.convertToFormat(QImage::Format_ARGB32);
    QImage work(w, h, QImage::Format_ARGB32);
    work.fill(0);

    for ( int y = 0; y < h; ++y ) {
        QRgb* dst = reinterpret_cast<QRgb*>(work.scanLine(y));
        const QRgb* src = reinterpret_cast<const QRgb*>(base.constScanLine(y));
        for ( int x = 0; x < w; ++x ) {
            const int a = qAlpha(src[x]);
            if ( a < 2 ) {
                dst[x] = 0;
                continue;
            }

            const float hx = (sampleH(hmap, w, h, x - 1, y) - sampleH(hmap, w, h, x + 1, y)) * 0.5f;
            const float hy = (sampleH(hmap, w, h, x, y - 1) - sampleH(hmap, w, h, x, y + 1)) * 0.5f;
            float nx = hx * bump;
            float ny = hy * bump;
            float nz = 1.0f;
            const float invN = 1.f / std::sqrt(nx * nx + ny * ny + nz * nz);
            nx *= invN;
            ny *= invN;
            nz *= invN;

            const float diff = std::max(0.f, nx * Lx + ny * Ly + nz * Lz);
            const float shade = amb + (1.f - amb) * diff;

            float nh = nx * Hx + ny * Hy + nz * Hz;
            nh = std::max(0.f, nh);
            const float specExp = qBound(1.f, specPow / 10.f, 96.f);
            const float specAtt = 1.f - mcapMix * 0.92f;
            const float spec = ( specStr > 0.001f ) ? ( std::pow(nh, specExp) * specStr * specAtt ) : 0.f;

            float ar;
            float ag;
            float ab;
            if ( settings.forceFlatBase ) {
                ar = fbr;
                ag = fbg;
                ab = fbb;
            } else {
                ar = qRed(src[x]) * (1.f / 255.f);
                ag = qGreen(src[x]) * (1.f / 255.f);
                ab = qBlue(src[x]) * (1.f / 255.f);
                if ( ar + ag + ab < 0.02f ) {
                    ar = fbr;
                    ag = fbg;
                    ab = fbb;
                }
            }

            float br = ar * shade * lr + spec * lr;
            float bg = ag * shade * lg + spec * lg;
            float bb = ab * shade * lb + spec * lb;

            if ( mcapMix > 0.001f && matcapPtr != nullptr && !matcapPtr->isNull() ) {
                const float denom = 2.f * std::sqrt(nx * nx + ny * ny + (nz + 1.f) * (nz + 1.f));
                if ( denom > 1e-5f ) {
                    const float invd = 1.f / denom;
                    const float mu = nx * invd + 0.5f;
                    const float mv = 0.5f - ny * invd;
                    const QRgb mc = sampleMatcapBilinear(*matcapPtr, mu, mv);
                    const float mr = qRed(mc) * (1.f / 255.f);
                    const float mg = qGreen(mc) * (1.f / 255.f);
                    const float mb = qBlue(mc) * (1.f / 255.f);
                    const float mcr = ar * mr * lr;
                    const float mcg = ag * mg * lg;
                    const float mcb = ab * mb * lb;
                    const float om = 1.f - mcapMix;
                    br = br * om + mcr * mcapMix;
                    bg = bg * om + mcg * mcapMix;
                    bb = bb * om + mcb * mcapMix;
                }
            }
            const int lrOut = qBound(0, static_cast<int>(std::lround(255.f * br)), 255);
            const int lgOut = qBound(0, static_cast<int>(std::lround(255.f * bg)), 255);
            const int lbOut = qBound(0, static_cast<int>(std::lround(255.f * bb)), 255);

            dst[x] = qRgba(lrOut, lgOut, lbOut, a);
        }
    }

    QPainter painter(&img);
    painter.setCompositionMode(QPainter::CompositionMode_Source);
    painter.drawImage(0, 0, base);
    painter.setCompositionMode(compositionForBlendMode(mBlendMode));
    painter.setOpacity(qBound(0.0, static_cast<double>(mOpacity), 1.0));
    painter.drawImage(0, 0, work);
    painter.end();
}

void SGFShadedMaterialEffect::setDefaultParameters()
{
    setSettings(SGFShadedMaterialEffectSettings());
}

bool SGFShadedMaterialEffect::writeSubclassToXmlStream(QXmlStreamWriter& writer)
{
    const SGFShadedMaterialEffectSettings s = getSettings();
    writer.writeAttribute(kLightAngleKey, QString::number(s.lightAngle));
    writer.writeAttribute(kLightElevationKey, QString::number(s.lightElevation));
    writer.writeAttribute(kBumpScaleKey, QString::number(s.bumpScale));
    writer.writeAttribute(kAmbientKey, QString::number(s.ambient));
    writer.writeAttribute(kSmoothRadiusKey, QString::number(s.smoothRadius));
    writer.writeAttribute(kLightColorKey, s.lightColor.name(QColor::HexArgb));
    writer.writeAttribute(kFlatBaseColorKey, s.flatBaseColor.name(QColor::HexArgb));
    writer.writeAttribute(kForceFlatBaseKey, QString::number(s.forceFlatBase ? 1 : 0));
    writer.writeAttribute(kInteriorReliefKey, QString::number(s.interiorRelief));
    writer.writeAttribute(kSpecularStrengthKey, QString::number(s.specularStrength));
    writer.writeAttribute(kSpecularPowerKey, QString::number(s.specularPower));
    writer.writeAttribute(kInflateAmountKey, QString::number(s.inflateAmount));
    writer.writeAttribute(kMatcapMixKey, QString::number(s.matcapMix));
    writer.writeAttribute(kMatcapImagePathKey, s.matcapImagePath);
    return true;
}

void SGFShadedMaterialEffect::readSubclassFromXmlNode(const QDomElement& element)
{
    SGFShadedMaterialEffectSettings s = getSettings();

    if ( element.hasAttribute(kLightAngleKey) ) {
        s.lightAngle = element.attribute(kLightAngleKey).toFloat();
    }
    if ( element.hasAttribute(kLightElevationKey) ) {
        s.lightElevation = element.attribute(kLightElevationKey).toFloat();
    }
    if ( element.hasAttribute(kBumpScaleKey) ) {
        s.bumpScale = element.attribute(kBumpScaleKey).toFloat();
    }
    if ( element.hasAttribute(kAmbientKey) ) {
        s.ambient = element.attribute(kAmbientKey).toFloat();
    }
    if ( element.hasAttribute(kSmoothRadiusKey) ) {
        s.smoothRadius = element.attribute(kSmoothRadiusKey).toInt();
    }
    if ( element.hasAttribute(kLightColorKey) ) {
        s.lightColor = QColor(element.attribute(kLightColorKey));
    }
    if ( element.hasAttribute(kFlatBaseColorKey) ) {
        s.flatBaseColor = QColor(element.attribute(kFlatBaseColorKey));
    }
    if ( element.hasAttribute(kForceFlatBaseKey) ) {
        s.forceFlatBase = element.attribute(kForceFlatBaseKey).toInt() != 0;
    }
    if ( element.hasAttribute(kInteriorReliefKey) ) {
        s.interiorRelief = element.attribute(kInteriorReliefKey).toFloat();
    }
    if ( element.hasAttribute(kSpecularStrengthKey) ) {
        s.specularStrength = element.attribute(kSpecularStrengthKey).toFloat();
    }
    if ( element.hasAttribute(kSpecularPowerKey) ) {
        s.specularPower = element.attribute(kSpecularPowerKey).toFloat();
    }
    if ( element.hasAttribute(kInflateAmountKey) ) {
        s.inflateAmount = element.attribute(kInflateAmountKey).toFloat();
    }
    if ( element.hasAttribute(kMatcapMixKey) ) {
        s.matcapMix = element.attribute(kMatcapMixKey).toFloat();
    }
    if ( element.hasAttribute(kMatcapImagePathKey) ) {
        s.matcapImagePath = element.attribute(kMatcapImagePathKey);
    }

    setSettings(s);
}

SGFShadedMaterialEffectSettings SGFShadedMaterialEffect::getSettings()
{
    SGFShadedMaterialEffectSettings s;
    s.enabled = mEnabled;
    s.blendMode = mBlendMode;
    s.opacity = mOpacity;
    s.lightAngle = mParameters.value(kLightAngleKey, 135.f).toFloat();
    s.lightElevation = mParameters.value(kLightElevationKey, 45.f).toFloat();
    s.bumpScale = mParameters.value(kBumpScaleKey, 3.5f).toFloat();
    s.ambient = mParameters.value(kAmbientKey, 0.35f).toFloat();
    s.smoothRadius = mParameters.value(kSmoothRadiusKey, 2).toInt();
    s.lightColor = mParameters.value(kLightColorKey, QColor(255, 255, 255)).value<QColor>();
    s.flatBaseColor = mParameters.value(kFlatBaseColorKey, QColor(200, 200, 210)).value<QColor>();
    s.forceFlatBase = mParameters.value(kForceFlatBaseKey, false).toBool();
    s.interiorRelief = mParameters.value(kInteriorReliefKey, 0.88f).toFloat();
    s.specularStrength = mParameters.value(kSpecularStrengthKey, 0.45f).toFloat();
    s.specularPower = mParameters.value(kSpecularPowerKey, 56.f).toFloat();
    s.inflateAmount = mParameters.value(kInflateAmountKey, 0.55f).toFloat();
    s.matcapMix = mParameters.value(kMatcapMixKey, 0.f).toFloat();
    s.matcapImagePath = mParameters.value(kMatcapImagePathKey, QString()).toString();
    return s;
}

void SGFShadedMaterialEffect::setSettings(SGFShadedMaterialEffectSettings s)
{
    mEnabled = s.enabled;
    mBlendMode = s.blendMode;
    mOpacity = s.opacity;
    mParameters[kLightAngleKey] = s.lightAngle;
    mParameters[kLightElevationKey] = s.lightElevation;
    mParameters[kBumpScaleKey] = s.bumpScale;
    mParameters[kAmbientKey] = s.ambient;
    mParameters[kSmoothRadiusKey] = s.smoothRadius;
    mParameters[kLightColorKey] = QVariant::fromValue(s.lightColor);
    mParameters[kFlatBaseColorKey] = QVariant::fromValue(s.flatBaseColor);
    mParameters[kForceFlatBaseKey] = s.forceFlatBase;
    mParameters[kInteriorReliefKey] = s.interiorRelief;
    mParameters[kSpecularStrengthKey] = s.specularStrength;
    mParameters[kSpecularPowerKey] = s.specularPower;
    mParameters[kInflateAmountKey] = s.inflateAmount;
    mParameters[kMatcapMixKey] = s.matcapMix;
    mParameters[kMatcapImagePathKey] = s.matcapImagePath;
    mHasUnsavedData = true;
}

QPixmap SGFShadedMaterialEffect::getPreview(int width, int height, int pointSize)
{
    SGFGlyph glyph(QFont(QStringLiteral("Arial"), pointSize), QLatin1Char('A'));
    glyph.expandSizeForEffect(this);
    glyph.prepareImages();

    QPainter seed(&glyph.image);
    seed.setRenderHint(QPainter::Antialiasing);
    seed.fillPath(glyph.path, QColor(210, 215, 225));
    seed.end();

    applyToGlyph(glyph);
    glyph.composeFinalImage();

    QPixmap icon(width, height);
    icon.fill(QColor(0, 0, 0, 0));

    const int x = static_cast<int>((width * 0.5f) - (glyph.image.width() * 0.5f));
    const int y = static_cast<int>((height * 0.5f) - (glyph.image.height() * 0.5f));

    QPainter painter(&icon);
    painter.drawImage(x, y, glyph.image);
    painter.end();

    return icon;
}
