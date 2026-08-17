#ifndef DEVICE_VIEW_H
#define DEVICE_VIEW_H

#include <QWidget>
#include <QAction>
#include <QStandardItemModel>
#include <QTreeView>

#include "device/device_manager.h"
#include "image_control.h"
// #include "controls/scannerview.h"

#include "scannerwindow.h"

class DeviceView : public QWidget
{
    Q_OBJECT
public:
    explicit DeviceView(QWidget *parent = nullptr);

    void RegisteControl(ImageControl* control);

signals:

private slots:
    void OnDeviceSelected(const QModelIndex& current, const QModelIndex& previous);
    void OnScanDeviceFinished();
    void OnConnectCamera();
    void OnStartStreaming();
    void OnStopStreaming();

private:
    QAction* connector_;
    QAction* start_streaming_;
    QAction* stop_streaming_;

    QStandardItemModel* device_tree_model_;
    QTreeView* device_tree_view_;

    QStandardItemModel* device_info_model_;
    QTreeView* device_info_view_;

    DeviceManager* device_manager_;

    QPointer<ScannerWindow> scanner_;
    ImageControl* img_control_;
};

#endif // DEVICE_VIEW_H
