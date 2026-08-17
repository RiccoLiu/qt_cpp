
#include <QComboBox>
#include <QLabel>
#include <QSpinBox>

#include "feature_widget_factory.h"

FeatureWidgetFactory::FeatureWidgetFactory(QWidget* widget_parent)
    :  widget_parent_(widget_parent) {}

FeatureWidgetFactory::~FeatureWidgetFactory() {}

QWidget* FeatureWidgetFactory::CreateFeatureWidget(const FeatureBase& feature_base) {
    QWidget* widget = nullptr;
    if (feature_base.GetNodeName() == "DeviceType") {
        QComboBox* cb = new QComboBox(widget_parent_);
        cb->addItems({"transmitter"});
        cb->setCurrentIndex(0);
        widget = cb;
    } else if (feature_base.GetNodeName() == "DeviceScanType") {
        QComboBox* cb = new QComboBox(widget_parent_);
        cb->addItems({"arescan"});
        cb->setCurrentIndex(0);
        widget = cb;
    } else if (feature_base.GetNodeName() == "DeviceVendorName") {
        QLabel* label = new QLabel("GEV", widget_parent_);
        widget = label;
    } else if (feature_base.GetNodeName() == "WidthMax") {
        QSpinBox* width_max = new QSpinBox(widget_parent_);
        width_max->setRange(0, 4096);
        width_max->setSingleStep(4);
        widget = width_max;
    } else if (feature_base.GetNodeName() == "HeightMax") {
        QSpinBox* height_max = new QSpinBox(widget_parent_);
        height_max->setRange(0, 2160);
        height_max->setSingleStep(4);
        widget = height_max;
    } else if (feature_base.GetNodeName() == "RegionSelector") {
        QComboBox* cb = new QComboBox(widget_parent_);
        cb->addItems({"Region 0"});
        cb->setCurrentIndex(0);
        widget = cb;
    } else if (feature_base.GetNodeName() == "AcquisitionMode") {
        QComboBox* cb = new QComboBox(widget_parent_);
        cb->addItems({"Continus", "SingleFrame"});
        cb->setCurrentIndex(0);
        widget = cb;
    }
    return widget;
}
