#ifndef SGFFILEWRITER_H
#define SGFFILEWRITER_H

#include <QtCore>
#include <QtXml>

#include "../SGFDocument.h"

class SGFFileWriter
{
public:
    static const QString kSGFRootKey;
    static const QString kSGFEffectsKey;

public:
    SGFFileWriter(QString path);

    bool writeDocument(SGFDocument * document);

    QString lastError();

private:
    QString mPath;
    QString mLastError;
};

#endif // SGFFILEWRITER_H
