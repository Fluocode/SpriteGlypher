#ifndef FILLEFFECTSETTINGSPANEL_H
#define FILLEFFECTSETTINGSPANEL_H

#include <QWidget>

#include "../../Model/Effects/SGFFillEffect.h"

namespace Ui {
class FillEffectSettingsPanel;
}

class FillEffectSettingsPanel : public QWidget
{
    Q_OBJECT

public:
    explicit FillEffectSettingsPanel(QWidget *parent = 0);
    ~FillEffectSettingsPanel();

    void setValue(const SGFFillEffectSettings & value);
    SGFFillEffectSettings getValue() const;

signals:
    void valueChanged(SGFFillEffectSettings value);

private slots:
    void subWidgetValueChanged();

private:
    void refreshVisibleElements();

private:
    Ui::FillEffectSettingsPanel *ui;

    bool mIsUpdatingGui;
    SGFFillEffectSettings mValue;
};

#endif // FILLEFFECTSETTINGSPANEL_H
