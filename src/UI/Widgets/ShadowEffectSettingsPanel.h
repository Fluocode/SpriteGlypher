#ifndef SHADOWEFFECTSETTINGSPANEL_H
#define SHADOWEFFECTSETTINGSPANEL_H

#include <QWidget>

#include "../../Model/Effects/SGFShadowEffect.h"

namespace Ui {
class ShadowEffectSettingsPanel;
}

class ShadowEffectSettingsPanel : public QWidget
{
    Q_OBJECT

public:
    explicit ShadowEffectSettingsPanel(QWidget *parent = 0);
    ~ShadowEffectSettingsPanel();

    void setValue(const SGFShadowEffectSettings & value);
    SGFShadowEffectSettings getValue() const;

signals:
    void valueChanged(SGFShadowEffectSettings value);

private slots:
    void subWidgetValueChanged();

private:
    Ui::ShadowEffectSettingsPanel *ui;
    bool mIsUpdatingGui;
    SGFShadowEffectSettings mValue;

};

#endif // SHADOWEFFECTSETTINGSPANEL_H
