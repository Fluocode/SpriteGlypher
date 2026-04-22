#include "SGFFontExporterBMFontJson.h"

#include "SGFFontBMFontExportData.h"
#include "SGFFontExporter.h"
#include "../SGFDocument.h"
#include "../SGFExportSettings.h"

#include <QFile>
#include <QFileInfo>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

SGFFontExporterBMFontJson::SGFFontExporterBMFontJson()
{
}

bool SGFFontExporterBMFontJson::exportFont(SGFDocument *doc)
{
    if ( doc == nullptr || doc->getSpriteFont().isNull ) {
        mLastError = "Null document or document with null sprite font provided for exporting.";
        return false;
    }

    QString path = SGFFontExporter::normalizedExportBasePath(doc, doc->getExportSettings().path);
    if ( path.isNull() ) {
        mLastError = "Invalid path provided.";
        return false;
    }

    bool result = writeDocumentToPath(doc, path);

    if ( result && doc->getExportSettings().retinaCopy )
    {
        SGFDocument::Ptr retinaDoc = doc->clone();
        retinaDoc->scale(2.0f);
        retinaDoc->generateSpriteFont();
        result = writeDocumentToPath(retinaDoc.get(), path + QStringLiteral("@2x"));
    }

    return result;
}

bool SGFFontExporterBMFontJson::writeDocumentToPath(SGFDocument *doc, const QString &path)
{
    if ( path.isNull() ) {
        mLastError = "Invalid path provided.";
        return false;
    }

    QString jsonPath = path;
    if ( !jsonPath.endsWith(QStringLiteral(".json"), Qt::CaseInsensitive) ) {
        jsonPath.append(QStringLiteral(".json"));
    }

    QString pngPath = jsonPath;
    pngPath.replace(QStringLiteral(".json"), QStringLiteral(".png"), Qt::CaseInsensitive);

    SGFBMFontExportData data;
    if ( !SGFFontBMFontExport::buildExportData(doc, data) ) {
        mLastError = "Failed to build export data.";
        return false;
    }

    const QFileInfo pngInfo(pngPath);
    const QString pngDir = pngInfo.dir().absolutePath();
    const QString pngBase = pngInfo.completeBaseName();
    const QString pngExt = pngInfo.suffix().isEmpty() ? QStringLiteral("png") : pngInfo.suffix();

    data.pageFileNames.clear();
    if ( data.pages <= 1 ) {
        data.pageFileNames.append(pngInfo.fileName());
    } else {
        for ( int i = 0; i < data.pages; ++i ) {
            data.pageFileNames.append(QStringLiteral("%1_%2.%3").arg(pngBase).arg(i).arg(pngExt));
        }
    }
    data.page0FileName = !data.pageFileNames.isEmpty() ? data.pageFileNames.first() : pngInfo.fileName();

    QString padding = QStringLiteral("%1,%2,%3,%4")
            .arg(data.paddingUp)
            .arg(data.paddingRight)
            .arg(data.paddingDown)
            .arg(data.paddingLeft);
    QString spacing = QStringLiteral("%1,%2").arg(data.spacingHoriz).arg(data.spacingVert);

    QJsonObject info;
    info.insert(QStringLiteral("face"), data.face);
    info.insert(QStringLiteral("size"), data.fontSize);
    info.insert(QStringLiteral("bold"), data.bold ? 1 : 0);
    info.insert(QStringLiteral("italic"), data.italic ? 1 : 0);
    info.insert(QStringLiteral("charset"), data.charset);
    info.insert(QStringLiteral("unicode"), data.unicode ? 1 : 0);
    info.insert(QStringLiteral("stretchH"), data.stretchH);
    info.insert(QStringLiteral("smooth"), data.smooth);
    info.insert(QStringLiteral("aa"), data.aa);
    info.insert(QStringLiteral("padding"), padding);
    info.insert(QStringLiteral("spacing"), spacing);
    info.insert(QStringLiteral("outline"), data.outline);

    QJsonObject common;
    common.insert(QStringLiteral("lineHeight"), data.lineHeight);
    common.insert(QStringLiteral("base"), data.base);
    common.insert(QStringLiteral("scaleW"), data.scaleW);
    common.insert(QStringLiteral("scaleH"), data.scaleH);
    common.insert(QStringLiteral("pages"), data.pages);
    common.insert(QStringLiteral("packed"), data.packed);
    common.insert(QStringLiteral("alphaChnl"), data.alphaChnl);
    common.insert(QStringLiteral("redChnl"), data.redChnl);
    common.insert(QStringLiteral("greenChnl"), data.greenChnl);
    common.insert(QStringLiteral("blueChnl"), data.blueChnl);

    QJsonArray pages;
    {
        for ( int i = 0; i < data.pageFileNames.size(); ++i ) {
            QJsonObject p;
            p.insert(QStringLiteral("id"), i);
            p.insert(QStringLiteral("file"), data.pageFileNames[i]);
            pages.append(p);
        }
    }

    QJsonArray chars;
    for ( const SGFBMFontCharEntry &glyph : data.chars )
    {
        QJsonObject ch;
        ch.insert(QStringLiteral("id"), static_cast<double>(glyph.id));
        ch.insert(QStringLiteral("x"), glyph.x);
        ch.insert(QStringLiteral("y"), glyph.y);
        ch.insert(QStringLiteral("width"), glyph.width);
        ch.insert(QStringLiteral("height"), glyph.height);
        ch.insert(QStringLiteral("xoffset"), glyph.xoffset);
        ch.insert(QStringLiteral("yoffset"), glyph.yoffset);
        ch.insert(QStringLiteral("xadvance"), glyph.xadvance);
        ch.insert(QStringLiteral("page"), glyph.page);
        ch.insert(QStringLiteral("chnl"), glyph.chnl);
        chars.append(ch);
    }

    QJsonArray kernings;
    for ( const SGFBMFontKerningEntry &k : data.kernings )
    {
        QJsonObject ko;
        ko.insert(QStringLiteral("first"), static_cast<double>(k.first));
        ko.insert(QStringLiteral("second"), static_cast<double>(k.second));
        ko.insert(QStringLiteral("amount"), k.amount);
        kernings.append(ko);
    }

    QJsonObject root;
    root.insert(QStringLiteral("info"), info);
    root.insert(QStringLiteral("common"), common);
    root.insert(QStringLiteral("pages"), pages);
    root.insert(QStringLiteral("chars"), chars);
    root.insert(QStringLiteral("kernings"), kernings);

    QFile file(jsonPath);
    if ( !file.open(QIODevice::WriteOnly | QIODevice::Truncate) ) {
        mLastError = "Could not open descriptor file for writing.";
        return false;
    }

    QJsonDocument docJson(root);
    file.write(docJson.toJson(QJsonDocument::Indented));
    file.close();

    // Write texture pages
    bool pngOk = true;
    const SGFSpriteFont &sf = doc->getSpriteFont();
    const QVector<QImage> pageImgs = !sf.textureAtlases.isEmpty()
        ? sf.textureAtlases
        : QVector<QImage>({ sf.textureAtlas });

    for ( int i = 0; i < pageImgs.size(); ++i ) {
        const QString outPath = (data.pages <= 1)
            ? pngPath
            : (pngDir + QStringLiteral("/") + data.pageFileNames[i]);
        if ( !pageImgs[i].save(outPath, "PNG") ) {
            pngOk = false;
            break;
        }
    }

    if ( !pngOk ) {
        mLastError = "Save failed.";
    }

    return pngOk;
}
