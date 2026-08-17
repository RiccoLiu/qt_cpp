#ifndef DEVICE_MANAGER_H
#define DEVICE_MANAGER_H

#include <vector>
#include <memory>

#include <QString>
#include <QList>
#include <QMap>
#include <QRunnable>
#include <QObject>
#include <QFutureWatcher>
#include <QtConcurrent>

enum class DeviceType {
    GIGE,
    USB,
    CAMERA_LINK,
    GENTL
};

class Device {
public:
    Device() = delete;
    Device(QString name, DeviceType type) : name_(name), type_(type) {}
    virtual ~Device() {}

    virtual QString GetName() const {
        return name_;
    }
    virtual DeviceType GetType() const {
        return type_;
    }
    virtual const QList<QPair<QString, QString>>& GetProperty() const {
        return property_in_order_;
    }

    virtual void AddProperty(const QString& key, const QString& value) {
        // property_[key] = value;
        property_in_order_.push_back(qMakePair(key, value));
    }

private:
    QString name_;
    DeviceType type_;
    // QMap<QString, QString> property_;
    QList<QPair<QString, QString>> property_in_order_;
};

// Slave Device
class CameraDevice : public Device {
public:
    CameraDevice(QString name, DeviceType type)
        : Device(name, type) {}
};

// Host Device
class HostControlDevice : public Device {
public:
    HostControlDevice(QString name, DeviceType type)
        : Device(name, type) {}
    virtual ~HostControlDevice() {}

    virtual const QList<Device*>& GetSlaveDevice() const {
        return slave_device_;
    }

    virtual void AddDevice(std::unique_ptr<Device> device) {
        owned_slave_device_.push_back(std::move(device));
        slave_device_.push_back(owned_slave_device_.back().get());
    }

private:
    QList<Device*> slave_device_;
    // QList<std::unique_ptr<Device>> owned_slave_device_; // 使用QList报错: 不支持移动语义
    std::vector<std::unique_ptr<Device>> owned_slave_device_;
};

class EthnetDevice : public HostControlDevice {
public:
    EthnetDevice(QString name)
        : HostControlDevice(name, DeviceType::GIGE) {}
};

class XhciDevice : public HostControlDevice {
public:
    XhciDevice(QString name)
        : HostControlDevice(name, DeviceType::USB) {}
};

class DeviceManager : public QObject {
    Q_OBJECT

public:
    explicit DeviceManager(QObject* parent = nullptr)
        : QObject(parent) {}
    virtual ~DeviceManager() {}

    int StartScan() {
        QThreadPool::globalInstance()->start([this] () {
            // 1. 异步线程扫描所有设备
            auto devices = ScanDevice();

            // 2. 回到主线程更新成员变量，抛出扫描完成信号
            QMetaObject::invokeMethod(this, [this, new_devices = std::move(devices)] () mutable  { // 加上 mutable 后才能对 new_devices 进行修改或者移动
                 Q_ASSERT(QThread::currentThread() == qApp->thread());

                owned_device_ = std::move(new_devices);
                device_.clear();
                for (auto& dev : owned_device_) {
                    device_.push_back(dev.get());
                }

                emit ScanFinished();
            }, Qt::QueuedConnection); // Qt::QueuedConnection: 跨线程投递到目标对象所属线程执行; DeviceManager 是在主线程创建的，这段代码就会在主线程中执行。
        });
        return 0;
    }

    int GetDeviceCount() const {
        return device_.size();
    }
    Device* GetDevice(int index) const {
        return (index >= 0 && index < device_.size()) ? device_[index] : nullptr;
    }
    const QList<Device*>& GetDevice() const {
        return device_;
    }

signals:
    void ScanFinished();

private:
    std::vector<std::unique_ptr<Device>> ScanDevice();

private:
    QList<Device*> device_;
    std::vector<std::unique_ptr<Device>> owned_device_; // EthnetDevice & XhciDevice
};

/*
// 1. 如果使用线程池，需要将任务和管理分离，任务执行完结果放到管理中，任务自动回收
class Task : public QRunnable {
    void run() override { }
};

QThreadPool::globalInstance()->start(new Task);

// 2. QtConcurrent + QFutureWatcher
*/

#endif // DEVICE_MANAGER_H
