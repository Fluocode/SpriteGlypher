#ifndef SGFFONTEXPORTERBMFONTBINARY_H
#define SGFFONTEXPORTERBMFONTBINARY_H

#include "SGFFontExporter.h"

class SGFFontExporterBMFontBinary : public SGFFontExporter
{
public:
    SGFFontExporterBMFontBinary();

    bool exportFont(SGFDocument *doc) override;

private:
    bool writeDocumentToPath(SGFDocument *doc, const QString &path);
};

#endif // SGFFONTEXPORTERBMFONTBINARY_H
