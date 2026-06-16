#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <pipeline.h>

#include "usermanager_window.h"
#include "management_window.h"
#include "process/video_decoder.h"


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
    void DisplayImg(const QImage& img);

    void on_manerge_triggered();
    void on_open_triggered();
    void on_close_triggered();
    void on_play_triggered();

private:
    Ui::MainWindow *ui;

    QLabel* displayer;
    QString filePath;

    ManagementWindow* managementWindow;

    std::shared_ptr<Pipeline> pipeline;
    std::shared_ptr<VideoDecoder> decoder;
};
#endif // MAINWINDOW_H
