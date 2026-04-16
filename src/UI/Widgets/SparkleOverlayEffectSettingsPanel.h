#ifndef SPARKLEOVERLAYEFFECTSETTINGSPANEL_H
#define SPARKLEOVERLAYEFFECTSETTINGSPANEL_H

#include <QWidget>
#include "../../Model/Effects/SGFSparkleOverlayEffect.h"

namespace Ui { class SparkleOverlayEffectSettingsPanel; }

class SparkleOverlayEffectSettingsPanel : public QWidget
{
    Q_OBJECT
public:
    explicit SparkleOverlayEffectSettingsPanel(QWidget *parent = nullptr);
    ~SparkleOverlayEffectSettingsPanel();

    void setValue(const SGFSparkleOverlayEffectSettings &value);
    SGFSparkleOverlayEffectSettings getValue() const;

signals:
    void valueChanged(SGFSparkleOverlayEffectSettings value);

private slots:
    void subWidgetValueChanged();

private:
    Ui::SparkleOverlayEffectSettingsPanel *ui;
    bool mIsUpdatingGui = false;
    SGFSparkleOverlayEffectSettings mValue;
};

#endif // SPARKLEOVERLAYEFFECTSETTINGSPANEL_H

