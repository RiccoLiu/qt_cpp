#include <QPainter>
#include <qcoreevent.h>
#include <QMouseEvent>
#include "circlebutton.h"

CircleButton::CircleButton(const QString& text, QWidget *parent)
    : QWidget{parent}
    , m_padding(5)
    , m_fontSize(14)
    , m_text(text)
    , m_hoverFactor(0.0)
    , m_isPressed(false)
    , m_hoverAnim(nullptr)
{
    // setFixedSize(QSize(80, 80));                                    // 固定写死尺寸
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);          // 严格使用 sizeHint() 的尺寸
    // setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);  // 使用 sizeHint() 的尺寸，但是可以被拉伸
    // setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);  // 主动填满可用空间

    setMouseTracking(true);
    setFocusPolicy(Qt::StrongFocus); // 可以获得焦点： 可以点击 / 可以使用TAB切换到控件

    m_hoverAnim = new QPropertyAnimation(this, "hoverFactor", this);
    m_hoverAnim->setDuration(300);
    m_hoverAnim->setEasingCurve(QEasingCurve::InOutQuad); // 平滑加减速
}

void CircleButton::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    const QPalette pal = palette();
    QColor bgColor, textColor;

    if (!isEnabled()) {
        bgColor   = pal.color(QPalette::Disabled, QPalette::Button);
        textColor = pal.color(QPalette::Disabled, QPalette::ButtonText);
    } else if (m_isPressed) {
        // 按下状态
        bgColor = m_pressedColor ;
        textColor = pal.color(QPalette::HighlightedText);
    } else {
        // 正常状态 & 悬停状态 使用动画插值实现
        bgColor   = interpolateColor(m_normalColor, m_hoverColor, m_hoverFactor);
        textColor = pal.color(QPalette::ButtonText);
    }

    QRectF circleRect = rect().adjusted(4, 4, -4, -4);
    painter.setPen(Qt::NoPen);
    painter.setBrush(bgColor);
    painter.drawEllipse(circleRect);

    painter.setPen(textColor);
    QFont font = this->font();
    font.setPixelSize(m_fontSize);
    painter.setFont(font);
    painter.drawText(circleRect, Qt::AlignCenter, m_text);

    if (hasFocus() && isEnabled()) {
        painter.setPen(QPen(bgColor, 2.0));
        painter.setBrush(Qt::NoBrush);

        QRectF focusRect = rect().adjusted(1, 1, -1, -1);
        painter.drawEllipse(focusRect);
    }
}

void CircleButton::enterEvent(QEnterEvent *event) {
    QWidget::enterEvent(event);
    if (!m_isPressed) {
        m_hoverAnim->stop();

        // 渲染动画到悬停
        m_hoverAnim->setStartValue(m_hoverFactor);
        m_hoverAnim->setEndValue(1.0);
        m_hoverAnim->start();
    }

}

void CircleButton::leaveEvent(QEvent *event) {
    QWidget::leaveEvent(event);
    if (!m_isPressed) {
        m_hoverAnim->stop();

        // 渲染动画到正常状态
        m_hoverAnim->setStartValue(m_hoverFactor);
        m_hoverAnim->setEndValue(0.0);
        m_hoverAnim->start();
    }
}

void CircleButton::mousePressEvent(QMouseEvent *event) {
    QWidget::mousePressEvent(event);
    if (event->button() == Qt::LeftButton) {
        m_isPressed = true;

        // 按下, 动画停止
        m_hoverAnim->stop();
        update();
    }
}

void CircleButton::mouseReleaseEvent(QMouseEvent *event) {
    QWidget::mouseReleaseEvent(event);
    if (event->button() == Qt::LeftButton) {
        m_isPressed = false;
        if (rect().contains(event->pos())) {
            emit clicked();
            m_hoverAnim->stop();

            // 渲染动画到悬停
            m_hoverAnim->setStartValue(m_hoverFactor);
            m_hoverAnim->setEndValue(1.0);
            m_hoverAnim->start();
        } else {
            m_hoverAnim->stop();

            // 渲染动画到正常状态
            m_hoverAnim->setStartValue(m_hoverFactor);
            m_hoverAnim->setEndValue(0.0);
            m_hoverAnim->start();
        }
        update();
    }
}

/**
 * 1. Auto-Repeat 机制：
 *
 * 长按连续触发 keyPress/keyRelease 消息，有些场景会多次触发 click 信号， 当前的代码由于有 m_isPressed  守卫, 并不会多次触发 clicked
 *
 *  if (event->isAutoRepeat()) {
 *      event->accept();
 *      return; // 跳过 auto-repeat 产生的伪释放
 *  }
 *
 *  2. accept() + return 阻止传播： 自己控件自己处理
 *      QWidget::keyPressEvent(event) /  QWidget::keyReleaseEvent(event): 自己控件不处理，交给父控件处理
 *
 */
void CircleButton::keyPressEvent(QKeyEvent *event) {
    if (event->isAutoRepeat()) {
        event->accept();
        return; // 跳过 auto-repeat 产生的伪释放
    }

    if (isEnabled() && !m_isPressed &&
        (event->key() == Qt::Key_Space || event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter)) {

        m_isPressed = true;
        m_hoverAnim->stop();

        update();
        event->accept();
        return;
    }
    QWidget::keyPressEvent(event);
}

