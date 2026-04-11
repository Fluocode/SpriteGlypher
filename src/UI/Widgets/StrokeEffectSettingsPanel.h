#ifndef STROKEEFFECTSETTINGSPANEL_H
#define STROKEEFFECTSETTINGSPANEL_H

#include <QWidget>

#include "../../Model/Effects/SGFStrokeEffect.h"

namespace Ui {
class StrokeEffectSettingsPanel;
}

class StrokeEffectSettingsPanel : public QWidget
{
    Q_OBJECT

public:
    explicit StrokeEffectSettingsPanel(QWidget *parent = 0);
    ~StrokeEffectSettingsPanel();

    void setValue(const SGFStrokeEffectSettings & value);
    SGFStrokeEffectSettings getValue() const;

signals:
    void valueChanged(SGFStrokeEffectSettings value);

private slots:
    void subWidgetValueChanged();

private:
    Ui::StrokeEffectSettingsPanel *ui;
    bool mIsUpdatingGui;
    SGFStrokeEffectSettings mValue;

};

#endif // STROKEEFFECTSETTINGSPANEL_H
