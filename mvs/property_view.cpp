#include <QToolBar>
#include <QGroupBox>
#include <QVBoxLayout>

#include <QPushButton>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QComboBox>
#include <QFormLayout>
#include <QMessageBox>
#include <QCheckBox>
#include <QSpinBox>

#include "property_view.h"
#include "device/feature_widget_factory.h"

FeatureTreeWidget::FeatureTreeWidget(QWidget *parent)
    : QWidget{parent}
{
    SetupUi();
}

void FeatureTreeWidget::SetupUi() {
    // 1.创建搜索框 - 组合控件
    SearchBar* search_bar = new SearchBar(tr("Search Feature..."), this);
    connect(search_bar, &SearchBar::searchRequested, this, &FeatureTreeWidget::OnSearch);

    // 2.树形图
    // 2.1. 创建树形图
    feature_tree_ = new QTreeWidget(this);
    // feature_tree->setHeaderLabels({"Feature", ""}); // 第二列放控件
    feature_tree_->setHeaderHidden(true);
    feature_tree_->setColumnCount(2);

    feature_tree_->header()->setSectionResizeMode(0, QHeaderView::Stretch);  // 第一列自适应
    feature_tree_->header()->setSectionResizeMode(1, QHeaderView::Fixed);    // 第二列固定宽度
    feature_tree_->header()->resizeSection(1, 120);                          // 第二列列宽 120
    feature_tree_->header()->setStretchLastSection(true);                    // 最后一列自动拉伸
    feature_tree_->setAlternatingRowColors(false);                           // 斑马纹

    // 2.2. 获取所有 Feature,添加叶子节点
    FeatureWidgetFactory factory(feature_tree_);
    feature_manager_ = FeatureManager::CreateFeatureManager();
    for (FeatureControl* feature_control : feature_manager_->GetFeatureControl()) {
        QTreeWidgetItem* control_item = new QTreeWidgetItem(feature_tree_, {feature_control->GetName()});
        control_item->setData(0, Qt::UserRole, QVariant::fromValue(feature_control));
        control_item->setExpanded(true);
        control_item->setFirstColumnSpanned(true);

        for (Feature* feature : feature_control->GetFeature()) {
            QTreeWidgetItem* feature_item = new QTreeWidgetItem(control_item, {feature->GetName()});
            feature_item->setData(0, Qt::UserRole, QVariant::fromValue(feature));
            QWidget* feature_widget = factory.CreateFeatureWidget(*feature);
            if (feature_widget) {
                feature_tree_->setItemWidget(feature_item, 1, feature_widget);
            }
        }
    }

    connect(feature_tree_, &QTreeWidget::currentItemChanged, this, &FeatureTreeWidget::OnFeatureSelected);

    // 3.信息视图
    feature_info_ = new QTextEdit(this);
    feature_info_->setText(tr("No Info"));

    // 4.状态视图
    QComboBox* user_level = new QComboBox(this);
    user_level->addItems({"Guru", "Expert", "Bignners"});
    user_level->setCurrentIndex(1);

    QFormLayout* bottom_layout = new QFormLayout(); // 不要这样写 new QHBoxLayout(feature_widget);
    bottom_layout->addRow(tr("User Level"), user_level);

    // 5. 垂直布局
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(search_bar);
    layout->addWidget(feature_tree_, 2);
    layout->addWidget(feature_info_, 1);
    layout->addLayout(bottom_layout);
    setLayout(layout);
}

void FeatureTreeWidget::OnSearch(const QString &keyword) {
    QList<QTreeWidgetItem*> items = feature_tree_->findItems(keyword, Qt::MatchContains | Qt::MatchRecursive, 0);
    if (items.isEmpty()) {
        QMessageBox::information(this, "查找结果", "未找到匹配项。");
    } else {
        QTreeWidgetItem* first = items.first();
        feature_tree_->setCurrentItem(first);
        feature_tree_->scrollToItem(first, QAbstractItemView::PositionAtCenter);
        feature_tree_->setFocus();
    }
}

