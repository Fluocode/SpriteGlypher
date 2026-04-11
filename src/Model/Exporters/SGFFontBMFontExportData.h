#ifndef SGFFONTBMFONTEXPORTDATA_H
#define SGFFONTBMFONTEXPORTDATA_H

#include <QString>
#include <QVector>
#include <QtGlobal>

class SGFDocument;

struct SGFBMFontCharEntry
{
    quint32 id = 0;
    int x = 0;
    int y = 0;
    int width = 0;
    int height = 0;
    int xoffset = 0;
    int yoffset = 0;
    int xadvance = 0;
    quint8 page = 0;
    quint8 chnl = 15;
};

struct SGFBMFontKerningEntry
{
    quint32 first = 0;
    quint32 second = 0;
    qint16 amount = 0;
};

struct SGFBMFontExportData
{
    QString face;
    int fontSize = 0;
    bool bold = false;
    bool italic = false;
    QString charset;
    bool unicode = true;
    int stretchH = 100;
    int smooth = 1;
    int aa = 1;
    int paddingUp = 0;
    int paddingRight = 0;
    int paddingDown = 0;
    int paddingLeft = 0;
    int spacingHoriz = 0;
    int spacingVert = 0;
    int outline = 0;

    int lineHeight = 0;
    int base = 0;
    int scaleW = 0;
    int scaleH = 0;
    int pages = 1;
    int packed = 0;
    int alphaChnl = 0;
    int redChnl = 4;
    int greenChnl = 4;
    int blueChnl = 4;

    QString page0FileName;

    QVector<SGFBMFontCharEntry> chars;
    QVector<SGFBMFontKerningEntry> kernings;
};

namespace SGFFontBMFontExport
{
    bool buildExportData(SGFDocument *doc, SGFBMFontExportData &out);
}

#endif // SGFFONTBMFONTEXPORTDATA_H
