#include "SGFInnerBevel.h"

#include <QPainter>
#include <QtMath>
#include <cmath>

namespace {

// Same separable blur as SGFOuterGlowEffect::blurImage (alpha channel only).
QImage blurImageAlpha(const QImage &image, int radius)
{
    int tab[] = { 14, 10, 8, 6, 5, 5, 4, 3, 3, 3, 3, 2, 2, 2, 2, 2, 2 };
    const int alpha = (radius < 1) ? 16 : (radius > 17) ? 1 : tab[radius - 1];

    QImage result = image.convertToFormat(QImage::Format_ARGB32_Premultiplied);
    const int r1 = image.rect().top();
    const int r2 = image.rect().bottom();
    const int c1 = image.rect().left();
    const int c2 = image.rect().right();

    const int bpl = result.bytesPerLine();
    int rgba[4];
    unsigned char *p;

    int i1;
    int i2;
    i1 = i2 = (QSysInfo::ByteOrder == QSysInfo::BigEndian ? 0 : 3);

    for ( int col = c1; col <= c2; col++ ) {
        p = result.scanLine(r1) + col * 4;
        for ( int i = i1; i <= i2; i++ ) {
            rgba[i] = p[i] << 4;
        }

        p += bpl;
        for ( int j = r1; j < r2; j++, p += bpl ) {
            for ( int i = i1; i <= i2; i++ ) {
                p[i] = (rgba[i] += ((p[i] << 4) - rgba[i]) * alpha / 16) >> 4;
            }
        }
    }

    for ( int row = r1; row <= r2; row++ ) {
        p = result.scanLine(row) + c1 * 4;
        for ( int i = i1; i <= i2; i++ ) {
            rgba[i] = p[i] << 4;
        }

        p += 4;
        for ( int j = c1; j < c2; j++, p += 4 ) {
            for ( int i = i1; i <= i2; i++ ) {
                p[i] = (rgba[i] += ((p[i] << 4) - rgba[i]) * alpha / 16) >> 4;
            }
        }
    }

    for ( int col = c1; col <= c2; col++ ) {
        p = result.scanLine(r2) + col * 4;
        for ( int i = i1; i <= i2; i++ ) {
            rgba[i] = p[i] << 4;
        }

        p -= bpl;
        for ( int j = r1; j < r2; j++, p -= bpl ) {
            for ( int i = i1; i <= i2; i++ ) {
                p[i] = (rgba[i] += ((p[i] << 4) - rgba[i]) * alpha / 16) >> 4;
            }
        }
    }

    for ( int row = r1; row <= r2; row++ ) {
        p = result.scanLine(row) + c2 * 4;
        for ( int i = i1; i <= i2; i++ ) {
            rgba[i] = p[i] << 4;
        }

        p -= 4;
        for ( int j = c1; j < c2; j++, p -= 4 ) {
            for ( int i = i1; i <= i2; i++ ) {
                p[i] = (rgba[i] += ((p[i] << 4) - rgba[i]) * alpha / 16) >> 4;
            }
        }
    }

    return result.convertToFormat(QImage::Format_ARGB32);
}

float sampleAlphaBilinear(const QImage &img, float x, float y)
{
    const int w = img.width();
    const int h = img.height();
    if ( w < 1 || h < 1 ) {
        return 0.f;
    }
    x = qBound(0.f, x, static_cast<float>(w - 1) - 1e-4f);
    y = qBound(0.f, y, static_cast<float>(h - 1) - 1e-4f);
    const int x0 = static_cast<int>(std::floor(x));
    const int y0 = static_cast<int>(std::floor(y));
    const int x1 = qMin(x0 + 1, w - 1);
    const int y1 = qMin(y0 + 1, h - 1);
    const float tx = x - static_cast<float>(x0);
    const float ty = y - static_cast<float>(y0);
    const float a00 = static_cast<float>(qAlpha(img.pixel(x0, y0)));
    const float a10 = static_cast<float>(qAlpha(img.pixel(x1, y0)));
    const float a01 = static_cast<float>(qAlpha(img.pixel(x0, y1)));
    const float a11 = static_cast<float>(qAlpha(img.pixel(x1, y1)));
    const float i0 = a00 * (1.f - tx) + a10 * tx;
    const float i1 = a01 * (1.f - tx) + a11 * tx;
    return i0 * (1.f - ty) + i1 * ty;
}

} // namespace

void SGFInnerBevel::applyToImage(QImage &image, const QPainterPath &path, float amount, float angleDeg, float blurPx, float opacity01, float intensity)
{
    if ( amount <= 0.001f || image.isNull() ) {
        return;
    }

    const int w = image.width();
    const int h = image.height();
    if ( w < 1 || h < 1 ) {
        return;
    }

    if ( image.format() != QImage::Format_ARGB32 ) {
        image = image.convertToFormat(QImage::Format_ARGB32);
    }

    QImage mask(w, h, QImage::Format_ARGB32);
    mask.fill(0);
    {
        QPainter p(&mask);
        p.setRenderHint(QPainter::Antialiasing);
        p.fillPath(path, QColor(255, 255, 255, 255));
    }

    QImage blurred = mask;
    const int br = static_cast<int>(std::round(std::max(0.f, blurPx)));
    if ( br > 0 ) {
        blurred = blurImageAlpha(mask, br);
    }

    const float rad = qDegreesToRadians(angleDeg);
    const float lx = std::cos(rad);
    const float ly = std::sin(rad);
    const float step = std::max(1.f, 1.f + blurPx * 0.4f);
    const float intMul = qBound(0.1f, intensity / 100.f, 8.f);
    const float gain = (amount / 100.f) * qBound(0.f, opacity01, 1.f) * 110.f * intMul;

    for ( int y = 0; y < h; ++y ) {
        QRgb *dst = reinterpret_cast<QRgb *>(image.scanLine(y));
        for ( int x = 0; x < w; ++x ) {
            if ( qAlpha(mask.pixel(x, y)) == 0 ) {
                continue;
            }

            const float x1 = static_cast<float>(x) + lx * step;
            const float y1 = static_cast<float>(y) + ly * step;
            const float x2 = static_cast<float>(x) - lx * step;
            const float y2 = static_cast<float>(y) - ly * step;
            const float b1 = sampleAlphaBilinear(blurred, x1, y1);
            const float b2 = sampleAlphaBilinear(blurred, x2, y2);
            const float diff = (b1 - b2) / 255.f;
            const float delta = diff * gain;
            if ( std::abs(delta) < 0.25f ) {
                continue;
            }

            const QRgb pix = dst[x];
            int r = qRed(pix);
            int g = qGreen(pix);
            int b = qBlue(pix);
            const int a = qAlpha(pix);
            if ( delta > 0.f ) {
                r = qBound(0, static_cast<int>(static_cast<float>(r) + delta), 255);
                g = qBound(0, static_cast<int>(static_cast<float>(g) + delta), 255);
                b = qBound(0, static_cast<int>(static_cast<float>(b) + delta), 255);
            } else {
                const float d = -delta;
                r = qBound(0, static_cast<int>(static_cast<float>(r) - d), 255);
                g = qBound(0, static_cast<int>(static_cast<float>(g) - d), 255);
                b = qBound(0, static_cast<int>(static_cast<float>(b) - d), 255);
            }
            dst[x] = qRgba(r, g, b, a);
        }
    }
}