void FeatureTreeWidget::OnFeatureSelected(QTreeWidgetItem *current, QTreeWidgetItem *previous) {
    Q_UNUSED(previous);

    QVariant data = current->data(0, Qt::UserRole);

    FeatureBase* feature_base = nullptr;
    if (data.canConvert<Feature*>()) {
        feature_base  = data.value<Feature*>();
    }
    if (data.canConvert<FeatureControl*>()) {
        feature_base = data.value<FeatureControl*>();
    }
    if (!feature_base) {
        qDebug() <<  "get feature base failed..";
        return;
    }
    feature_info_->setText(feature_base->GetString());
}

CommonFeatureWidget::CommonFeatureWidget(QWidget *parent)
    : QWidget{parent}
    , search_bar_(new SearchBar(tr("Search Feature"), this))
{
    SetupUi();
}

QValidator* CreateQValidator(double min, double max, int decimals, QObject* parent) {
    QDoubleValidator* validator = new QDoubleValidator(min, max, decimals, parent); // 允许输入 0.1 ~ 1000000 最多 2 位小数
    validator->setNotation(QDoubleValidator::StandardNotation); // 避免科学计数法
    return validator;
}

void CommonFeatureWidget::SetupUi() {
    // 1. 创建 此页面的 控件
    QCheckBox* fps_en = new QCheckBox(this);
    fps_en->setCheckable(true); // 默认为 true, 选则 false 时: 用户不能勾选
    fps_en->setChecked(true);

    QLineEdit* set_fps = new QLineEdit(this);
    set_fps->setValidator(CreateQValidator(0.1, 1000000, 4, this));

    QLineEdit* act_fps = new QLineEdit(this);
    act_fps->setEnabled(false);

    QComboBox* exposure_auto = new QComboBox(this);
    exposure_auto->addItem(tr("Off"));
    exposure_auto->addItem(tr("Once"));
    exposure_auto->addItem(tr("Continue"));
    exposure_auto->setCurrentIndex(0);

    QLineEdit* exposure_time = new QLineEdit(this);
    exposure_time->setValidator(CreateQValidator(15, 9999500, 4, this));

    QComboBox* gain_auto = new QComboBox(this);
    gain_auto->addItem(tr("Off"));
    gain_auto->addItem(tr("Once"));
    gain_auto->addItem(tr("Continue"));
    gain_auto->setCurrentIndex(0);

    QLineEdit* gain = new QLineEdit(this);
    gain->setValidator(CreateQValidator(0, 19.96333, 4, this));

    QSpinBox* width = new QSpinBox(this);
    width->setRange(32, 4096);
    width->setSingleStep(4);

    QSpinBox* height = new QSpinBox(this);
    height->setRange(32, 1592);
    height->setSingleStep(4);

    QComboBox* format = new QComboBox(this);
    format->addItem(tr("Mono 8"));
    format->addItem(tr("Mono 10"));
    format->addItem(tr("Mono 12"));
    format->setCurrentIndex(2);

    QComboBox* test_pattern = new QComboBox(this);
    test_pattern->addItem(tr("Off"));
    test_pattern->addItem(tr("Mono Bar"));
    test_pattern->addItem(tr("Checkboard"));
    test_pattern->setCurrentIndex(0);

    QLineEdit* digital_shift = new QLineEdit(this);
    digital_shift->setEnabled(false);

    QCheckBox* gamma_en = new QCheckBox(this);
    QComboBox* gamma_selector = new QComboBox(this);
    gamma_selector->addItem(tr("User"));
    gamma_selector->addItem(tr("sRGB"));
    gamma_selector->setCurrentIndex(0);

    QLineEdit* gamma = new QLineEdit(this);
    gamma->setValidator(CreateQValidator(0, 4, 4, this));
    gamma->setEnabled(false);

    QCheckBox* sharpness_en = new QCheckBox(this);
    QSpinBox* sharpness = new QSpinBox(this);
    // sharpness->setValidator(CreateQValidator(0, 100, 4, common_feature));
    sharpness->setRange(0, 100);
    sharpness->setSingleStep(4);
    sharpness->setEnabled(false);

    QSpinBox* scps_packet_size = new QSpinBox(this);
    scps_packet_size->setRange(220, 9156);
    scps_packet_size->setSingleStep(4);

    QSpinBox* scpd = new QSpinBox(this);
    scpd->setRange(0, 1000000);
    scpd->setSingleStep(4);

    QCheckBox* pause_frame_reception = new QCheckBox(this);

    // 2. 控件连接信号槽
    // TODO:
    connect(search_bar_, &SearchBar::searchRequested, this, &CommonFeatureWidget::OnSearch);

    // 3. GroupBox 分区
    // 3.1. Basic Features
    QGroupBox* basic_features_box = new QGroupBox(tr("Basic Features"), this);
    basic_features_box->setCheckable(true);
    basic_features_box->setChecked(true);

    QFormLayout* form = new QFormLayout(basic_features_box);
    form->addRow(tr("Acquisition Frame Rate Control Enable"), fps_en);
    form->addRow(tr("Acquisition Frame Rate(Fps)"), set_fps);
    form->addRow(tr("Resulting Frame Rate(Fps)"), act_fps);
    form->addRow(tr("Exposure Auto"), exposure_auto);
    form->addRow(tr("Exposure Time(us)"), exposure_time);
    form->addRow(tr("Gain Auto"), gain_auto);
    form->addRow(tr("Gain(dB)"), gain);
    form->addRow(tr("Width"), width);
    form->addRow(tr("Height"), height);
    form->addRow(tr("Pixel Format"), format);
    form->addRow(tr("Test Pattern"), test_pattern);
    form->addRow(tr("digital Shift"), digital_shift);

    // 3.2. ISP
    QGroupBox* isp_box = new QGroupBox(tr("ISP"), this);
    isp_box->setCheckable(true);
    isp_box->setChecked(true);

    QFormLayout* form2 = new QFormLayout(isp_box);
    form2->addRow(tr("Gamma Enable"), gamma_en);
    form2->addRow(tr("Gamma Selector"), gamma_selector);
    form2->addRow(tr("Gamma"), gamma);
    form2->addRow(tr("Shapness Enable"), sharpness_en);
    form2->addRow(tr("Sharpness"), sharpness);

    // 3.3. Transport Layer Controler
    QGroupBox* transport_layer_controler = new QGroupBox(tr("Transport Layer Controler"), this);
    transport_layer_controler->setCheckable(true);
    transport_layer_controler->setChecked(true);

    QFormLayout* form3 = new QFormLayout(transport_layer_controler);
    form3->addRow(tr("GEV SCPS PAcket Size(B)"), scps_packet_size);
    form3->addRow(tr("GEV SCPD"), scpd);
    form3->addRow(tr("GEV Pause Frame Reception"), pause_frame_reception);

    // 4. 垂直布局
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(search_bar_);
    layout->addWidget(basic_features_box);
    layout->addWidget(isp_box);
    layout->addWidget(transport_layer_controler);
    setLayout(layout);
}

