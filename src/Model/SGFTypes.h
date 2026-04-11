#ifndef SGFTYPES_H
#define SGFTYPES_H

#include <QImage>
#include <QMetaType>

#include "SGFGradient.h"

#include "SGFInputSettings.h"
#include "SGFGenerationSettings.h"
#include "SGFExportSettings.h"

struct SGFPadding
{
    float top;
    float right;
    float bottom;
    float left;

    SGFPadding(float paddingTop, float paddingRight, float paddingBottom, float paddingLeft) {
        top = paddingTop;
        right = paddingRight;
        bottom = paddingBottom;
        left = paddingLeft;
    }

    SGFPadding() {
        top = right = bottom = left = 0.f;
    }
};

Q_DECLARE_METATYPE(SGFPadding)


struct SGFPattern
{
    QImage image;           //< Raw image data to use as the pattern.
    float scale;            //< Scale of the provided image.
};

Q_DECLARE_METATYPE(SGFPattern)


#endif // SGFTYPES_H
