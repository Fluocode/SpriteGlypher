#ifndef SGACCORDIONPLUGIN_H
#define SGACCORDIONPLUGIN_H

#include <QtDesigner/QDesignerCustomWidgetInterface>
#include <QtCore/QObject>

class SGAccordionPlugin : public QObject, public QDesignerCustomWidgetInterface
{
    Q_OBJECT
    Q_INTERFACES(QDesignerCustomWidgetInterface)

#if QT_VERSION >= 0x050000
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QDesignerCustomWidgetInterface")
#endif // QT_VERSION >= 0x050000

public:
    explicit SGAccordionPlugin(QObject *parent = 0);

    // QDesignerCustomWidgetInterface Methods
public:
    bool isContainer() const;
    bool isInitialized() const;
    QIcon icon() const;
    QString domXml() const;
    QString group() const;
    QString includeFile() const;
    QString name() const;
    QString toolTip() const;
    QString whatsThis() const;
    QWidget *createWidget(QWidget *parent);
    void initialize(QDesignerFormEditorInterface *formEditor);

private:
    bool m_initialized;
};

#endif // SGACCORDIONPLUGIN_H
