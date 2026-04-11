#ifndef SGACCORDIONEXTENSIONFACTORY_H
#define SGACCORDIONEXTENSIONFACTORY_H

#include <QtDesigner/QExtensionFactory>
#include <QtDesigner/QExtensionManager>

class SGAccordionExtensionFactory : public QExtensionFactory
{

public:
    SGAccordionExtensionFactory(QExtensionManager *parent = 0);

protected:
    QObject *createExtension(QObject *object, const QString &iid, QObject *parent) const;

};

#endif // SGACCORDIONEXTENSIONFACTORY_H
