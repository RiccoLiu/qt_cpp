#ifndef CIRCLEBUTTON_H
#define CIRCLEBUTTON_H

#include <QWidget>
#include <QString>
#include <QPropertyAnimation>

/**
 * 1.添加过渡动画：Q_PROPERTY + QPropertyAnimation 对颜色做插值，实现 正常 ↔ 悬停 之间有 150ms 的平滑渐变，而非瞬间跳变 (已完成)。
 * 2.支持 QSS：重写 initStyleOption() + 在 paintEvent 中调用 style()->drawPrimitive()，让控件能被外部样式表控制颜色 (暂时先这样)。
 * 3.支持键盘操作：重写 keyPressEvent，响应 Space/Enter 触发点击，Tab 切换焦点时显示聚焦环 (已完成)。
 * 4.自适应尺寸：重写 sizeHint() 根据文字长度自动计算最小尺寸，替代 setFixedSize (已完成)。
 */

class CircleButton : public QWidget
{
    Q_OBJECT

    Q_PROPERTY(QString text READ text WRITE setText NOTIFY textChanged)
    Q_PROPERTY(qreal hoverFactor READ hoverFactor WRITE setHoverFactor)

public:
    explicit CircleButton(const QString& text = "", QWidget *parent = nullptr);

    QString text() const { return m_text; }
    void setText(const QString& text);

    qreal hoverFactor() const { return m_hoverFactor; }
    void setHoverFactor(qreal factor); // setter 中触发 update()

signals:
    void clicked();
    void textChanged(const QString &text);

protected:
    void paintEvent(QPaintEvent *event) override;
    void enterEvent(QEnterEvent *event) override;
    void leaveEvent(QEvent *event) override;

    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
    void focusInEvent(QFocusEvent *event) override;
    void focusOutEvent(QFocusEvent *event) override;

    void changeEvent(QEvent *) override;

    QSize sizeHint() const override;

private:
    // QColor bgColorFromState(State state);
    // QColor textColorFromState(State state);

    QColor interpolateColor(const QColor &from, const QColor &to, qreal factor) const;

private:
    int m_padding; // 文字到圆边的最小间距
    int m_fontSize; // 文字大小

    QString m_text;

    bool    m_isPressed;
    qreal   m_hoverFactor;

    QPropertyAnimation *m_hoverAnim; // 通过 hoverFactor 属性设置渐变动画

private:
    QColor m_normalColor  = QColor(0, 0, 255);
    QColor m_hoverColor   = QColor(255, 0, 0);
    QColor m_pressedColor = QColor(40, 90, 180);
    QColor m_textColor    = Qt::white;
};

#endif // CIRCLEBUTTON_H
