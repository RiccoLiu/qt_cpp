
#include <QToolBar>
#include <QVBoxLayout>

#include "device_view.h"

DeviceView::DeviceView(QWidget *parent)
    : QWidget{parent}
    , connector_(new QAction(tr("Connect Camera"), this))
    , start_streaming_(new QAction(tr("Starting Streaming"), this))
    , stop_streaming_ (new QAction(tr("Stop Streaming"), this))
    , device_tree_model_(new QStandardItemModel(this))
    , device_tree_view_(new QTreeView(this))
    , device_info_model_(new QStandardItemModel(this))
    , device_info_view_(new QTreeView(this))
    , device_manager_(new DeviceManager(this))
{
    setMinimumSize(200, 300);

    connect(connector_, &QAction::triggered, this, &DeviceView::OnConnectCamera);
    connect(start_streaming_, &QAction::triggered, this, &DeviceView::OnStartStreaming);
    connect(stop_streaming_, &QAction::triggered, this, &DeviceView::OnStopStreaming);

    QToolBar* toolbar = new QToolBar(this);
    toolbar->addAction(connector_);
    toolbar->addAction(start_streaming_);
    toolbar->addAction(stop_streaming_);

    device_tree_view_->setModel(device_tree_model_);
    device_tree_view_->setHeaderHidden(true);

    device_info_view_->setModel(device_info_model_);
    device_info_view_->setHeaderHidden(true);

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(toolbar);
    layout->addWidget(device_tree_view_);
    layout->addWidget(device_info_view_);
    layout->setContentsMargins(0, 0, 0, 0); // 移除边距
    layout->setSpacing(0);                  // 移除间距

    device_tree_view_->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
    device_info_view_->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);

    connect(device_tree_view_->selectionModel(), &QItemSelectionModel::currentChanged, this, &DeviceView::OnDeviceSelected);
    connect(device_manager_, &DeviceManager::ScanFinished, this, &DeviceView::OnScanDeviceFinished);
}

void DeviceView::OnDeviceSelected(const QModelIndex& current, const QModelIndex& previous) {
    Q_UNUSED(previous);

    device_info_model_->clear();
    device_info_model_->setColumnCount(2);

    if (!current.isValid())
        return;

    // 从模型项中取出设备信息
    QStandardItem* item = device_tree_model_->itemFromIndex(current);
    if (!item)
        return;

    QVariant data = item->data(Qt::UserRole);
    if (!data.canConvert<Device*>())
        return;

    Device* dev = data.value<Device*>();

    QStandardItem* interface_info = new QStandardItem("Interface Info");
    const QList<QPair<QString, QString>>& props = dev->GetProperty();
    for (auto it = props.cbegin(); it != props.cend(); ++it) {
        interface_info->appendRow( { new QStandardItem(it->first) ,  new QStandardItem(it->second)} );
    }
    device_info_model_->appendRow(interface_info);

    // 展开并调整列宽
    device_info_view_->expandAll();
    device_info_view_->resizeColumnToContents(0);
    device_info_view_->resizeColumnToContents(1);
}

void DeviceView::OnScanDeviceFinished() {
    device_tree_model_->clear();

    QStandardItem* gige = new QStandardItem(tr("GigE"));
    QStandardItem* usb = new QStandardItem(tr("USB"));
    QStandardItem* camera_link = new QStandardItem(tr("Camera Link"));
    QStandardItem* gentl = new QStandardItem(tr("GenTL"));

    for (Device* device : device_manager_->GetDevice()) {
        if (device->GetType() == DeviceType::GIGE) {
            QStandardItem* item = new QStandardItem(device->GetName());
            HostControlDevice* host = dynamic_cast<HostControlDevice*>(device);
            if (host) {
                for (auto slave : host->GetSlaveDevice()) {
                    QStandardItem* slave_item = new QStandardItem(slave->GetName());
                    slave_item->setData(QVariant::fromValue(slave), Qt::UserRole); // 关键：保存完整设备信息
                    item->appendRow(slave_item);
                }
            }
            item->setData(QVariant::fromValue(device), Qt::UserRole); // 关键：保存完整设备信息
            gige->appendRow(item);
        } else if (device->GetType() == DeviceType::USB) {
            QStandardItem* item = new QStandardItem(device->GetName());
            HostControlDevice* host = dynamic_cast<HostControlDevice*>(device);
            if (host) {
                for (auto slave : host->GetSlaveDevice()) {
                    QStandardItem* slave_item = new QStandardItem(slave->GetName());
                    slave_item->setData(QVariant::fromValue(slave), Qt::UserRole); // 关键：保存完整设备信息
                    item->appendRow(slave_item);
                }
            }
            item->setData(QVariant::fromValue(device), Qt::UserRole); // 关键：保存完整设备信息
            usb->appendRow(item);
        } else if (device->GetType() == DeviceType::CAMERA_LINK) {
            QStandardItem* item = new QStandardItem(device->GetName());
            HostControlDevice* host = dynamic_cast<HostControlDevice*>(device);
            if (host) {
                for (auto slave : host->GetSlaveDevice()) {
                    QStandardItem* slave_item = new QStandardItem(slave->GetName());
                    slave_item->setData(QVariant::fromValue(slave), Qt::UserRole); // 关键：保存完整设备信息
                    item->appendRow(slave_item);
                }
            }
            item->setData(QVariant::fromValue(device), Qt::UserRole); // 关键：保存完整设备信息
            camera_link->appendRow(item);
        } else if (device->GetType() == DeviceType::GENTL) {
            QStandardItem* item = new QStandardItem(device->GetName());
            HostControlDevice* host = dynamic_cast<HostControlDevice*>(device);
            if (host) {
                for (auto slave : host->GetSlaveDevice()) {
                    QStandardItem* slave_item = new QStandardItem(slave->GetName());
                    slave_item->setData(QVariant::fromValue(slave), Qt::UserRole); // 关键：保存完整设备信息
                    item->appendRow(slave_item);
                }
            }
            item->setData(QVariant::fromValue(device), Qt::UserRole); // 关键：保存完整设备信息
            gentl->appendRow(item);
        }
    }
    device_tree_model_->appendRow(gige);
    device_tree_model_->appendRow(usb);
    device_tree_model_->appendRow(camera_link);
    device_tree_model_->appendRow(gentl);

    device_tree_view_->expandAll();
}

void DeviceView::OnConnectCamera() {
    device_manager_->StartScan();
}

void DeviceView::OnStartStreaming() {
    if (!scanner_) {
        scanner_ = new ScannerWindow();
        scanner_->setAttribute(Qt::WA_DeleteOnClose);

        if (img_control_) {
            scanner_->RegisteControl(img_control_);
        }
    }
    scanner_->show();
    scanner_->raise();
    scanner_->activateWindow();
}

void DeviceView::OnStopStreaming() {
    if (scanner_) {
        scanner_->close(); // 触发 WA_DeleteOnClose ，自动销毁并触发 destroyed 信号将 scanner_ 置空
    }
}

void DeviceView::RegisteControl(ImageControl* control) {
    qDebug() << "--- OnStopStreaming ----";
    if (control) {
        img_control_ = control;
    }
}
