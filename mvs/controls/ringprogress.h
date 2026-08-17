#ifndef RINGPROCESS_H
#define RINGPROCESS_H

#include <QWidget>
#include <QColor>
#include <QPropertyAnimation>
#include <QParallelAnimationGroup>

class RingProgress : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(qreal progress READ progress WRITE setProgress)
    Q_PROPERTY(qreal pulseOpacity READ pulseOpacity WRITE setPulseOpacity)

public:
    RingProgress(QWidget* parent = nullptr);

    qreal progress() { return m_progress; }
    void setProgress(qreal progress);

    qreal pulseOpacity() const { return m_pulseOpacity; }
    void setPulseOpacity(qreal val);

    void animateTo(qreal progress, int duration_ms);
    void triggerPulse();   // 触发脉冲
    void stopPulse();      // 停止脉冲

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    qreal m_progress;       // 当前进度 [0.0, 1.0]
    qreal m_ringWidth;     // 环宽
    QPropertyAnimation *m_animation = nullptr;

    qreal m_pulseOpacity;      // 脉冲光晕透明度 (0.0 ~ 1.0)
    QParallelAnimationGroup* m_pulseAnim;

private:
    // QColor m_bgColor   = QColor(230, 230, 235);
    // QColor m_fgColor   = QColor(70, 130, 230);
    // QColor m_textColor = QColor(50, 50, 50);

    QColor m_bgColor = Qt::red;
    QColor m_fgColor = Qt::green;
    QColor m_startColor = Qt::blue;
    QColor m_endColor = Qt::green;
    QColor m_textColor = Qt::blue;
};
#endif // RINGPROCESS_H
