#include "designer/SGAccordionExtensionFactory.h"

#include "designer/SGAccordionContainerExtension.h"
#include "widgets/SGAccordion.h"



SGAccordionExtensionFactory::SGAccordionExtensionFactory(QExtensionManager *parent) :
    QExtensionFactory(parent)
{

}

QObject *SGAccordionExtensionFactory::createExtension(QObject *object, const QString &iid, QObject *parent) const
{
    SGAccordion *widget = qobject_cast<SGAccordion*>(object);

    if (widget && (iid == Q_TYPEID(QDesignerContainerExtension))) {
        return new SGAccordionContainerExtension(widget, parent);
    } else {
        return 0;
    }
}
