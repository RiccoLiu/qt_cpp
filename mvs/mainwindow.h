#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <QTreeView>
#include <QLabel>
#include <QStandardItemModel>
#include <QTabWidget>
#include <QMutex>
#include <QWaitCondition>
#include <QMutexLocker>
#include <QMenu>
#include <QTextEdit>
#include <QTreeWidgetItem>

#include "device_view.h"
#include "image_view.h"
#include "property_view.h"
#include "image_control.h"
#include "controls/scannerview.h"
#include "controls/mygl.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    // File
    void OnOpenFile();
    void OnOpenRecentFile(const QString& file);
    void OnSaveFile();
    void OnSaveAsFile();
    void OnExitFile();

    // View
    void OnSetFps(int fps);
    void OnRenderD3d();
    void OnRenderGDI();

    // Setup
    void OnGeneral();
    void OnCapture();
    void OnCache();
    void OnResendPacket();
    void OnShortcut();

    // Tool
    void OnIpConfig();
    void OnFirmwareUpgrade();
    void OnImportExport();
    void OnLogView();
    void OnVirtualCamera();
    void OnBandwidthManagement();
    void OnGigeActionCommand();
    void OnExportAllDevices();

    // Help
    void OnChinese();
    void OnEnglish();
    void OnUserManual();
    void OnDevelopment();
    void OnAbout();

    // Toolbar
    void OnStatus();
    void OnWatermark();
    void OnLog();
    void OnRefresh();

private:
    void CreateActions();
    void CreateMenus();
    void CreateToolBars();
    void SetupLayout();
    void ConnctSlot();
    void PopulateData();

    void UpdateOpenRecentFiles();

private:
    // File
    QAction* open_act_;
    QMenu* recent_file_menu_ {nullptr};
    QAction* save_act_;
    QAction* save_as_act_;
    QAction* exit_act_;

    // View
    QMenu* filter_method_;
    QMenu* vsync_method_;

    // Toolbar
    QToolButton* screen_layout_;
    QAction* status_;
    QAction* watermark_;
    QAction* log_;
    QAction* refresh_;

    // Widget View
    DeviceView* device_view_;
    ImageView* image_view_;
    PropertyView* property_view_;

    // OpenGL
    QPointer<MyGLWidget> gl_;

private:
    QString open_path_;
    QString file_;

    QMutex mutex_;
    QWaitCondition cond_;
    QStringList recent_file_;

    ImageControl* img_ctl_;

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
