#ifndef OUTERGLOWEFFECTSETTINGSPANEL_H
#define OUTERGLOWEFFECTSETTINGSPANEL_H

#include <QWidget>
#include "../../Model/Effects/SGFOuterGlowEffect.h"

namespace Ui { class OuterGlowEffectSettingsPanel; }

class OuterGlowEffectSettingsPanel : public QWidget
{
    Q_OBJECT
public:
    explicit OuterGlowEffectSettingsPanel(QWidget *parent = nullptr);
    ~OuterGlowEffectSettingsPanel();

    void setValue(const SGFOuterGlowEffectSettings &value);
    SGFOuterGlowEffectSettings getValue() const;

signals:
    void valueChanged(SGFOuterGlowEffectSettings value);

private slots:
    void subWidgetValueChanged();

private:
    Ui::OuterGlowEffectSettingsPanel *ui;
    bool mIsUpdatingGui = false;
    SGFOuterGlowEffectSettings mValue;
};

#endif // OUTERGLOWEFFECTSETTINGSPANEL_H

