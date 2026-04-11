#ifndef SGFFONTEXPORTERBMFONT_H
#define SGFFONTEXPORTERBMFONT_H

#include "SGFFontExporter.h"

class SGFFontExporterBMFont : public SGFFontExporter
{
public:
    SGFFontExporterBMFont();


public:
    virtual bool exportFont(SGFDocument *doc);

private:
    bool writeDocumentToPath(SGFDocument * doc, const QString &path);
};

#endif // SGFFONTEXPORTERBMFONT_H
