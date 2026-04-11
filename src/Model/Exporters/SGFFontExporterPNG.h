#ifndef SGFFONTEXPORTERPNG_H
#define SGFFONTEXPORTERPNG_H

#include "SGFFontExporter.h"

class SGFFontExporterPNG : public SGFFontExporter
{
public:
    SGFFontExporterPNG();


public:
    virtual bool exportFont(SGFDocument *doc);
};

#endif // SGFFONTEXPORTERPNG_H
