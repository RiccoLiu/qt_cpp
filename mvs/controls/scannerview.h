#ifndef SCANNERVIEW_H
#define SCANNERVIEW_H

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QGraphicsRectItem>

// 自定义图元：使用 ScanImageItem 替代 QGraphicsPixmapItem
class ScanImageItem : public QGraphicsItem {
public:
    explicit ScanImageItem(QGraphicsItem *parent = nullptr)
        :  QGraphicsItem(parent) {}

    QRectF boundingRect() const override {
        if (m_image.isNull())
            return QRectF();
        return QRectF(0, 0, m_image.width(), m_image.height());
    }

    void paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*) override {
        if (!m_image.isNull()) {
            // 只绘制当前有效区域，避免绘制80000宽的空白区域
            painter->drawImage(0, 0, m_image, 0, 0, m_validWidth, m_image.height());
        }
    }

    void setValidWidth(QImage image, int validWidth) {
        m_validWidth = validWidth;
        m_image = image;
        update();
    }

private:
    QImage m_image;
    int m_validWidth = 0;
};

class ScannerView : public QGraphicsView
{
public:
    explicit ScannerView(QWidget* parent);

public slots:
    void onScanLine(QImage oneLine);
    void onScanFinished();

protected:
    void resizeEvent(QResizeEvent *event) override;

private:
    QImage* m_scanImage;
    int m_scanColumn;
    int m_scanMaxWidth;

    QGraphicsScene* m_scene;
    // QGraphicsPixmapItem* m_pixmapItem;
    QGraphicsRectItem* m_scanLine;
    ScanImageItem* m_scanImageItem;
};

#endif // SCANNERVIEW_H
