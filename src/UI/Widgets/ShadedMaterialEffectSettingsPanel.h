#ifndef SHADEDMATERIALEFFECTSETTINGSPANEL_H
#define SHADEDMATERIALEFFECTSETTINGSPANEL_H

#include <QWidget>

#include "../../Model/Effects/SGFShadedMaterialEffect.h"

namespace Ui {
class ShadedMaterialEffectSettingsPanel;
}

class ShadedMaterialEffectSettingsPanel : public QWidget
{
    Q_OBJECT

public:
    explicit ShadedMaterialEffectSettingsPanel(QWidget *parent = nullptr);
    ~ShadedMaterialEffectSettingsPanel();

    void setValue(const SGFShadedMaterialEffectSettings &value);
    SGFShadedMaterialEffectSettings getValue() const;

signals:
    void valueChanged(SGFShadedMaterialEffectSettings value);

private slots:
    void subWidgetValueChanged();
    void onPresetMatte();
    void onPresetGlossy();
    void onPresetChrome();
    void onMatcapBrowse();
    void onMatcapUseDefault();

private:
    void pullFromWidgets();
    void refreshMatcapPathLabel();

    Ui::ShadedMaterialEffectSettingsPanel *ui;
    bool mIsUpdatingGui = false;
    SGFShadedMaterialEffectSettings mValue;
};

#endif
