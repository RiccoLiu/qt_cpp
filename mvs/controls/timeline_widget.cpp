#include "timeline_widget.h"
#include <QPainter>
#include <QGraphicsSceneMouseEvent>
#include <cmath>
#include <algorithm>
#include <QStyleOptionGraphicsItem>

// ==================== ClipItem 实现 ====================
ClipItem::ClipItem(qreal x, qreal y, qreal w, qreal h, const QString &name, QGraphicsItem *parent)
    : QGraphicsRectItem(x, y, w, h, parent), m_name(name)
{
    setFlags(ItemIsSelectable | ItemIsMovable | ItemSendsGeometryChanges);
    setBrush(QColor(70, 130, 180));
    setPen(QPen(QColor(50, 100, 150), 1));
    setZValue(1);
}

void ClipItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(widget);

    // 选中时改变颜色
    if (option->state & QStyle::State_Selected) {
        setBrush(QColor(100, 180, 255));
        setPen(QPen(QColor(255, 200, 50), 2));
    } else {
        setBrush(QColor(70, 130, 180));
        setPen(QPen(QColor(50, 100, 150), 1));
    }

    QGraphicsRectItem::paint(painter, option, widget);

    // 绘制文字（自动裁剪）
    painter->setPen(Qt::white);
    QFont font = painter->font();
    font.setPixelSize(12);
    painter->setFont(font);
    QRectF textRect = rect().adjusted(4, 0, -4, 0);
    painter->drawText(textRect, Qt::AlignVCenter | Qt::AlignLeft | Qt::TextSingleLine, m_name);
}

// ==================== TimelineWidget 实现 ====================
TimelineWidget::TimelineWidget(QWidget *parent)
    : QGraphicsView(parent)
{
    auto *scene = new QGraphicsScene(this);
    scene->setSceneRect(0, 0, 1000000, TRACK_HEIGHT + 100);
    setScene(scene);

    // 关键配置
    setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
    setViewportUpdateMode(QGraphicsView::SmartViewportUpdate);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse); // 缩放以鼠标为中心
    setResizeAnchor(QGraphicsView::AnchorUnderMouse);
    setDragMode(QGraphicsView::NoDrag); // 我们自己处理拖拽
    // setSelectionMode(QGraphicsView::ExtendedSelection); // 支持Ctrl/Shift多选

    // 背景色
    setBackgroundBrush(QColor(30, 30, 30));

    connect(scene, &QGraphicsScene::selectionChanged, this, &TimelineWidget::onSelectionChanged);
}

void TimelineWidget::addClip(qint64 startTimeMs, qint64 durationMs, const QString &name)
{
    qreal x = msToPixel(startTimeMs);
    qreal w = msToPixel(durationMs) - msToPixel(0);
    auto *clip = new ClipItem(x, 10, std::max(w, 2.0), TRACK_HEIGHT, name);
    scene()->addItem(clip);
}

void TimelineWidget::clearClips()
{
    for (auto *item : scene()->items()) {
        if (item->type() == ClipItem::Type) {
            delete item;
        }
    }
}

QStringList TimelineWidget::selectedClips() const
{
    QStringList names;
    for (auto *item : scene()->selectedItems()) {
        if (auto *clip = dynamic_cast<ClipItem*>(item)) {
            names.append(clip->name());
        }
    }
    return names;
}

// ---- 缩放 ----
void TimelineWidget::wheelEvent(QWheelEvent *event)
{
    if (event->modifiers() & Qt::ControlModifier) {
        qreal factor = (event->angleDelta().y() > 0) ? 1.15 : 1.0 / 1.15;
        updateScale(factor);
        event->accept();
    } else {
        // 无Ctrl时滚动
        QGraphicsView::wheelEvent(event);
    }
}

void TimelineWidget::updateScale(qreal factor)
{
    qreal newScale = m_scaleFactor * factor;
    newScale = std::clamp(newScale, MIN_SCALE, MAX_SCALE);

    if (newScale != m_scaleFactor) {
        qreal actualFactor = newScale / m_scaleFactor;
        scale(actualFactor, 1.0); // 只水平缩放
        m_scaleFactor = newScale;
        viewport()->update();
    }
}

// ---- 拖拽画布 (中键或空格+左键) ----
void TimelineWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::MiddleButton ||
        (event->button() == Qt::LeftButton && event->modifiers() & Qt::AltModifier)) {
        m_isPanning = true;
        m_lastPanPos = event->pos();
        setCursor(Qt::ClosedHandCursor);
        event->accept();
        return;
    }
    QGraphicsView::mousePressEvent(event);
}

void TimelineWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (m_isPanning) {
        QPoint delta = event->pos() - m_lastPanPos;
        horizontalScrollBar()->setValue(horizontalScrollBar()->value() - delta.x());
        m_lastPanPos = event->pos();
        event->accept();
        return;
    }
    QGraphicsView::mouseMoveEvent(event);
}

void TimelineWidget::mouseReleaseEvent(QMouseEvent *event)
{
    if (m_isPanning) {
        m_isPanning = false;
        setCursor(Qt::ArrowCursor);
        event->accept();
        return;
    }
    QGraphicsView::mouseReleaseEvent(event);
}

// ---- 自绘时间刻度背景 ----
void TimelineWidget::drawBackground(QPainter *painter, const QRectF &rect)
{
    QGraphicsView::drawBackground(painter, rect);

    painter->setPen(QColor(80, 80, 80));
    QFont font = painter->font();
    font.setPixelSize(10);
    painter->setFont(font);

    // 自适应刻度间距：根据当前缩放级别选择合适的间隔
    const qint64 intervals[] = {100, 500, 1000, 5000, 10000, 30000, 60000, 300000, 600000, 1800000, 3600000};
    qint64 bestInterval = intervals[0];
    qreal pixelsPerMs = PIXELS_PER_MS_BASE * m_scaleFactor;

    for (qint64 iv : intervals) {
        if (iv * pixelsPerMs >= 60) { // 刻度最小像素间距
            bestInterval = iv;
            break;
        }
    }

    qint64 startMs = pixelToMs(rect.left());
    qint64 endMs = pixelToMs(rect.right());
    startMs = (startMs / bestInterval) * bestInterval;

    for (qint64 ms = startMs; ms <= endMs; ms += bestInterval) {
        qreal x = msToPixel(ms);

        // 主刻度线
        painter->drawLine(QPointF(x, 0), QPointF(x, TRACK_HEIGHT + 10));

        // 时间标签
        QString label;
        if (bestInterval >= 3600000) {
            label = QString("%1h").arg(ms / 3600000);
        } else if (bestInterval >= 60000) {
            label = QString("%1:%2").arg(ms / 60000).arg((ms % 60000) / 1000, 2, 10, QChar('0'));
        } else {
            label = QString("%1.%2").arg(ms / 1000).arg((ms % 1000) / 100, 2, 10, QChar('0'));
        }
        painter->setPen(QColor(180, 180, 180));
        painter->drawText(QPointF(x + 3, 12), label);
        painter->setPen(QColor(80, 80, 80));
    }
}

// ---- 工具方法 ----
qint64 TimelineWidget::pixelToMs(qreal x) const
{
    return static_cast<qint64>(x / (PIXELS_PER_MS_BASE * m_scaleFactor));
}

qreal TimelineWidget::msToPixel(qint64 ms) const
{
    return ms * PIXELS_PER_MS_BASE * m_scaleFactor;
}

void TimelineWidget::onSelectionChanged()
{
    emit clipSelectionChanged(selectedClips());
}
