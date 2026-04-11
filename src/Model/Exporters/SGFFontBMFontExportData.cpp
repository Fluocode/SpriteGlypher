#include "SGFFontBMFontExportData.h"

#include "../SGFDocument.h"
#include "../SGFInputSettings.h"
#include "../SGFGenerationSettings.h"
#include "../SGFSpriteFont.h"
#include "../SGFGlyph.h"

#include <QFontMetrics>

#include <algorithm>

bool SGFFontBMFontExport::buildExportData(SGFDocument *doc, SGFBMFontExportData &data)
{
    if ( doc == nullptr || doc->getSpriteFont().isNull ) {
        return false;
    }

    SGFInputSettings inSettings = doc->getInputSettings();
    SGFGenerationSettings genSettings = doc->getGenerationSettings();
    SGFSpriteFont spriteFont = doc->getSpriteFont();

    const bool fromPng = (inSettings.inputSource == SGFInputSource::PngSprites);

    QFont qfont(inSettings.fontFamily, qRound(inSettings.fontSize));
    QFontMetrics fontMetrics(qfont);

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
            maxH = std::max(maxH, h);
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
            entry.xadvance = std::max(1, w);
        } else {
            QRect boundingRect = fontMetrics.boundingRect(glyph.character);
            entry.xoffset = static_cast<int>(boundingRect.x() + glyph.effectPadding.left);
            entry.yoffset = static_cast<int>(fontMetrics.height() + boundingRect.y() - fontMetrics.descent()
                - glyph.effectPadding.bottom);
            entry.xadvance = static_cast<int>(fontMetrics.horizontalAdvance(glyph.character));
        }

        data.chars.append(entry);
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
            spaceEntry.xadvance = std::max(1, data.lineHeight / 4);
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
            SGFBMFontKerningEntry k;
            k.first = glyph.character.unicode();
            k.second = kerning.first.unicode();
            k.amount = amount;
            data.kernings.append(k);
        }
    }

    return true;
}
