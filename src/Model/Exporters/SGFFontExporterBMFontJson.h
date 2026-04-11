#ifndef SGFFONTEXPORTERBMFONTJSON_H
#define SGFFONTEXPORTERBMFONTJSON_H

#include "SGFFontExporter.h"

class SGFFontExporterBMFontJson : public SGFFontExporter
{
public:
    SGFFontExporterBMFontJson();

    bool exportFont(SGFDocument *doc) override;

private:
    bool writeDocumentToPath(SGFDocument *doc, const QString &path);
};

#endif // SGFFONTEXPORTERBMFONTJSON_H
