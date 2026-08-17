#ifndef PROPERTY_VIEW_H
#define PROPERTY_VIEW_H

#include <QWidget>
#include <QTabWidget>
#include <QAction>
#include <QWidget>
#include <QLineEdit>
#include <QTreeWidget>
#include <QTextEdit>

#include <QDoubleSpinBox>
#include <QSpinBox>

#include "image_control.h"
#include "device/feature_manager.h"
#include "controls/searchbar.h"
#include "controls/circlebutton.h"
#include "controls/ringprogress.h"
#include "controls/ringprogress_performance.h"
#include "controls/timeline_widget.h"

class FeatureTreeWidget : public QWidget
{
    Q_OBJECT
public:
    explicit FeatureTreeWidget(QWidget *parent = nullptr);

    void SetupUi();


signals:

private slots:
    void OnSearch(const QString &keyword);
    void OnFeatureSelected(QTreeWidgetItem *current, QTreeWidgetItem *previous);

private:
    QTreeWidget* feature_tree_;
    QTextEdit* feature_info_;

    std::unique_ptr<FeatureManager> feature_manager_;
};

class CommonFeatureWidget : public QWidget
{
    Q_OBJECT
public:
    explicit CommonFeatureWidget(QWidget *parent = nullptr);

    void SetupUi();

private slots:
    void OnSearch(QString text);

private:
    SearchBar* search_bar_;
};

class TestWidget : public QWidget
{
    Q_OBJECT
public:
    explicit TestWidget(QWidget *parent = nullptr);

    void RegisteControl(ImageControl* control);

private:
    QSpinBox* fps_;
    QDoubleSpinBox* gamma_;
};

class PropertyView : public QWidget
{
    Q_OBJECT
public:
    explicit PropertyView(QWidget *parent = nullptr);

    void RegisteControl(ImageControl* control);

signals:

private:
    QAction* import_;
    QTabWidget* property_tab_;

    FeatureTreeWidget* feature_tree_widget_;
    CommonFeatureWidget* common_feature_widget_;
    TestWidget* test_widget_;

    ImageControl* img_ctl_;
};

#endif // PROPERTY_VIEW_H
