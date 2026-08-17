#ifndef SCANNERWINDOW_H
#define SCANNERWINDOW_H

#include <QWidget>
#include <QStatusBar>
#include <QTimer>
#include "controls/scannerview.h"
#include "image_control.h"

class ScannerWindow : public QWidget
{
    Q_OBJECT
public:
    explicit ScannerWindow(QWidget *parent = nullptr);

    void setupUI();

    void RegisteControl(ImageControl* control);

signals:

private slots:
    void onStartScan();
    void onStopScan();
    void onSave();

private:
    ScannerView* m_scanner;
    ImageControl* m_imageControl;

    QTimer* m_timer;

    QStatusBar* m_statusBar;
};

#endif // SCANNERWINDOW_H
