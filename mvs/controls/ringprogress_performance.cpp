#include "ringprogress_performance.h"

#include <QPainter>
#include <QPainterPath>
#include <QApplication>


/**
 *
 * !! 预渲染使用的位图要基于物理像素制作的位图
 *
 *  逻辑像素尺寸与物理像素尺寸的转换
 *      | 概念 | 说明 |
 *      | :--- | :--- |
 *      | 逻辑像素 | Qt 布局系统使用的单位，例如 `width() = 80` |
 *      | 物理像素 | 屏幕上实际的像素点数量 |
 *      | DPR | 物理像素 / 逻辑像素的比值 |
 *
 */

QPixmap* RingProgressPerformance::s_bgCache = nullptr;
QVector<QPixmap*>* RingProgressPerformance::s_fgCache = nullptr;
QVector<QPixmap*>* RingProgressPerformance::s_textCache = nullptr;
bool RingProgressPerformance::s_cacheInitialized = false;

RingProgressPerformance::RingProgressPerformance(QWidget *parent)
    : QWidget{parent}
    , m_progress(0.0)
    , m_ringWidth(8.0)
    , m_pulseOpacity(0.0)
{
    setFixedSize(80, 80);

    m_animation = new QPropertyAnimation(this, "progress", this);
    m_animation->setEasingCurve(QEasingCurve::OutCubic);

    // !! 动画结束后，强制将进度设为精确的目标值
    connect(m_animation, &QPropertyAnimation::finished, this, [this]() {
        qreal target = qBound(0.0, m_animation->endValue().toReal(), 1.0);
        if (m_progress != target) {
            m_progress = target;
            update();
        }

        if (qFuzzyCompare(target, 1.0)) {
            // triggerPulse();
        } else {
            // stopPulse();
        }
    });
}

void RingProgressPerformance::ensureCacheInitialized(int size, qreal ringWidth,
                                                     QColor bgColor, QColor startColor,
                                                     QColor endColor, QColor textColor) {
    if (s_cacheInitialized)
        return;

    // ⭐ 安全检查：确保 QApplication 已创建
    Q_ASSERT_X(qApp, "RingProgressPerformance",
               "ensureCacheInitialized() must be called after QApplication is created");

    qreal dpr = qApp->devicePixelRatio();
    int pxSize = qRound(size * dpr);

    // 1️⃣ 背景环
    s_bgCache = new QPixmap(pxSize, pxSize);  // ✅ 运行时分配
    s_bgCache->setDevicePixelRatio(dpr);
    s_bgCache->fill(Qt::transparent);
    {
        QPainter p(s_bgCache);
        p.setRenderHint(QPainter::Antialiasing);
        QRectF r = QRectF(0, 0, size, size).adjusted(
            ringWidth/2, ringWidth/2, -ringWidth/2, -ringWidth/2);
        p.setPen(QPen(bgColor, ringWidth, Qt::SolidLine, Qt::RoundCap));
        p.setBrush(Qt::NoBrush);
        p.drawEllipse(r);
    }

    // 2️. 渐变进度环
    s_fgCache = new QVector<QPixmap*>(101, nullptr);
    buildFgCache(size, ringWidth, startColor, endColor);

    // 3️. 文本缓存
    s_textCache = new QVector<QPixmap*>(101, nullptr);
    buildTextCache(size, ringWidth, textColor);

    s_cacheInitialized = true;
}


