#include "feature_manager.h"
#include "device_manager.h"

FeatureManager::FeatureManager() {}


std::unique_ptr<FeatureManager> FeatureManager::CreateFeatureManager() {
    std::unique_ptr<FeatureManager> feature_manager = std::make_unique<FeatureManager>();

    std::unique_ptr<Feature> dev_type = std::make_unique<Feature>("Device Type", "Returns the device type.", "DeviceType", "Enumeration", "Standard", "Guru", "No");

    dev_type->AddAttribute("Transmitter", "");
    dev_type->AddAttribute("Enum Entry Name", "Transmitter");
    dev_type->AddAttribute("Enum Entry Value", "0");
    dev_type->AddAttribute("Name Space", "Standard");
    dev_type->AddAttribute("Visibility", "Beginner");

    std::unique_ptr<Feature> scan_type = std::make_unique<Feature>("Device Scan Type", "Scan type of the sensor.", "DeviceScanType", "Enumeration", "Standard", "Expert", "No");

    scan_type->AddAttribute("Areascan", "");
    scan_type->AddAttribute("Enum Entry Name", "Areascan");
    scan_type->AddAttribute("Enum Entry Value", "0");
    scan_type->AddAttribute("Name Space", "Standard");
    scan_type->AddAttribute("Visibility", "Beginner");

    std::unique_ptr<Feature> vendor_name = std::make_unique<Feature>("Device Vendor Name", "Name of the manufacturer of the device.", "DeviceVendorName", "StringReg", "Standard", "Beginner", "No");

    std::unique_ptr<FeatureControl> device_control = std::make_unique<FeatureControl>("Device Control", "Category for device information and control.", "DeviceControl", "Category", "Standard", "Beginner", "No");
    device_control->AddFeature(std::move(dev_type));
    device_control->AddFeature(std::move(scan_type));
    device_control->AddFeature(std::move(vendor_name));

    feature_manager->AddFeatureControl(std::move(device_control));

    std::unique_ptr<Feature> max_width = std::make_unique<Feature>("Width Max", "Maximum width of the image (in pixels).", "WidthMax", "Integer", "Standard", "Expert", "No");
    max_width->AddAttribute("Min", "4096");
    max_width->AddAttribute("Max", "4096");
    max_width->AddAttribute("Inc", "1");

    std::unique_ptr<Feature> max_height = std::make_unique<Feature>("Height Max", "Maximum height of the image (in pixels).", "HeightMax", "Integer", "Standard", "Expert", "No");
    max_height->AddAttribute("Min", "2160");
    max_height->AddAttribute("Max", "2160");
    max_height->AddAttribute("Inc", "1");

    std::unique_ptr<Feature> region_selector = std::make_unique<Feature>("Region Selector", "Selects the Region of interest to control. The RegionSelector feature allows devices that are able to extract multiple regions out of an image, to configure the features of those individual regions independently.",
                            "RegionSelector", "Enumeration", "Standard", "Beginner", "No");
    region_selector->AddAttribute("Region 0", "");
    region_selector->AddAttribute("Enum Entry Name", "Region0");
    region_selector->AddAttribute("Enum Entry Value", "0");
    region_selector->AddAttribute("Name Space", "Standard");
    region_selector->AddAttribute("Visibility", "Beginner");

    std::unique_ptr<FeatureControl> image_format_control = std::make_unique<FeatureControl>("Image Format Control", "Category for image format control features.", "ImageFormatControl", "Category", "Standard", "Beginner", "No");
    image_format_control->AddFeature(std::move(max_width));
    image_format_control->AddFeature(std::move(max_height));
    image_format_control->AddFeature(std::move(region_selector));

    feature_manager->AddFeatureControl(std::move(image_format_control));

    std::unique_ptr<Feature> acq_mode = std::make_unique<Feature>("Acquisition Mode", "Sets the acquisition mode of the device.", "AcquisitionMode", "Enumeration", "Standard", "Beginner", "Yes");
    acq_mode->AddAttribute("Continuous", "");
    acq_mode->AddAttribute("Enum Entry Name", "Continuous");
    acq_mode->AddAttribute("Enum Entry Value", "2");
    acq_mode->AddAttribute("Name Space", "Standard");
    acq_mode->AddAttribute("Visibility", "Beginner");

    std::unique_ptr<FeatureControl> acquisition_control = std::make_unique<FeatureControl>("Acquisition Control", "Category for the acquisition and trigger control features.", "AcquisitionControl", "Category", "Standard", "Beginner", "No");
    acquisition_control->AddFeature(std::move(acq_mode));

    feature_manager->AddFeatureControl(std::move(acquisition_control));
    return feature_manager;
}

#if 0

