#ifndef UNDERLAYEFFECTSETTINGSPANEL_H
#define UNDERLAYEFFECTSETTINGSPANEL_H

#include <QWidget>
#include "../../Model/Effects/SGFUnderlayEffect.h"

namespace Ui {
class UnderlayEffectSettingsPanel;
}

class UnderlayEffectSettingsPanel : public QWidget
{
    Q_OBJECT

public:
    explicit UnderlayEffectSettingsPanel(QWidget *parent = nullptr);
    ~UnderlayEffectSettingsPanel();

    void setValue(const SGFUnderlayEffectSettings &value);
    SGFUnderlayEffectSettings getValue() const;

signals:
    void valueChanged(SGFUnderlayEffectSettings value);

private slots:
    void subWidgetValueChanged();

private:
    void refreshVisibleElements();

private:
    Ui::UnderlayEffectSettingsPanel *ui;
    bool mIsUpdatingGui = false;
    SGFUnderlayEffectSettings mValue;
};

#endif // UNDERLAYEFFECTSETTINGSPANEL_H

