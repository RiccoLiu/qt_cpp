
#include <QThread>
#include <QFileDialog>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "utils.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , managementWindow(new ManagementWindow)
    , filePath(QDir::homePath())
{
    ui->setupUi(this);

    decoder = new VideoDecoder();
    detector = new FaceDetector();

    decoderThread = new QThread(this);
    detectorThread = new QThread(this);

    decoder->moveToThread(decoderThread);
    detector->moveToThread(detectorThread);

    connect(decoder, &VideoDecoder::frameDecoded, detector, &FaceDetector::detect);
    connect(detector, &FaceDetector::detectionResult, this, &MainWindow::onDetectionResult);
    connect(decoder, &VideoDecoder::finished, this, &MainWindow::onPipelineFinished);

    decoderThread->start();
    detectorThread->start();

    // 显示控件
    displayer = new QLabel(this);
    setCentralWidget(displayer);

    displayer->setScaledContents(true);             // 自动缩放到控件大小
    displayer->setAlignment(Qt::AlignCenter);       // 居中显示
    displayer->setBackgroundRole(QPalette::Dark);   // 可选：深色背景
}

MainWindow::~MainWindow()
{
    QMetaObject::invokeMethod(decoder, "stop");

    decoderThread->quit();
    detectorThread->quit();
    decoderThread->wait();
    detectorThread->wait();

    delete decoder;
    delete detector;

    delete ui;
}

void MainWindow::on_manerge_triggered()
{
    managementWindow->show();
}

void MainWindow::onDetectionResult(const cv::Mat& result) {
    QImage img = Utils::cvMatToQImage(result);
    if (!img.isNull()) {
        displayer->setPixmap(QPixmap::fromImage(img));
    }
}

void MainWindow::onPipelineFinished() {
    qDebug() << "Pipeline finished.";
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
        QMetaObject::invokeMethod(decoder, "startDecoding", Qt::QueuedConnection, Q_ARG(QString, file));

        filePath = fileInfo.path();
    }
}
