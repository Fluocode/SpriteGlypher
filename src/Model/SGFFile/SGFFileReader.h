#ifndef SGFFILEREADER_H
#define SGFFILEREADER_H

#include "SGFDocumentElement.h"
#include "../SGFDocument.h"
#include "../SGFTypes.h"
#include "../Effects/SGFEffect.h"


class SGFFileReader
{
public:
    SGFFileReader();

    /** Load an SGF document object from the XML file at the given path. */
    SGFDocument::Ptr loadDocument(const QString &path);

    /** Get the last error that occured. */
    QString lastError();

private:
    SGFDocument::Ptr mDocument;
    QString mLastError;
};

#endif // SGFFILEREADER_H
