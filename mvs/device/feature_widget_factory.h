#ifndef FEATURE_WIDGET_FACTORY_H
#define FEATURE_WIDGET_FACTORY_H

#include <QWidget>
#include "feature_manager.h"

class FeatureWidgetFactory
{
public:
    FeatureWidgetFactory(QWidget* widget_parent);
    virtual ~FeatureWidgetFactory();

    QWidget* CreateFeatureWidget(const FeatureBase& feature_base);

private:
    QWidget* widget_parent_;
};

#endif // FEATURE_WIDGET_FACTORY_H
