#ifndef SGFFONTEXPORTERBMFONTXML_H
#define SGFFONTEXPORTERBMFONTXML_H

#include "SGFFontExporter.h"

class SGFFontExporterBMFontXml : public SGFFontExporter
{
public:
    SGFFontExporterBMFontXml();

    bool exportFont(SGFDocument *doc) override;

private:
    bool writeDocumentToPath(SGFDocument *doc, const QString &path);
};

#endif // SGFFONTEXPORTERBMFONTXML_H
