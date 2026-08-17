#ifndef TIMELINE_WIDGET_H
#define TIMELINE_WIDGET_H

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsRectItem>
#include <QWheelEvent>
#include <QMouseEvent>
#include <QScrollBar>
#include <vector>

// 时间轴上的单个片段(Clip)
class ClipItem : public QGraphicsRectItem {
public:
    enum { Type = UserType + 1 };

    ClipItem(qreal x, qreal y, qreal w, qreal h, const QString &name, QGraphicsItem *parent = nullptr);

    int type() const override { return Type; }
    QString name() const { return m_name; }

protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

private:
    QString m_name;
};

// 时间轴主控件
class TimelineWidget : public QGraphicsView {
    Q_OBJECT
public:
    explicit TimelineWidget(QWidget *parent = nullptr);

    // 添加一个片段 (startTimeMs, durationMs)
    void addClip(qint64 startTimeMs, qint64 durationMs, const QString &name);
    void clearClips();

    // 获取当前选中的片段名称列表
    QStringList selectedClips() const;

signals:
    void clipSelectionChanged(const QStringList &names);
    void clipMoved(const QString &name, qint64 newStartTimeMs);

protected:
    void wheelEvent(QWheelEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void drawBackground(QPainter *painter, const QRectF &rect) override;

private slots:
    void onSelectionChanged();

private:
    void updateScale(qreal factor);
    qint64 pixelToMs(qreal x) const;
    qreal msToPixel(qint64 ms) const;

    static constexpr qreal PIXELS_PER_MS_BASE = 0.1; // 基础缩放比例
    static constexpr qreal MIN_SCALE = 0.001;
    static constexpr qreal MAX_SCALE = 100.0;
    static constexpr int TRACK_HEIGHT = 40;

    qreal m_scaleFactor = 1.0;
    bool m_isPanning = false;
    QPoint m_lastPanPos;
};

#endif // TIMELINE_WIDGET_H



