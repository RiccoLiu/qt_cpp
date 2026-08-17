#include "scannerview.h"

ScannerView::ScannerView(QWidget* parent)
    : QGraphicsView(parent)
    , m_scanColumn(0)
    , m_scanMaxWidth(10000)
    , m_scanImage(nullptr)
    , m_scanLine(nullptr)
{
    setRenderHint(QPainter::SmoothPixmapTransform);
    setBackgroundBrush(Qt::black);
    setFrameShape(QFrame::NoFrame);

    m_scene = new QGraphicsScene(this);
    setScene(m_scene);

    m_scanImageItem = new ScanImageItem();
    m_scene->addItem(m_scanImageItem);

    m_scanLine = new QGraphicsRectItem();
    m_scene->addItem(m_scanLine);

    m_scanLine->setPen(Qt::NoPen);
    m_scanLine->setBrush(QColor(0, 255, 0));
    m_scanLine->setVisible(false);
}

void ScannerView::onScanLine(QImage scanImage) {
    if (scanImage.isNull() || scanImage.width() <= 0 || scanImage.height() <= 0) {
        return;
    }

    if (!m_scanImage) {
        m_scanImage = new QImage(m_scanMaxWidth, scanImage.height(), scanImage.format());
        m_scanImage->fill(Qt::white);

        m_scanLine->setRect(0, 0, 3, scanImage.height());
        m_scanLine->setPos(0, 0);
        m_scanLine->setVisible(true);

        m_scanColumn = 0;
    }

    int copyWidth = scanImage.width();

    if (m_scanColumn + copyWidth > m_scanMaxWidth) {
        qDebug() << "scan image oversize max width...";
        return;
    }

    int bpp = scanImage.depth() / 8;

    for (int h = 0; h < scanImage.height(); h++) {
        uchar* source = scanImage.scanLine(h);
        uchar* target = m_scanImage->scanLine(h) + bpp * m_scanColumn;
        memcpy(target, source, copyWidth * bpp);
    }
    m_scanColumn += copyWidth;

    //
    // m_scanImageItem->setPixmap(QPixmap::fromImage(*m_scanImage));
    m_scanImageItem->setValidWidth(*m_scanImage, m_scanColumn);
    m_scanLine->setPos(m_scanColumn, 0);

    /*
     * ensureVisible：
     *      局部跟踪：平移视口使指定目标可见，不改变当前缩放比例
     *
     *      xMargin/yMargin: 目标边缘到视口边缘的最小像素
     */
    ensureVisible(m_scanLine, 50, 0);
}


void ScannerView::onScanFinished() {
    if (m_scanImage) {
        delete m_scanImage;
        m_scanImage = nullptr;
    }
    // 清空
    // m_pixmapItem->setPixmap(QPixmap());
}


void ScannerView::resizeEvent(QResizeEvent *event) {
    QGraphicsView::resizeEvent(event);

   /*
    * fitInView:
    *       全局适配： 将指定矩形区域缩放居中填满整个视图
    */
    if (m_scanImage && !m_scanImage->isNull() && m_scanColumn > 0) {
        fitInView(0, 0, m_scanColumn, m_scanImage->height(), Qt::KeepAspectRatio);
    }
}

