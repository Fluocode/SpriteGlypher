#ifndef SGFINNERBEVEL_H
#define SGFINNERBEVEL_H

#include <QImage>
#include <QPainterPath>

namespace SGFInnerBevel {

/** Directional inner emboss on existing pixels, clipped to the alpha mask of @a path.
 *  @param amount 0–100 (0 = off); @a angleDeg light direction; @a blurPx softens the mask edge;
 *  @param opacity01 multiplies strength (e.g. effect opacity);
 *  @param intensity 100 = default strength; higher = stronger highlight/shadow. */
void applyToImage(QImage &image, const QPainterPath &path, float amount, float angleDeg, float blurPx, float opacity01, float intensity);

}

#endif
