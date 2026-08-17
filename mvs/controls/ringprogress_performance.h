#ifndef RINGPROGRESS_PERFORMANCE_H
#define RINGPROGRESS_PERFORMANCE_H

#include <QWidget>
#include <QPropertyAnimation>

class RingProgressPerformance : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(qreal progress READ progress WRITE setProgress)

public:
    explicit RingProgressPerformance(QWidget *parent = nullptr);

    qreal progress() { return m_progress; }
    void setProgress(qreal progress);

    void animateTo(qreal progress, int duration_ms);

signals:

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    static QPixmap* s_bgCache;
    static QVector<QPixmap*>* s_fgCache;   // 注意：这里也建议用指针容器
    static QVector<QPixmap*>* s_textCache;

    static bool s_cacheInitialized;

    static void ensureCacheInitialized(int size, qreal ringWidth,
                                       QColor bgColor, QColor startColor,
                                       QColor endColor, QColor textColor);
    static void buildFgCache(int size, qreal ringWidth,
                             QColor startColor, QColor endColor);
    static void buildTextCache(int size, qreal ringWidth, QColor textColor);

private:
    qreal m_progress;       // 当前进度 [0.0, 1.0]
    qreal m_ringWidth;     // 环宽
    QPropertyAnimation *m_animation = nullptr;

    qreal m_pulseOpacity;      // 脉冲光晕透明度 (0.0 ~ 1.0)
    // QParallelAnimationGroup* m_pulseAnim;

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

#endif // RINGPROGRESS_PERFORMANCE_H
