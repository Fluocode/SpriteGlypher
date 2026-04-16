#include "SGFFontBMFontExportData.h"

#include "../SGFDocument.h"
#include "../SGFInputSettings.h"
#include "../SGFGenerationSettings.h"
#include "../SGFSpriteFont.h"
#include "../SGFGlyph.h"

#include <QFontMetrics>
#include <QHash>
#include <algorithm>
#include <cmath>
#include <limits>

bool SGFFontBMFontExport::buildExportData(SGFDocument *doc, SGFBMFontExportData &data)
{
    if ( doc == nullptr ) {
        return false;
    }

    // Regenerate atlas + glyph rects so xadvance uses current effects and non-zero atlasRect/image widths.
    // Otherwise export can write stale TTF-only advances that match old BMFont output.
    doc->generateSpriteFont();

    if ( doc->getSpriteFont().isNull || doc->getSpriteFont().textureAtlas.isNull() ) {
        return false;
    }

    SGFInputSettings inSettings = doc->getInputSettings();
    SGFGenerationSettings genSettings = doc->getGenerationSettings();
    SGFSpriteFont spriteFont = doc->getSpriteFont();

    const bool fromPng = (inSettings.inputSource == SGFInputSource::PngSprites);

    // Match generateGlyphs(): same QFont as rasterized glyphs (family + style + point size), not only family + rounded size.
    QFont metricsFont(inSettings.fontFamily, qRound(inSettings.fontSize));
    if ( !fromPng && !spriteFont.glyphs.isEmpty() ) {
        const QFont &gf = spriteFont.glyphs.first().font;
        if ( !gf.family().isEmpty() ) {
            metricsFont = gf;
        }
    }
    QFontMetrics fontMetrics(metricsFont);

    data.bold = false;
    data.italic = false;
    data.charset = QString();
    data.unicode = true;
    data.stretchH = 100;
    data.smooth = 1;
    data.aa = 1;

    int pad = genSettings.padding;
    data.paddingUp = pad;
    data.paddingRight = pad;
    data.paddingDown = pad;
    data.paddingLeft = pad;
    data.spacingHoriz = genSettings.spacing;
    data.spacingVert = genSettings.spacing;
    data.outline = 0;

    if ( fromPng ) {
        data.face = inSettings.pngFontFaceName.isEmpty()
                        ? QStringLiteral("PNG Font") : inSettings.pngFontFaceName;
        int maxH = 1;
        for ( const SGFGlyph &g : spriteFont.glyphs ) {
            const int h = (g.atlasRect.height() > 0) ? g.atlasRect.height() : g.image.height();
            if ( h > maxH ) {
                maxH = h;
            }
        }
        data.fontSize = maxH;
        data.lineHeight = maxH;
        data.base = maxH;
    } else {
        data.face = inSettings.fontFamily;
        data.fontSize = qRound(inSettings.fontSize);
        data.lineHeight = fontMetrics.lineSpacing();
        data.base = fontMetrics.ascent();
    }

    data.scaleW = spriteFont.textureAtlas.width();
    data.scaleH = spriteFont.textureAtlas.height();
    data.pages = 1;
    data.packed = 0;
    data.alphaChnl = 0;
    data.redChnl = 4;
    data.greenChnl = 4;
    data.blueChnl = 4;

    data.page0FileName.clear();
    data.chars.clear();
    data.kernings.clear();

    // For vector fonts: pair kerning uses QFontMetrics string widths. After widening xadvance for bitmap
    // effects, those negative kerning values still pull the next glyph as if it were vector-only → overlap.
    // Skip exporting a kerning pair if either glyph needed a visual (bitmap) advance larger than TTF advance.
    QHash<QChar, int> hfAdvByChar;
    QHash<QChar, int> xadvByChar;

    bool hasSpace = false;

    for ( SGFGlyph &glyph : spriteFont.glyphs )
    {
        if ( glyph.character == QLatin1Char(' ') ) {
            hasSpace = true;
        }

        SGFBMFontCharEntry entry;
        entry.id = glyph.character.unicode();
        entry.x = glyph.atlasRect.x();
        entry.y = glyph.atlasRect.y();
        entry.width = glyph.atlasRect.width();
        entry.height = glyph.atlasRect.height();
        entry.page = 0;
        entry.chnl = 15;

        if ( fromPng ) {
            entry.xoffset = 0;
            entry.yoffset = 0;
            const int w = (glyph.atlasRect.width() > 0) ? glyph.atlasRect.width() : glyph.image.width();
            entry.xadvance = (w > 1) ? w : 1;
        } else {
            // tightBoundingRect follows the painted outline more closely than boundingRect(); the latter
            // often adds empty space on the left (looks like leading indent when Starling centers by xadvance).
            const QString chStr = QString(glyph.character);
            QRect ink = fontMetrics.tightBoundingRect(chStr);
            if ( ink.width() <= 0 || ink.height() <= 0 ) {
                ink = fontMetrics.boundingRect(glyph.character);
            }
            // Raster places the path at (effectPadding.left, effectPadding.top) inside the glyph image.
            // BMFont draws the subtexture at (cursor + xoffset); ink must line up with tightBoundingRect:
            //   cursor + xoffset + effectPadding.left ≈ cursor + ink.x()  →  xoffset ≈ ink.x() - effectPadding.left
            // (Glow/shadow pad the texture but must not shift the typographic origin — that was causing a
            // left “indent” when centering text in Starling / BMFont consumers.)
            entry.xoffset = static_cast<int>(std::lround(
                static_cast<double>(ink.x()) - static_cast<double>(glyph.effectPadding.left)));
            entry.yoffset = static_cast<int>(fontMetrics.height() + ink.y() - fontMetrics.descent()
                - glyph.effectPadding.bottom);
            const int hfAdv = static_cast<int>(fontMetrics.horizontalAdvance(glyph.character));
            // Advance should ensure effects don't overlap, but should NOT include left padding/bearing.
            // Compute the visual right edge from the font ink box + *right* effect padding:
            //   visualRight = (leftBearing + inkWidth) + rightPad
            // Then take max() with the font's horizontal advance.
            float rpad = glyph.advancePadding.right;
            if ( rpad < 0.0f ) {
                rpad = 0.0f;
            }
            const int extraRight = static_cast<int>(std::ceil(rpad));
            const int visualRight = ink.x() + ink.width() + extraRight;
            int xa = hfAdv;
            if ( visualRight > xa ) {
                xa = visualRight;
            }
            if ( xa < 1 ) {
                xa = 1;
            }
            entry.xadvance = xa;
            hfAdvByChar.insert(glyph.character, hfAdv);
            xadvByChar.insert(glyph.character, entry.xadvance);
        }

        data.chars.append(entry);
    }

    // Starling / BMFont: quads draw at (cursor + xoffset); centering uses sum(xadvance), so a large
    // xoffset looks like a left indent. Subtract a constant from every rasterized glyph's xoffset.
    // Use min(xoffset) over ASCII digits '0'..'9' when present — otherwise punctuation often has a
    // smaller bearing and would pull the global min down, leaving digits still indented.
    if ( !fromPng ) {
        const int id0 = static_cast<int>(QLatin1Char('0').unicode());
        const int id9 = static_cast<int>(QLatin1Char('9').unicode());

        int minXoDigits = std::numeric_limits<int>::max();
        bool anyDigit = false;
        for ( const SGFBMFontCharEntry &ch : data.chars ) {
            if ( ch.width > 0 && ch.id >= id0 && ch.id <= id9 ) {
                anyDigit = true;
                minXoDigits = std::min(minXoDigits, ch.xoffset);
            }
        }

        int minXo = std::numeric_limits<int>::max();
        if ( anyDigit && minXoDigits != std::numeric_limits<int>::max() ) {
            minXo = minXoDigits;
        } else {
            for ( const SGFBMFontCharEntry &ch : data.chars ) {
                if ( ch.width > 0 ) {
                    minXo = std::min(minXo, ch.xoffset);
                }
            }
        }

        if ( minXo != std::numeric_limits<int>::max() && minXo > 0 ) {
            for ( SGFBMFontCharEntry &ch : data.chars ) {
                if ( ch.width > 0 ) {
                    ch.xoffset -= minXo;
                }
            }
        }

        // Prevent overlap of SOLID content, but allow pure shadow pixels to overlap.
        // effectPadding includes shadows; advancePadding excludes them (Shadow::affectsAdvance() == false).
        for ( int i = 0; i < data.chars.size(); ++i ) {
            SGFBMFontCharEntry &ch = data.chars[i];
            if ( ch.width <= 0 ) {
                continue;
            }

            // Reconstruct per-side "shadow-only" expansion from stored padding on glyphs.
            // data.chars aligns 1:1 with spriteFont.glyphs order (space may be appended later).
            if ( i >= spriteFont.glyphs.size() ) {
                continue;
            }
            const SGFGlyph &g = spriteFont.glyphs[i];
            const int shadowRight = static_cast<int>(std::ceil(std::max(0.0f, g.effectPadding.right - g.advancePadding.right)));

            // Right edge of solid ink in screen space along X: bitmap starts at xoffset, texture width ch.width,
            // strip non-advancing glow on the right (shadowRight).
            const int solidRight = ch.xoffset + ch.width - shadowRight;
            if ( solidRight > ch.xadvance ) {
                ch.xadvance = solidRight;
            }
        }

        xadvByChar.clear();
        for ( const SGFBMFontCharEntry &ch : data.chars ) {
            if ( ch.width > 0 ) {
                xadvByChar.insert(QChar(static_cast<char16_t>(ch.id)), ch.xadvance);
            }
        }
    }

    if ( !hasSpace )
    {
        SGFBMFontCharEntry spaceEntry;
        spaceEntry.id = QChar::fromLatin1(' ').unicode();
        spaceEntry.x = 0;
        spaceEntry.y = 0;
        spaceEntry.width = 0;
        spaceEntry.height = 0;
        spaceEntry.xoffset = 0;
        spaceEntry.yoffset = 0;
        if ( fromPng ) {
            const int sp = data.lineHeight / 4;
            spaceEntry.xadvance = (sp > 1) ? sp : 1;
        } else {
            spaceEntry.xadvance = static_cast<int>(fontMetrics.horizontalAdvance(QLatin1Char(' ')));
        }
        spaceEntry.page = 0;
        spaceEntry.chnl = 15;
        data.chars.append(spaceEntry);
    }

    for ( SGFGlyph &glyph : spriteFont.glyphs )
    {
        for ( const SGFGlyph::KerningPair &kerning : glyph.kerningPairs )
        {
            qint16 amount = static_cast<qint16>(kerning.second);
            if ( amount == 0 ) {
                continue;
            }
            if ( !fromPng ) {
                const int hfA = hfAdvByChar.value(glyph.character);
                const int xa = xadvByChar.value(glyph.character);
                const int hfB = hfAdvByChar.value(kerning.first);
                const int xb = xadvByChar.value(kerning.first);
                if ( xa > hfA || xb > hfB ) {
                    continue;
                }
            }
            SGFBMFontKerningEntry k;
            k.first = glyph.character.unicode();
            k.second = kerning.first.unicode();
            k.amount = amount;
            data.kernings.append(k);
        }
    }

    return true;
}
