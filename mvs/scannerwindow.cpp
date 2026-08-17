#include <QMenu>
#include <QMenuBar>
#include <QAction>
#include <QToolBar>
#include <QVBoxLayout>

#include "scannerwindow.h"

ScannerWindow::ScannerWindow(QWidget *parent)
    : QWidget{parent}
    , m_scanner(new ScannerView(this))
    , m_timer(new QTimer(this))
{
    resize(800, 800);
    setWindowTitle("DFScan");

    setupUI();
}

void ScannerWindow::setupUI() {
    // 0. 行动项
    QAction* save = new QAction(tr("Save"), this); // 菜单栏和工具栏共有的行动项

    // 1. 编写菜单栏
    QMenuBar* menubar = new QMenuBar(this);

    QMenu* file = menubar->addMenu(tr("File"));
    QAction* open = file->addAction(tr("Open"));
    QMenu* openRecent = file->addMenu("Open Recent");
    QAction* recentFile = openRecent->addAction(tr("/home/face.png"));
    QAction* close = file->addAction(tr("Close"));
    file->addAction(save);

    QMenu* view = menubar->addMenu(tr("View"));
    QMenu* dispMethod = view->addMenu(tr("Display Method"));
    dispMethod->setEnabled(false);

    // 2. 编写 ToolBar
    QToolBar* toolbar = new QToolBar(tr("Scanner"), this);

    QAction* startScan = toolbar->addAction(tr("Start Scan"));
    QAction* stopScan = toolbar->addAction(tr("Stop Scan"));
    toolbar->addAction(save);

    connect(startScan, &QAction::triggered, this, &ScannerWindow::onStartScan);
    connect(stopScan, &QAction::triggered, this, &ScannerWindow::onStopScan);
    connect(save, &QAction::triggered, this, &ScannerWindow::onSave);

    // 3. 状态栏
    m_statusBar = new QStatusBar(this);
    m_statusBar->showMessage(tr("Ready To Scan"));

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0); // 移除边距
    layout->setSpacing(0);                  // 移除间距
    layout->addWidget(menubar);
    layout->addWidget(toolbar);
    layout->addWidget(m_scanner);
    layout->addWidget(m_statusBar);
    setLayout(layout);
}

void ScannerWindow::onStartScan() {
    if (m_imageControl) {
        m_imageControl->SimulateScanLineStart();
        m_statusBar->showMessage(tr("Ready To Scan"));
    }
}

void ScannerWindow::onStopScan() {
    if (m_imageControl) {
        m_imageControl->SimulateScanLineStop();
        m_statusBar->showMessage(tr("Scan Finished"));
    }
}

void ScannerWindow::onSave() {

}

void ScannerWindow::RegisteControl(ImageControl* control) {
    m_imageControl = control;

    connect(m_imageControl, &ImageControl::ScanImageReady, m_scanner, &ScannerView::onScanLine);
    connect(m_imageControl, &ImageControl::ScanImageFinished, m_scanner, &ScannerView::onScanFinished);
}