std::unique_ptr<FeatureManager> FeatureManager::CreateFeatureManager() {
    std::unique_ptr<FeatureManager> feature_manager = std::make_unique<FeatureManager>();
#if 1
    // auto device_control = std::make_unique();
    FeatureControl device_control("Device Control", "Category for device information and control.", "DeviceControl", "Category", "Standard", "Beginner", "No");

    Feature dev_type("Device Type", "Returns the device type.", "DeviceType", "Enumeration", "Standard", "Guru", "No");
    dev_type.AddAttribute("Transmitter", "");
    dev_type.AddAttribute("Enum Entry Name", "Transmitter");
    dev_type.AddAttribute("Enum Entry Value", "0");
    dev_type.AddAttribute("Name Space", "Standard");
    dev_type.AddAttribute("Visibility", "Beginner");

    Feature scan_type("Device Scan Type", "Scan type of the sensor.", "DeviceScanType", "Enumeration", "Standard", "Expert", "No");
    scan_type.AddAttribute("Areascan", "");
    scan_type.AddAttribute("Enum Entry Name", "Areascan");
    scan_type.AddAttribute("Enum Entry Value", "0");
    scan_type.AddAttribute("Name Space", "Standard");
    scan_type.AddAttribute("Visibility", "Beginner");

    Feature vendor_name("Device Vendor Name", "Name of the manufacturer of the device.", "DeviceVendorName", "StringReg", "Standard", "Beginner", "No");
    // device_control.feature_ = {dev_type, scan_type, vendor_name};

    // device_control.owned_feature_ = {
    //     std::make_unique<Feature>(dev_type),
    //     std::make_unique<Feature>(scan_type),
    //     std::make_unique<Feature>(vendor_name)

    // };


    // device_control.owned_feature_.clear();
    // device_control.owned_feature_.reserve(3); // 可选：预分配空间

    // device_control.owned_feature_.emplace_back(std::make_unique<Feature>(dev_type));
    // device_control.owned_feature_.emplace_back(std::make_unique<Feature>(scan_type));
    // device_control.owned_feature_.emplace_back(std::make_unique<Feature>(vendor_name));

    // device_control.feature_.clear();

    // for (auto& fea : device_control.owned_feature_) {
    //     device_control.feature_.push_back(fea.get());
    // }

    FeatureControl image_format_control("Image Format Control", "Category for image format control features.", "ImageFormatControl", "Category", "Standard", "Beginner", "No");

    Feature max_width("Width Max", "Maximum width of the image (in pixels).", "WidthMax", "Integer", "Standard", "Expert", "No");
    max_width.AddAttribute("Min", "4096");
    max_width.AddAttribute("Max", "4096");
    max_width.AddAttribute("Inc", "1");

    Feature max_height("Height Max", "Maximum height of the image (in pixels).", "HeightMax", "Integer", "Standard", "Expert", "No");
    max_height.AddAttribute("Min", "2160");
    max_height.AddAttribute("Max", "2160");
    max_height.AddAttribute("Inc", "1");

    Feature region_selector("Region Selector", "Selects the Region of interest to control. The RegionSelector feature allows devices that are able to extract multiple regions out of an image, to configure the features of those individual regions independently.",
                            "RegionSelector", "Enumeration", "Standard", "Beginner", "No");
    region_selector.AddAttribute("Region 0", "");
    region_selector.AddAttribute("Enum Entry Name", "Region0");
    region_selector.AddAttribute("Enum Entry Value", "0");
    region_selector.AddAttribute("Name Space", "Standard");
    region_selector.AddAttribute("Visibility", "Beginner");

    // image_format_control.feature_ = {max_width, max_height, region_selector};

    // image_format_control.owned_feature_ = {
    //     std::make_unique<Feature>(max_width),
    //     std::make_unique<Feature>(max_height),
    //     std::make_unique<Feature>(region_selector)
    // };
#if 0
    image_format_control.owned_feature_.clear();
    image_format_control.owned_feature_.reserve(3); // 可选：预分配空间

    image_format_control.owned_feature_.emplace_back(std::make_unique<Feature>(max_width));
    image_format_control.owned_feature_.emplace_back(std::make_unique<Feature>(max_height));
    image_format_control.owned_feature_.emplace_back(std::make_unique<Feature>(region_selector));

    image_format_control.feature_.clear();

    for (const auto& fea : image_format_control.owned_feature_) {
        image_format_control.feature_.push_back(fea.get());
    }

#endif
    FeatureControl acquisition_control("Acquisition Control", "Category for the acquisition and trigger control features.", "AcquisitionControl", "Category", "Standard", "Beginner", "No");

    Feature acq_mode("Acquisition Mode", "Sets the acquisition mode of the device.", "AcquisitionMode", "Enumeration", "Standard", "Beginner", "Yes");

    acq_mode.AddAttribute("Continuous", "");
    acq_mode.AddAttribute("Enum Entry Name", "Continuous");
    acq_mode.AddAttribute("Enum Entry Value", "2");
    acq_mode.AddAttribute("Name Space", "Standard");
    acq_mode.AddAttribute("Visibility", "Beginner");

    // acquisition_control.feature_ = {acq_mode};

    // acquisition_control.owned_feature_ = {
    //     std::make_unique<Feature>(acq_mode)
    // };

#if 0
    acquisition_control.owned_feature_.clear();
    acquisition_control.owned_feature_.reserve(1); // 可选：预分配空间

    acquisition_control.owned_feature_.emplace_back(std::make_unique<Feature>(acq_mode));

    acquisition_control.feature_.clear();

    for (const auto& fea : acquisition_control.owned_feature_) {
        acquisition_control.feature_.push_back(fea.get());
    }
#endif
    feature_manager->feature_control_ = {std::move(device_control), std::move(image_format_control), std::move(acquisition_control)};
#endif
    return feature_manager;
}
#endif
