#include "SGFFontExporterPNG.h"

#include "../SGFDocument.h"

SGFFontExporterPNG::SGFFontExporterPNG()
{

}


bool SGFFontExporterPNG::exportFont(SGFDocument *doc)
{
    QString path = doc->getExportSettings().path;

    if ( doc == nullptr || doc->getSpriteFont().isNull ) {
        mLastError = "Null document or document with null sprite font provided for exporting.";
        return false;
    }

    if ( path.isNull() ) {
        mLastError = "Invalid path provided.";
        return false;
    }

    bool result = doc->getSpriteFont().textureAtlas.save(path, "PNG");

    if ( result == false ) {
        mLastError = "Save failed.";
    }

    return result;
}
