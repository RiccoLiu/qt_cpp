#ifndef FEATURE_MANAGER_H
#define FEATURE_MANAGER_H

#include <QString>
#include <QList>

class FeatureManager;

class FeatureBase {
public:
    FeatureBase() {}
    FeatureBase(QString name, QString desc, QString node_name, QString type, QString name_space, QString visibility, QString streamable)
        : name_(name), descriptor_(desc), node_name_(node_name), type_(type), name_space_(name_space), visibility_(visibility), streamable_(streamable) {}
    virtual ~FeatureBase() {}

    virtual const QString& GetName() const {
        return name_;
    }
    virtual const QString& GetDescriptor() const {
        return descriptor_;
    }
    virtual const QString& GetNodeName() const {
        return node_name_;
    }
    virtual const QString& GetType() const {
        return type_;
    }
    virtual const QString& GetNameSpace() const {
        return name_space_;
    }
    virtual const QString& GetVisibility() const {
        return visibility_;
    }
    virtual const QString& GetStreamable() const {
        return streamable_;
    }
    virtual QString GetString() const {
        /*
        Device Control
        Category for device information and control.

        Node Name: DeviceControl
        Type: Category
        Name Space: Standard
        Visibility: Beginner
        Streamable: No
        */
        return name_ + "\n" +
               descriptor_ + "\n\n" +
               "Node Name: " + node_name_ + "\n" +
               "Type: " + type_ + "\n" +
               "Name Space: " + name_space_ + "\n" +
               "Visibility: " + visibility_ + "\n" +
               "Streamable: " + streamable_;
    }

private:
    QString name_;
    QString descriptor_;

    QString node_name_;
    QString type_;
    QString name_space_;
    QString visibility_;
    QString streamable_;
};

class Feature : public FeatureBase {
public:
    Feature() {}
    Feature(QString name, QString desc, QString node_name, QString type, QString name_space, QString visibility, QString streamable)
        : FeatureBase(name, desc, node_name, type, name_space, visibility, streamable) {}
    virtual ~Feature() {}

    const QList<QPair<QString, QString>>& GetAdditionalAttribute() const {
        return additional_attribute_;
    }

    void AddAttribute(const QString& key, const QString& value) {
        additional_attribute_.push_back(qMakePair(key, value));
    }

    virtual QString GetString() const override {
        QString str = FeatureBase::GetString();

        str.append("\n\n");

        for (auto it : additional_attribute_) {
            str.append(it.first).append(": ").append(it.second).append("\n");
        }
        return str;
    }

private:
    QList<QPair<QString, QString>> additional_attribute_;
};

class FeatureControl  : public FeatureBase {
public:
    FeatureControl() {}
    FeatureControl(QString name, QString desc, QString node_name, QString type, QString name_space, QString visibility, QString streamable)
         : FeatureBase(name, desc, node_name, type, name_space, visibility, streamable) {}
    virtual ~FeatureControl() {
        if (feature_.size() > 0) {
            feature_.clear();
        }
    }

    const QList<Feature*>& GetFeature() const {
        return feature_;
    }

    void AddFeature(std::unique_ptr<Feature> feature) {
        owned_feature_.push_back(std::move(feature));
        feature_.push_back(owned_feature_.back().get());
    }

    friend class FeatureManager;
private:
    QList<Feature*> feature_;
    std::vector<std::unique_ptr<Feature>> owned_feature_; // EthnetDevice & XhciDevice

};

class FeatureManager
{
public:
    FeatureManager();
    virtual ~FeatureManager() {}

    const QList<FeatureControl*>& GetFeatureControl() const {
        return feature_control_;
    }

    void AddFeatureControl(std::unique_ptr<FeatureControl> control) {
        owned_feature_control_.push_back(std::move(control));
        feature_control_.push_back(owned_feature_control_.back().get());
    }

    static std::unique_ptr<FeatureManager> CreateFeatureManager();

private:
    QList<FeatureControl*> feature_control_;
    std::vector<std::unique_ptr<FeatureControl>> owned_feature_control_; // EthnetDevice & XhciDevice
};

#endif // FEATURE_MANAGER_H
