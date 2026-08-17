
#include "ringprogress.h"

#include <QPainter>
#include <QPainterPath>

/**
 * 1. 由于动画插值 和 qFuzzyCompare 精度比较的问题，导致进度条卡在 99% 的问题；
 *      使用信号槽 动画结束后，强制设置进度为精确的目标值
 *
 * 2. drawArc 画弧线，不能使用色刷； 坐标系: 3点钟 起始位置，顺时针方向为负
 *    QConicalGradient: 渐变色色刷; 坐标系: 3点阵 起始位置，顺时针方向为正
 *
 * 3. QPainter.scale 是以坐标原点为中心向外进行缩放，脉冲动画 需要先将原点移动到中心，缩放后，再移动会原来的位置
 *      painter.translate(center);
 *      qreal scale = 1.0 + m_pulseOpacity * 0.15;
 *      painter.scale(scale, scale);
 *      painter.translate(-center);
 */

RingProgress::RingProgress(QWidget* parent)
    : QWidget(parent)
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
            triggerPulse();
        } else {
            stopPulse();
        }
    });

    // 脉冲动画
    m_pulseAnim = new QParallelAnimationGroup(this);
    auto* opacityAnim = new QPropertyAnimation(this, "pulseOpacity", this);
    opacityAnim->setDuration(600);

    opacityAnim->setKeyValues({
        {0.0, 0.0}, // 时间百分比: 0 -> 0.0  ->
        {0.4, 0.6}, // 时间百分比: 0.4 -> 0.6
        {1.0, 0.0}  // 时间百分比: 1 -> 0.0
    });
    opacityAnim->setEasingCurve(QEasingCurve::InOutQuad);

    m_pulseAnim->addAnimation(opacityAnim);
}

void RingProgress::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    if (m_pulseOpacity > 0.001) {
        painter.save();

        QPointF center = rect().center();

        // !! 由于Scale 是以原点为中心向外缩放，所以要先把原点移动到中心缩放后，再移会原来的位置
        painter.translate(center);
        qreal scale = 1.0 + m_pulseOpacity * 0.15;
        painter.scale(scale, scale);
        painter.translate(-center);

        QRectF pulseRect = rect().adjusted(m_ringWidth / 2, m_ringWidth / 2, -m_ringWidth / 2, -m_ringWidth / 2);
        QColor pulseColor = m_endColor;

        // 脉冲环越大，越透明(扩散效果)
        pulseColor.setAlphaF(m_pulseOpacity);

        QPen pulsePen(pulseColor, m_ringWidth / 2, Qt::SolidLine, Qt::RoundCap);
        painter.setPen(pulsePen);
        painter.setBrush(Qt::NoBrush);
        painter.drawEllipse(pulseRect);

        painter.restore();
    }

    QRectF ringRect = rect().adjusted(m_ringWidth, m_ringWidth, -m_ringWidth, -m_ringWidth);

    QPen bgPen(m_bgColor, m_ringWidth, Qt::SolidLine, Qt::RoundCap);
    painter.setPen(bgPen);
    painter.setBrush(Qt::NoBrush);
    painter.drawEllipse(ringRect);

    if (m_progress > 0.001) {
#if 0
        QPen fgPen(m_fgColor, m_ringWidth, Qt::SolidLine, Qt::RoundCap);

        painter.setPen(fgPen);
        painter.setBrush(Qt::NoBrush);

        int start = 90 * 16;    // 起始点在 12 点钟方向
        int length = -m_progress * 360 * 16; // drawArc 顺时针方向为 负

        painter.drawArc(ringRect, start, length);
#else
        // !! 使用 setClipPath 在矩形内裁剪一个镂空圆环，超过进度条的部分设置透明色
        QRectF outerRect = ringRect.adjusted(-m_ringWidth / 2, -m_ringWidth / 2, m_ringWidth / 2, m_ringWidth / 2);
        QRectF innerRect = ringRect.adjusted(-m_ringWidth / 2 + m_ringWidth, -m_ringWidth / 2 + m_ringWidth, -m_ringWidth / 2, -m_ringWidth / 2);

        QPainterPath clipPath;
        clipPath.addEllipse(outerRect);
        clipPath.addEllipse(innerRect);

        QPointF center = rect().center();
        QConicalGradient gradient(center, -90); // 起始点在 12 点钟方向

        gradient.setColorAt(0.0, m_startColor);
        gradient.setColorAt(m_progress, m_endColor);

        // 超过进度条的部分设为透明
        if (m_progress < 1.0) {
            gradient.setColorAt(qMin(m_progress + 0.001, 1.0), Qt::transparent);
            gradient.setColorAt(1.0, Qt::transparent);
        }

        painter.save();
        painter.setClipPath(clipPath);
        painter.setPen(Qt::NoPen);
        painter.setBrush(gradient);
        painter.drawRect(rect());
        painter.restore();
#endif
    }

    painter.setPen(m_textColor);
    painter.setBrush(Qt::NoBrush);

    QFont font = this->font();
    font.setPixelSize(ringRect.height() * 0.25);
    font.setBold(true);
    painter.setFont(font);

    // QString text = QString("%1%").arg(static_cast<int>(m_progress * 100));
    QString text = QString("%1%").arg(qRound(m_progress * 100));
    painter.drawText(ringRect, Qt::AlignCenter, text);
}

void RingProgress::animateTo(qreal progress, int durationMs) {
    stopPulse();

    if (m_animation->state() == QAbstractAnimation::Running) {
        m_animation->stop();
    }
    m_animation->setStartValue(m_progress);
    m_animation->setEndValue(progress);
    m_animation->setDuration(durationMs);
    m_animation->start();
}

void RingProgress::setProgress(qreal progress) {
    progress = qBound(0.0, progress, 1.0);

    if (!qFuzzyCompare(progress, m_progress)) {
        m_progress = progress;
        update();
    }
}

void RingProgress::setPulseOpacity(qreal val) {
    if (!qFuzzyCompare(val, m_pulseOpacity)) {
        m_pulseOpacity = val;
        update();
    }
}

void RingProgress::triggerPulse() {
    if (m_pulseAnim->state() == QAbstractAnimation::Running)
        return;
    m_pulseAnim->start();
}

void RingProgress::stopPulse() {
    if (m_pulseAnim->state() == QAbstractAnimation::Running) {
        m_pulseAnim->stop();
    }
    if (m_pulseOpacity != 0.0) {
        m_pulseOpacity = 0.0;
        update();
    }
}
