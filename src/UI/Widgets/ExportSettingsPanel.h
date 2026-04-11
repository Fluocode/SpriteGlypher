#ifndef EXPORTSETTINGSPANEL_H
#define EXPORTSETTINGSPANEL_H

#include <QWidget>
#include "../../Model/Exporters/SGFFontExporter.h"
#include "../../Model/SGFExportSettings.h"

namespace Ui {
class ExportSettingsPanel;
}

class ExportSettingsPanel : public QWidget
{
    Q_OBJECT

public:
    explicit ExportSettingsPanel(QWidget *parent = 0);
    ~ExportSettingsPanel();

    void setValue(const SGFExportSettings & value);
    SGFExportSettings getValue() const;

signals:
    void valueChanged(SGFExportSettings value);

private slots:
    void subWidgetValueChanged();

    void filePathButtonClicked();

private:
    Ui::ExportSettingsPanel *ui;

    bool mIsUpdatingGui;
    SGFExportSettings mValue;

};

#endif // EXPORTSETTINGSPANEL_H
