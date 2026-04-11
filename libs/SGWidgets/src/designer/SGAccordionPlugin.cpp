#include "designer/SGAccordionPlugin.h"

#include <QtPlugin>
#include <QtDesigner/QExtensionFactory>
#include <QtDesigner/QExtensionManager>
#include <QtDesigner/QDesignerFormEditorInterface>
#include <QtDesigner/QDesignerFormWindowInterface>
#include <QtDesigner/QDesignerContainerExtension>
#include <QtDesigner/QDesignerPropertySheetExtension>

#include "widgets/SGAccordion.h"

#include "designer/SGAccordionExtensionFactory.h"
#include "designer/SGAccordionContainerExtension.h"

SGAccordionPlugin::SGAccordionPlugin(QObject *parent)
    : QObject(parent)
{
    m_initialized = false;
}


//
// Plugin Methods
//

void SGAccordionPlugin::initialize(QDesignerFormEditorInterface * formEditor)
{
    if (m_initialized)
        return;

    QExtensionManager *manager = formEditor->extensionManager();
    QExtensionFactory *factory = new SGAccordionExtensionFactory(manager);

    Q_ASSERT(manager != 0);
    manager->registerExtensions(factory, Q_TYPEID(QDesignerContainerExtension));

    m_initialized = true;
}

bool SGAccordionPlugin::isInitialized() const
{
    return m_initialized;
}

QWidget *SGAccordionPlugin::createWidget(QWidget *parent)
{
    SGAccordion * widget = new SGAccordion(parent);
    return widget;
}

QString SGAccordionPlugin::name() const
{
    return QLatin1String("SGAccordion");
}

QString SGAccordionPlugin::group() const
{
    return QLatin1String("Sprite Glypher");
}

QIcon SGAccordionPlugin::icon() const
{
    return QIcon();
}

QString SGAccordionPlugin::toolTip() const
{
    return QLatin1String("An extended QToolBox container with multiple sections.");
}

QString SGAccordionPlugin::whatsThis() const
{
    return QLatin1String("An accordion container based around the QToolBox widget but extending it to allow for custom widgets to be placed in the page header and for other advanced features.");
}

bool SGAccordionPlugin::isContainer() const
{
    return true;
}

QString SGAccordionPlugin::domXml() const
{
    return QLatin1String("<ui language=\"c++\"> displayname=\"Accordion\">\n"
                         " <widget class=\"SGAccordion\" name=\"sgAccordion\"/>\n"
                         " <customwidgets>\n"
                         "  <customwidget>\n"
                         "   <class>SGAccordion</class>\n"
                         "   <addpagemethod>addPage</addpagemethod>\n"
                         "  </customwidget>\n"
                         " </customwidgets>\n"
                         "</ui>");
}

QString SGAccordionPlugin::includeFile() const
{
    return QLatin1String("widgets/SGAccordion.h");
}


#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN2(SGWidgets, SGAccordionPlugin)
#endif // QT_VERSION < 0x050000
