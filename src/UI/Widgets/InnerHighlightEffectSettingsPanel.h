#ifndef INNERHIGHLIGHTEFFECTSETTINGSPANEL_H
#define INNERHIGHLIGHTEFFECTSETTINGSPANEL_H

#include <QWidget>
#include "../../Model/Effects/SGFInnerHighlightEffect.h"

namespace Ui { class InnerHighlightEffectSettingsPanel; }

class InnerHighlightEffectSettingsPanel : public QWidget
{
    Q_OBJECT
public:
    explicit InnerHighlightEffectSettingsPanel(QWidget *parent = nullptr);
    ~InnerHighlightEffectSettingsPanel();

    void setValue(const SGFInnerHighlightEffectSettings &value);
    SGFInnerHighlightEffectSettings getValue() const;

signals:
    void valueChanged(SGFInnerHighlightEffectSettings value);

private slots:
    void subWidgetValueChanged();

private:
    void refreshVisibleElements();

private:
    Ui::InnerHighlightEffectSettingsPanel *ui;
    bool mIsUpdatingGui = false;
    SGFInnerHighlightEffectSettings mValue;
};

#endif // INNERHIGHLIGHTEFFECTSETTINGSPANEL_H