void CommonFeatureWidget::OnSearch(QString text) {
    qDebug() << "CommonFeatureWidget::search:" << text;
}

TestWidget::TestWidget(QWidget* parent)
    : QWidget(parent)
    , fps_ (new QSpinBox(this))
    , gamma_(new QDoubleSpinBox(this))
{
    fps_->setRange(1, 60);
    fps_->setSingleStep(5);

    gamma_->setRange(0.1, 5.0);
    gamma_->setSingleStep(0.1);

    QGroupBox* test_box = new QGroupBox(tr("Test Gourp"), this);
    test_box->setCheckable(true);
    test_box->setChecked(true);

    QFormLayout* form = new QFormLayout(test_box);
    form->addRow(tr("fps"), fps_);
    form->addRow(tr("gamma"), gamma_);

    CircleButton *btn0 = new CircleButton("OK", this);
    connect(btn0, &CircleButton::clicked, [](){
        qDebug() << "OK Circle button clicked!";
    });

    RingProgress *ring = new RingProgress(this);
    RingProgressPerformance *ringPerformance = new RingProgressPerformance(this);

    QPushButton *resetBtn = new QPushButton("Reset", this);

    // auto *timeline = new TimelineWidget(this);
    // timeline->addClip(0, 5000, "Intro");
    // timeline->addClip(5000, 12000, "Main Content");
    // timeline->addClip(17000, 3000, "Outro");
    // timeline->addClip(20000, 8000, "B-Roll");

    // connect(timeline, &TimelineWidget::clipSelectionChanged, [](const QStringList &names) {
    //     qDebug() << "Selected:" << names;
    // });

    connect(resetBtn, &QPushButton::clicked, [ring]{ ring->animateTo(0.0, 500); });
    connect(resetBtn, &QPushButton::clicked, [ringPerformance]{ ringPerformance->animateTo(0.0, 500); });

    QVBoxLayout* layout = new QVBoxLayout();
    layout->addWidget(test_box);
    layout->addWidget(btn0);
    layout->addWidget(ring);
    layout->addWidget(ringPerformance);
    layout->addWidget(resetBtn);
    // layout->addWidget(timeline);
    setLayout(layout);

    QTimer* timer = new QTimer(this);
    connect(timer,&QTimer::timeout, [ring] () {
        qreal progress = ring->progress();
        qreal newProgress = progress + 0.1;
        qDebug() << "ring: progress:" << progress << ", newProgress:" << newProgress;

        ring->animateTo(newProgress, 500);
    });

    timer->start(1000); // 默认循环触发，每1s调用一次超时函数

#if 1
    QTimer* timer2 = new QTimer(this);
    connect(timer2,&QTimer::timeout, [ringPerformance] () {
        qreal progress = ringPerformance->progress();
        qreal newProgress = progress + 0.1;
        qDebug() << "ring: progress:" << progress << ", newProgress:" << newProgress;

        ringPerformance->animateTo(newProgress, 500);
    });

    timer2->start(1000); // 默认循环触发，每1s调用一次超时函数
#endif
}

