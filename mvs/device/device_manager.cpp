#include "device_manager.h"

std::vector<std::unique_ptr<Device>> DeviceManager::ScanDevice() {
    std::vector<std::unique_ptr<Device>> devices;

    std::unique_ptr<EthnetDevice> eth_2 = std::make_unique<EthnetDevice>("eth 2");
    eth_2->AddProperty({"ID"}, {"Realtek PCIe GbE Family Controller"});
    eth_2->AddProperty({"MAC"}, {"F0-2F-74-DD-FB-61"});
    eth_2->AddProperty({"IP"}, {"192.168.1.115"});
    eth_2->AddProperty({"MASK"}, {"255.255.255.0"});
    eth_2->AddProperty({"GATEWAY"}, {"192.168.1.1"});

    std::unique_ptr<CameraDevice> camera_0 = std::make_unique<CameraDevice>("camera_0", DeviceType::GIGE);
    camera_0->AddProperty({"vendor"}, {"Basler"});
    camera_0->AddProperty({"model"}, {"acA1920-40gc"});
    camera_0->AddProperty({"serialNumber"}, {"123456"});
    camera_0->AddProperty({"width"}, {"1920"});
    camera_0->AddProperty({"height"}, {"1080"});
    eth_2->AddDevice(std::move(camera_0));
    devices.push_back(std::move(eth_2));

    std::unique_ptr<EthnetDevice> eth_7 = std::make_unique<EthnetDevice>("eth 7");
    eth_7->AddProperty({"ID"}, {"ASIX USB to Gigabit Ethernet Family Adapter #2"});
    eth_7->AddProperty({"MAC"}, {"08-26-AE-3C-56-EF"});
    eth_7->AddProperty({"IP"}, {"0.0.0.0"});
    eth_7->AddProperty({"MASK"}, {"0.0.0.0"});
    eth_7->AddProperty({"GATEWAY"}, {"0.0.0.0"});

    std::unique_ptr<CameraDevice> camera_1 = std::make_unique<CameraDevice>("camera_1", DeviceType::GIGE);
    camera_1->AddProperty({"vendor"}, {"FLIR"});
    camera_1->AddProperty({"model"}, {"Blackfly S"});
    camera_1->AddProperty({"serialNumber"}, {"789012"});
    camera_1->AddProperty({"width"}, {"1280"});
    camera_1->AddProperty({"height"}, {"720"});
    eth_7->AddDevice(std::move(camera_1));
    devices.push_back(std::move(eth_7));

    std::unique_ptr<XhciDevice> xhci_0 = std::make_unique<XhciDevice>("xhci 0");
    xhci_0->AddProperty({"Describe"}, {"xhci 0"});
    xhci_0->AddProperty({"Vendor ID"}, {"0x10DE"});
    xhci_0->AddProperty({"Device ID"}, {"0x1AD6"});
    xhci_0->AddProperty({"Sub System ID"}, {"0x37151462"});
    xhci_0->AddProperty({"Version"}, {"0xA1"});
    devices.push_back(std::move(xhci_0));

    std::unique_ptr<XhciDevice> xhci_1 = std::make_unique<XhciDevice>("xhci 1");
    xhci_1->AddProperty({"Describe"}, {"xhci 1"});
    xhci_1->AddProperty({"Vendor ID"}, {"0x8086"});
    xhci_1->AddProperty({"Device ID"}, {"0x6ED"});
    xhci_1->AddProperty({"Sub System ID"}, {"0x86941043"});
    xhci_1->AddProperty({"Version"}, {"0x0"});
    devices.push_back(std::move(xhci_1));

    return devices;
}
