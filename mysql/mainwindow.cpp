
#include <QThread>
#include <QFileDialog>
#include <QMessageBox>

#include <logger2.h>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "process/process_factory.h"
#include "process/video_displayer.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , managementWindow(new ManagementWindow)
    , filePath(QDir::homePath())
{
    ui->setupUi(this);

    // 显示控件
    displayer = new QLabel(this);
    setCentralWidget(displayer);

    displayer->setScaledContents(true);             // 自动缩放到控件大小
    displayer->setAlignment(Qt::AlignCenter);       // 居中显示
    displayer->setBackgroundRole(QPalette::Dark);   // 可选：深色背景

    pipeline = std::make_shared<Pipeline>(std::make_shared<ProcessFactory>());
    pipeline->LoadYAML("pipeline.yaml");
    decoder = std::dynamic_pointer_cast<VideoDecoder>(pipeline->GetNode(VideoDecoder::NodeName()));

    auto video_disp = std::dynamic_pointer_cast<VideoDisplayer>(pipeline->GetNode(VideoDisplayer::NodeName()));
    connect(video_disp.get(), &VideoDisplayer::Display, this, &MainWindow::DisplayImg, Qt::QueuedConnection);

    pipeline->Start();
}

MainWindow::~MainWindow()
{
    pipeline->Stop();
    delete ui;
}

void MainWindow::DisplayImg(const QImage& img) {
    displayer->setPixmap(QPixmap::fromImage(img));
}

void MainWindow::on_manerge_triggered()
{
    managementWindow->show();
}

void MainWindow::on_open_triggered()
{
    QString title =  "Open Video File";
    QString filter = "Video Files (*.mp4 *.avi *.mov *.mkv);;All Files (*)";

    QString file = QFileDialog::getOpenFileName(this, title, filePath, filter);
    if (file.isEmpty())
        return;

    QFileInfo fileInfo(file);
    if (fileInfo.exists() && fileInfo.isFile()) {
        filePath = fileInfo.path();

        if (decoder->Open(file.toStdString()) ) {
            QMessageBox::information(this, "提示", QString("加载 %1 成功").arg(file));
        } else {
            QMessageBox::warning(this, "警告", QString("加载 %1 失败").arg(file));
        }
    }
}

void MainWindow::on_close_triggered() {
    decoder->Close();
}

void MainWindow::on_play_triggered() {
    if (decoder->IsPause()) {
        decoder->SetPause(false);
    } else {
        decoder->SetPause(true);
    }
}