void CircleButton::keyReleaseEvent(QKeyEvent *event) {
    if (event->isAutoRepeat()) {
        event->accept();
        return; // 跳过 auto-repeat 产生的伪释放
    }

    if (isEnabled() && m_isPressed &&
        (event->key() == Qt::Key_Space || event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter)) {
        m_isPressed = false;
        emit clicked();

        m_hoverAnim->stop();

        m_hoverAnim->setStartValue(m_hoverFactor);
        m_hoverAnim->setEndValue(0.0);
        m_hoverAnim->start();

        update();
        event->accept();
        return;
    }
    QWidget::keyReleaseEvent(event);
}

void CircleButton::focusInEvent(QFocusEvent *event) {
    QWidget::focusInEvent(event);
    update(); // 触发重绘以显示聚焦环
}

void CircleButton::focusOutEvent(QFocusEvent *event) {
    QWidget::focusOutEvent(event);

    if (m_isPressed) {
        m_isPressed = false;
        m_hoverAnim->stop();
        m_hoverAnim->setStartValue(m_hoverFactor);
        m_hoverAnim->setEndValue(0.0);
        m_hoverAnim->start();
    }
    update(); // 触发重绘以隐藏聚焦环
}

QSize CircleButton::sizeHint() const
{
    // 1. 使用与 paintEvent 完全一致的字体参数
    QFont f = font();
    f.setPixelSize(m_fontSize);
    QFontMetrics fm(f);

    // 2. 计算文字包围盒（Qt6 用 horizontalAdvance，Qt5 用 width）
#if QT_VERSION >= QT_VERSION_CHECK(5, 11, 0)
    int textWidth = fm.horizontalAdvance(m_text);
#else
    int textWidth = fm.width(m_text);
#endif
    int textHeight = fm.height();

    // 3. 圆形按钮：取文字矩形的对角线作为最小圆直径
    //    对角线 = sqrt(w² + h²)，再加 padding 保证文字不贴边
    qreal diagonal = std::sqrt(textWidth * textWidth + textHeight * textHeight);
    int minDiameter = qCeil(diagonal) + m_padding * 2;

    // 4. 设置一个合理下限，避免空文字或短文字时按钮过小
    minDiameter = qMax(minDiameter, 40);

    return QSize(minDiameter, minDiameter);
}

void CircleButton::setText(const QString& text) {
    if (!text.isEmpty() && text != m_text) {
        m_text = text;
        emit textChanged(m_text);

        updateGeometry();   // 通知布局系统 sizeHint() 已变化
        update();
    }
}

// 颜色表变更，触发重绘
void CircleButton::changeEvent(QEvent *event) {
    if (event->type() == QEvent::PaletteChange ||
        event->type() == QEvent::EnabledChange) {
        update();
    }
}

void CircleButton::setHoverFactor(qreal factor)
{
    factor = qBound(0.0, factor, 1.0);              // factor 钳制截断
    if (!qFuzzyCompare(m_hoverFactor, factor)) {    // 安全的浮点相等判断
        m_hoverFactor = factor;
        update();
    }
}
// ✅ HSL 空间插值，避免 RGB 线性插值产生的“灰暗中间色”
QColor CircleButton::interpolateColor(const QColor &from, const QColor &to, qreal factor) const {
    // 在 HSL 色彩空间做插值，视觉过渡更自然
    float h1, s1, l1, h2, s2, l2;
    from.getHslF(&h1, &s1, &l1);
    to.getHslF(&h2, &s2, &l2);

    // 处理色相环跨越问题（如红→蓝应走短弧）
    if (qAbs(h2 - h1) > 0.5) {
        if (h1 > h2) h2 += 1.0; else h1 += 1.0;
    }

    qreal h = h1 + (h2 - h1) * factor;
    if (h > 1.0) h -= 1.0;

    qreal s = s1 + (s2 - s1) * factor;
    qreal l = l1 + (l2 - l1) * factor;

    return QColor::fromHslF(h, s, l);
}


#if 0
/*
 * 调色板取颜色示例
 *
| 用途 | 推荐角色 | 说明 |
| :--- | :--- | :--- |
| 按钮背景 | `QPalette::Button` | 标准按钮基色 |
| 按钮文字 | `QPalette::ButtonText` | 与 Button 配对的文字色 |
| 选中/激活态 | `QPalette::Highlight` | 系统级强调色 |
| 强调态文字 | `QPalette::HighlightedText` | 通常为白色，与 Highlight 配对 |
| 边框/分隔线 | `QPalette::Mid` / `Dark` | 比 Button 更深的中性色 |
| 禁用态 | `QPalette::Disabled, Role` | 传入 Disabled 组自动获取灰阶版本 |
*/

QColor CircleButton::bgColorFromState(State state) {
    const QPalette pal = palette();
    if (!isEnabled()) {
        return pal.color(QPalette::Disabled, QPalette::Button);
    }

    QColor bgColor;
    switch (state) {
    case Pressed:
        // 按下态
        bgColor   = pal.color(QPalette::Button).darker(600);
        break;
    case Hovered:
        // 悬停态
        bgColor   = pal.color(QPalette::Button).darker(400);
        break;
    default:
        // 正常态
        bgColor   = pal.color(QPalette::Button).darker(200);
        break;
    }
    return bgColor;
}

QColor CircleButton::textColorFromState(State state) {
    const QPalette pal = palette();
    if (!isEnabled()) {
        return pal.color(QPalette::Disabled, QPalette::ButtonText);
    }

    QColor textColor;
    switch (m_state) {
    case Pressed:
        // 按下态
        textColor = pal.color(QPalette::HighlightedText);
        break;
    case Hovered:
        // 悬停态
        textColor = pal.color(QPalette::ButtonText);
        break;
    default:
        // 正常态
        textColor = pal.color(QPalette::ButtonText);
        break;
    }
    return textColor;
}

#endif
