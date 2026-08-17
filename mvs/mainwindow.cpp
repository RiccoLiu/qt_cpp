#include <QToolBar>
#include <QFormLayout>
#include <QComboBox>
#include <QSpinBox>
#include <QPushButton>
#include <QMessageBox>
#include <QFileInfo>
#include <QFileDialog>
#include <QActionGroup>
#include <QToolButton>
#include <QGroupBox>
#include <QDockWidget>
#include <QTreeWidget>

#include <QCheckBox>
#include <QLineEdit>
#include <QSplitter>
#include <QHeaderView>
#include <QLabel>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "controls/mygl.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , device_view_(new DeviceView(this))
    , image_view_(new ImageView(this))
    , property_view_(new PropertyView(this))
    , img_ctl_(new ImageControl(this))
{
    ui->setupUi(this);

    setWindowTitle("Machine Vision Software");

    CreateActions();
    CreateMenus();
    CreateToolBars();
    SetupLayout();

    image_view_->RegisteControl(img_ctl_);
    property_view_->RegisteControl(img_ctl_);
    device_view_->RegisteControl(img_ctl_);

    statusBar()->showMessage(tr("Ready"));
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::CreateActions() {
    // 这里的Action 既要放在菜单中也要在工具栏中使用
    open_act_ = new QAction(tr("&Open"), this);
    open_act_->setIcon(QIcon::fromTheme("document-open"));   // 设置图标 QIcon(":/icons/refresh.png")
    open_act_->setShortcut(QKeySequence::Open);              // 设置快捷键

    save_act_ = new QAction(tr("&Save"), this);
    save_act_->setIcon(QIcon::fromTheme("document-save"));
    save_act_->setShortcut(QKeySequence::Save);
    save_act_->setEnabled(false);

    save_as_act_ = new QAction(tr("Save as"), this);
    save_as_act_->setIcon(QIcon::fromTheme("document-save-as"));
    save_as_act_->setShortcut(QKeySequence::SaveAs);
    save_as_act_->setEnabled(false);

    exit_act_ = new QAction(tr("Exit"), this);

    // 画面布局（下拉式菜单）: 单画面、四画面、九画面
    screen_layout_ = new QToolButton(this);
    screen_layout_->setText(tr("Screen Layout"));
    screen_layout_->setIcon(QIcon(":/png/screen_layout.png"));

    // screen_layout_->setPopupMode(QToolButton::DelayedPopup);    // 短按执行动作、长按出现下拉菜单
    // screen_layout_->setPopupMode(QToolButton::MenuButtonPopup); // 点击按钮执行动作、点击下拉按钮出现下拉菜单
    screen_layout_->setPopupMode(QToolButton::InstantPopup);    // 点击开启下拉菜单

    QMenu* screen_menu = new QMenu(tr("Screen Menu"), screen_layout_);
    QAction* single_screen = screen_menu->addAction(tr("Single Screen"));
    QAction* four_screen = screen_menu->addAction(tr("Four Screen"));
    QAction* nine_screen = screen_menu->addAction(tr("Nine Screen"));

    screen_layout_->setMenu(screen_menu);

    single_screen->setCheckable(true);
    four_screen->setCheckable(true);
    nine_screen->setCheckable(true);
    single_screen->setChecked(true);

    QActionGroup* screen_group = new QActionGroup(screen_menu);
    screen_group->setExclusive(true);
    screen_group->addAction(single_screen);
    screen_group->addAction(four_screen);
    screen_group->addAction(nine_screen);

    status_= new QAction(tr("Status"), this);
    // status_->setIcon(QIcon::fromTheme(""));

    watermark_ = new QAction(tr("Watermark"), this);
    // watermark_->setIcon(QIcon::fromTheme(""));

    log_ = new QAction(tr("Log View"), this);
    // log_->setIcon(QIcon::fromTheme(""));

    refresh_ = new QAction(tr("Refresh"), this);
    refresh_->setIcon(QIcon::fromTheme("view-refresh"));

    connect(open_act_, &QAction::triggered, this, &MainWindow::OnOpenFile);
    connect(save_act_, &QAction::triggered, this, &MainWindow::OnSaveFile);
    connect(save_as_act_, &QAction::triggered, this, &MainWindow::OnSaveAsFile);
    connect(exit_act_, &QAction::triggered, this, &MainWindow::OnExitFile);

    connect(single_screen, &QAction::triggered, this, [this] () { image_view_->SetImageViewCount(1); });
    connect(four_screen, &QAction::triggered, this, [this] () { image_view_->SetImageViewCount(4); });
    connect(nine_screen, &QAction::triggered, this, [this] () { image_view_->SetImageViewCount(9); });
    connect(status_, &QAction::triggered, this, &MainWindow::OnStatus);
    connect(watermark_, &QAction::triggered, this, &MainWindow::OnWatermark);
    connect(log_, &QAction::triggered, this, &MainWindow::OnLog);
    connect(refresh_, &QAction::triggered, this, &MainWindow::OnRefresh);
}

void MainWindow::CreateMenus() {
    // file
    QMenu* file = menuBar()->addMenu(tr("File"));
    file->addAction(open_act_);

    recent_file_menu_ = new QMenu(tr("Open Recent"), this);
    UpdateOpenRecentFiles();
    file->addMenu(recent_file_menu_);

    file->addSeparator();
    file->addAction(save_act_);
    file->addAction(save_as_act_);
    file->addSeparator();
    file->addAction(exit_act_);

    // view
    QMenu* view = menuBar()->addMenu(tr("View"));
    QMenu* display_method = view->addMenu(tr("Display Method"));

    QAction* fps30 = display_method->addAction(tr("30 FPS"));
    QAction* fps60 = display_method->addAction(tr("60 FPS"));

    // 对钩单选: fps30 & fps 60
    QActionGroup* fps_group = new QActionGroup(display_method);
    fps_group->setExclusive(true);

    fps_group->addAction(fps30);
    fps_group->addAction(fps60);

    fps30->setCheckable(true);
    fps60->setCheckable(true);
    fps30->setChecked(true);

    filter_method_ = view->addMenu(tr("filter method"));
    filter_method_->setEnabled(false);

    vsync_method_ = view->addMenu(tr("vsync method"));
    vsync_method_->setEnabled(false);

    QMenu* render_method = view->addMenu(tr("render method"));

    QAction* d3d = render_method->addAction(tr("D3D"));
    QAction* gdi = render_method->addAction(tr("GDI"));

    // 对钩单选: D3D & GDI
    QActionGroup* render_group = new QActionGroup(render_method);
    render_group->setExclusive(true);
    render_group->addAction(d3d);
    render_group->addAction(gdi);

    d3d->setCheckable(true);
    gdi->setCheckable(true);
    d3d->setChecked(true);

    connect(fps30, &QAction::triggered, this, [this] () { OnSetFps(30); });
    connect(fps60, &QAction::triggered, this, [this] () { OnSetFps(60); });
    connect(d3d, &QAction::triggered, this, &MainWindow::OnRenderD3d);
    connect(gdi, &QAction::triggered, this, &MainWindow::OnRenderGDI);

    // Setup
    QMenu* setup = menuBar()->addMenu(tr("Setup"));
    QAction* general = setup->addAction(tr("General"));
    QAction* capture = setup->addAction(tr("Capture"));
    QAction* cache = setup->addAction(tr("Cetwork"));
    QAction* resend_packet = setup->addAction(tr("Resend Packet"));
    QAction* shortcut = setup->addAction(tr("Shortcut"));

    connect(general, &QAction::triggered, this, &MainWindow::OnGeneral);
    connect(capture, &QAction::triggered, this, &MainWindow::OnCapture);
    connect(cache, &QAction::triggered, this, &MainWindow::OnCache);
    connect(resend_packet, &QAction::triggered, this, &MainWindow::OnResendPacket);
    connect(shortcut, &QAction::triggered, this, &MainWindow::OnShortcut);

    // Tool
    QMenu* tool = menuBar()->addMenu(tr("Tool"));
    QAction* ip_config = tool->addAction(tr("IP Config"));
    QAction* firmware_upgrade = tool->addAction(tr("Firmware Upgrade"));
    QAction* import_export = tool->addAction(tr("Import And Export"));
    QAction* log_view = tool->addAction(tr("Log View"));
    QAction* virtual_camera = tool->addAction(tr("Virtual Camera"));
    tool->addSeparator();
    QAction* bandwidth_management = tool->addAction(tr("Bandwidth Management"));
    QAction* gige_action_command = tool->addAction(tr("Gige Action Command"));
    QAction* export_all_devices = tool->addAction(tr("Export All Devices"));

    connect(ip_config, &QAction::trigger, this, &MainWindow::OnIpConfig);
    connect(firmware_upgrade, &QAction::trigger, this, &MainWindow::OnFirmwareUpgrade);
    connect(import_export, &QAction::trigger, this, &MainWindow::OnImportExport);
    connect(log_view, &QAction::trigger, this, &MainWindow::OnLogView);
    connect(virtual_camera, &QAction::trigger, this, &MainWindow::OnVirtualCamera);
    connect(bandwidth_management, &QAction::trigger, this, &MainWindow::OnBandwidthManagement);
    connect(gige_action_command, &QAction::trigger, this, &MainWindow::OnGigeActionCommand);
    connect(export_all_devices, &QAction::trigger, this, &MainWindow::OnExportAllDevices);

    // help
    QMenu* help = menuBar()->addMenu(tr("Help"));

    QMenu* language = help->addMenu(tr("Language"));
    QAction* chinese = language->addAction(tr("Chinese"));
    QAction* english = language->addAction(tr("English"));

    // 对钩单选: chinese & english
    QActionGroup* language_group = new QActionGroup(language);
    language_group->setExclusive(true);

    language_group->addAction(chinese);
    language_group->addAction(english);

    chinese->setCheckable(true);
    english->setCheckable(true);
    chinese->setChecked(true);

    QAction* user_manual = help->addAction(tr("User Manual"));
    QAction* development = help->addAction(tr("Development"));
    QAction* about = help->addAction(tr("About"));

    connect(chinese, &QAction::triggered, this, &MainWindow::OnChinese);
    connect(english, &QAction::triggered, this, &MainWindow::OnEnglish);
    connect(user_manual, &QAction::triggered, this, &MainWindow::OnUserManual);
    connect(development, &QAction::triggered, this, &MainWindow::OnDevelopment);
    connect(about, &QAction::triggered, this, &MainWindow::OnAbout);
}

void MainWindow::CreateToolBars() {
    QAction* test = new QAction("test", this);
    connect(test, &QAction::triggered, this, [this] () {
        qDebug() << "show mygl";
        if (!gl_) {
            gl_ = new MyGLWidget();
            gl_->setAttribute(Qt::WA_DeleteOnClose);               // 设置属性: 窗口关闭时自动释放内存, 防止内存泄漏
        }
        gl_->show();
        gl_->raise();
        gl_->activateWindow();
    });

    QToolBar* toolbar = addToolBar(tr("Main"));
    toolbar->addAction(open_act_);
    toolbar->addAction(save_act_);
    toolbar->addAction(save_as_act_);
    toolbar->addSeparator();
    toolbar->addWidget(screen_layout_);
    toolbar->addAction(status_);
    toolbar->addAction(watermark_);
    toolbar->addAction(log_);
    toolbar->addAction(test);
    toolbar->addAction(refresh_);
}

void MainWindow::SetupLayout() {
    QWidget* central = new QWidget(this);
    setCentralWidget(central);

    QHBoxLayout* layout = new QHBoxLayout(central);
    layout->addWidget(device_view_, 1);
    layout->addWidget(image_view_, 3);
    layout->addWidget(property_view_, 1);
    central->setLayout(layout);

}

void MainWindow::ConnctSlot() {

}

void MainWindow::PopulateData() {
    // !! 现象: 子窗口显示的图像非常小
    // !! 原因: 显示 label 还没有经过布局计算实际的尺寸，所以返回的尺寸是一个极小值，显示的图像极小
    // !! 优化: 在父窗口resize, 当前窗口show后，label才获得实际尺寸，使用刷新按钮重新显示图像就是正常的
    // !! 根源优化: 自定义 QLabel 覆写 QLabel 的 resizeEvent 事件，尺寸更新后重新计算尺寸更新 Pixmap
    // image_view_->Display(0, QPixmap(":/png/t-17.jpg"));
}

void MainWindow::OnOpenFile() {
    QString title = "Open File";
    QString filter = "";

    QString file_name = QFileDialog::getOpenFileName(this, title, open_path_, filter);

    QFileInfo file(file_name);
    if (file.exists()) {
        open_path_ = file.path();
        file_ = file.filePath();

        img_ctl_->Open(file_);

        recent_file_.removeAll(file_); // 移除所有相同项（通常只有0或1个）
        recent_file_.append(file_);

        while (recent_file_.size() > 5) {
            recent_file_.removeFirst();
        }
        UpdateOpenRecentFiles();
    }
}

void MainWindow::UpdateOpenRecentFiles() {
    recent_file_menu_->clear();
    if (recent_file_.isEmpty()) {
        QAction* empty_act = recent_file_menu_->addAction(tr("No Recent Files"));
        empty_act->setEnabled(false);
    } else {
        for (const QString& file : std::as_const(recent_file_)) {
            QAction* act = recent_file_menu_->addAction(file);
            act->setData(file);
            connect(act, &QAction::triggered, this, [this, file]() {
                OnOpenRecentFile(file);
            });
        }
    }
}

void MainWindow::OnRefresh() {
    for (int i = 0; i < image_view_->GetImgViewCount(); i++) {
        // image_view_->Display(i, QPixmap(":/png/t-17.jpg"));
    }
}

void MainWindow::OnOpenRecentFile(const QString& file) {
    qDebug() << "--- OnOpenRecentFile: " << file << " ----";
}

void MainWindow::OnSaveFile() {
    qDebug() << "--- OnSaveFile ---";
}

void MainWindow::OnSaveAsFile() {
    qDebug() << "--- OnSaveAsFile ---";
}

void MainWindow::OnExitFile() {
    qDebug() << "--- OnExitFile ---";

    QMainWindow::close();
}

void MainWindow::OnSetFps(int fps) {
    qDebug() << "--- OnSetFps:" << fps << " ---";
}

void MainWindow::OnRenderD3d() {
    qDebug() << "--- OnRenderD3d ----";
}

void MainWindow::OnRenderGDI() {
    qDebug() << "--- OnRenderGDI ----";
}

void MainWindow::OnGeneral() {
    qDebug() << "--- OnGeneral ----";
}

void MainWindow::OnCapture() {
    qDebug() << "--- OnCapture ----";
}

void MainWindow::OnCache() {
    qDebug() << "--- OnCache ----";
}

void MainWindow::OnResendPacket() {
    qDebug() << "--- OnResendPacket ----";
}

void MainWindow::OnShortcut() {
    qDebug() << "--- OnShortcut ----";
}

void MainWindow::OnIpConfig() {
    qDebug() << "--- OnIpConfig ----";
}

void MainWindow::OnFirmwareUpgrade() {
    qDebug() << "--- OnFirmwareUpgrade ----";
}

void MainWindow::OnImportExport() {
    qDebug() << "--- OnImportExport ----";
}

void MainWindow::OnLogView() {
    qDebug() << "--- OnLogView ----";
}

void MainWindow::OnVirtualCamera() {
    qDebug() << "--- OnVirtualCamera ----";
}

void MainWindow::OnBandwidthManagement() {
    qDebug() << "--- OnBandwidthManagement ----";
}

void MainWindow::OnGigeActionCommand() {
    qDebug() << "--- OnGigeActionCommand ----";
}

void MainWindow::OnExportAllDevices() {
    qDebug() << "--- OnExportAllDevices ----";
}

void MainWindow::OnChinese() {
    qDebug() << "--- OnChinese ----";
}

void MainWindow::OnEnglish() {
    qDebug() << "--- OnEnglish ----";
}

void MainWindow::OnUserManual() {
    qDebug() << "--- OnUserManual ----";
}

void MainWindow::OnDevelopment() {
    qDebug() << "--- OnDevelopment ----";
}

void MainWindow::OnAbout() {
    qDebug() << "--- OnAbout ----";
}

void MainWindow::OnStatus() {
    qDebug() << "--- OnStatus ----";
}

void MainWindow::OnWatermark() {
    qDebug() << "--- OnWatermark ----";
}

void MainWindow::OnLog() {
    qDebug() << "--- OnLog ----";
}