void RingProgressPerformance::buildFgCache(int size, qreal ringWidth,
                                QColor startColor, QColor endColor) {
    qreal dpr = qApp->devicePixelRatio();
    int pxSize = qRound(size * dpr);
    s_fgCache->resize(101);

    for (int pct = 0; pct <= 100; ++pct) {
        QPixmap* pixmap = new QPixmap(pxSize, pxSize);
        pixmap->setDevicePixelRatio(dpr);
        pixmap->fill(Qt::transparent);
        (*s_fgCache)[pct] = pixmap;

        if (pct == 0)
            continue; // 0% 无需绘制

        qreal progress = pct / 100.0;
        QPainter p(s_fgCache->at(pct));
        p.setRenderHint(QPainter::Antialiasing);

        QRectF ringRect = QRectF(0, 0, size, size).adjusted(
            ringWidth/2, ringWidth/2, -ringWidth/2, -ringWidth/2);
        QRectF outerRect = ringRect.adjusted(-ringWidth/2, -ringWidth/2,
                                             ringWidth/2,  ringWidth/2);
        QRectF innerRect = ringRect.adjusted( ringWidth/2,  ringWidth/2,
                                             -ringWidth/2, -ringWidth/2);

        QPainterPath clipPath;
        clipPath.addEllipse(outerRect);
        clipPath.addEllipse(innerRect);

        QPointF center(size/2.0, size/2.0);
        QConicalGradient gradient(center, -90);
        gradient.setColorAt(0.0, startColor);
        gradient.setColorAt(progress, endColor);
        if (progress < 1.0) {
            gradient.setColorAt(qMin(progress + 0.001, 1.0), Qt::transparent);
            gradient.setColorAt(1.0, Qt::transparent);
        }

        p.setClipPath(clipPath);
        p.setPen(Qt::NoPen);
        p.setBrush(gradient);
        p.drawRect(0, 0, size, size);
    }
}

void RingProgressPerformance::buildTextCache(int size, qreal ringWidth, QColor textColor) {
    qreal dpr = qApp->devicePixelRatio();
    int pxSize = qRound(size * dpr);
    s_textCache->resize(101);

    QFont font;
    font.setPixelSize(qRound((size - ringWidth*2) * 0.25));
    font.setBold(true);

    for (int pct = 0; pct <= 100; ++pct) {
        QPixmap* pixmap = new QPixmap(pxSize, pxSize);
        pixmap->setDevicePixelRatio(dpr);
        pixmap->fill(Qt::transparent);
        (*s_textCache)[pct] = pixmap;

        QPainter p(s_textCache->at(pct));
        p.setRenderHint(QPainter::Antialiasing);
        p.setPen(textColor);
        p.setFont(font);
        p.drawText(QRectF(0, 0, size, size), Qt::AlignCenter,
                   QString("%1%").arg(pct));
    }
}

void RingProgressPerformance::paintEvent(QPaintEvent *event) {
    // ⭐ 首次使用时懒初始化缓存
    ensureCacheInitialized(width(), m_ringWidth,
                           m_bgColor, m_startColor, m_endColor, m_textColor);

    qDebug() << "rect.().width(): " << rect().width() << ", width():" << width() << ", qApp->devicePixelRatio():" << qApp->devicePixelRatio();

    QPainter painter(this);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);

    // ⭐ 指针访问
    painter.drawPixmap(rect(), *s_bgCache);

    int fgIndex = qBound(0, qRound(m_progress * 100), 100);
    if (fgIndex > 0 && (*s_fgCache)[fgIndex]) {
        painter.drawPixmap(rect(), *(*s_fgCache)[fgIndex]);
    }

    if ((*s_textCache)[fgIndex]) {
        painter.drawPixmap(rect(), *(*s_textCache)[fgIndex]);
    }

    // 4️⃣ 脉冲光晕（仍需实时绘制，但仅 1.0 时触发，不影响常规性能）
    if (m_pulseOpacity > 0.001) {
        painter.save();
        QPointF center = rect().center();
        painter.translate(center);
        painter.scale(1.0 + m_pulseOpacity * 0.15,
                      1.0 + m_pulseOpacity * 0.15);
        painter.translate(-center);

        QRectF pulseRect = rect().adjusted(m_ringWidth/2, m_ringWidth/2,
                                           -m_ringWidth/2, -m_ringWidth/2);
        QColor pulseColor = m_endColor;
        pulseColor.setAlphaF(m_pulseOpacity);

        painter.setPen(QPen(pulseColor, m_ringWidth * 1.5,
                            Qt::SolidLine, Qt::RoundCap));
        painter.setBrush(Qt::NoBrush);
        painter.drawEllipse(pulseRect);
        painter.restore();
    }
}


void RingProgressPerformance::animateTo(qreal progress, int duration_ms) {
    if (m_animation->state() == QAbstractAnimation::Running) {
        m_animation->stop();
    }
    m_animation->setStartValue(m_progress);
    m_animation->setEndValue(progress);
    m_animation->setDuration(duration_ms);
    m_animation->start();
}

void RingProgressPerformance::setProgress(qreal progress) {
    progress = qBound(0.0, progress, 1.0);

    if (!qFuzzyCompare(m_progress, progress)) {
        m_progress = progress;
        update();
    }
}