void TestWidget::RegisteControl(ImageControl* control) {
    if (control) {
        connect(fps_, &QSpinBox::valueChanged, control, &ImageControl::SetFps);
        connect(gamma_, &QDoubleSpinBox::valueChanged, control, &ImageControl::SetGamma);
    }
}

PropertyView::PropertyView(QWidget *parent)
    : QWidget{parent}
    , import_(new QAction(tr("Import"), this))
    , property_tab_(new QTabWidget(this))
    , feature_tree_widget_(new FeatureTreeWidget(this))
    , common_feature_widget_(new CommonFeatureWidget(this))
    , test_widget_(new TestWidget(this))
{
    // 1. 工具栏
    QToolBar* toolbar = new QToolBar(tr("Property Toolbar"), this);
    toolbar->addAction(import_);

    // 2. Tab 栏
    property_tab_->addTab(feature_tree_widget_, tr("Feature Tree"));
    property_tab_->addTab(common_feature_widget_, tr("Common Features"));
    property_tab_->addTab(test_widget_, tr("Test Widget"));
    // Trigger
    // AdvancedFeature

    // 3.QGroupBox 模拟子窗口标题栏
    QGroupBox* group = new QGroupBox(tr("SCAN"));
    QVBoxLayout* group_layout = new QVBoxLayout(group);
    group_layout->addWidget(toolbar);
    group_layout->addWidget(property_tab_);

    // 4.垂直布局
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(group);
    setLayout(layout);
}

void PropertyView::RegisteControl(ImageControl* control) {
    if (control) {
        img_ctl_ = control;
        test_widget_->RegisteControl(img_ctl_);
    }
}
