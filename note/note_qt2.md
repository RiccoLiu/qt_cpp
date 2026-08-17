# QString 相关

```
// 1. QString 和 std::string 相互转化
std::string std_str_format = str_format.toStdString();
QString str_format2 = QString::fromStdString(std_str_format);

// 2.
QStringList currentList = model_->stringList();
QString text = currentList.join("\n"); // 每项一行
```

# 定时器 QTimer
```
// 1. 循环触发，每次间隔固定时间触发槽函数 
timer = new QTimer(this);
connect(timer,&QTimer::timeout, this,&MainWindow::slotFunc);

timer->start(1000); // 每 1s 调用一次槽函数
timer->stop();      // 定时器停止

// 2. 触发一次，可用于延迟初始化场景
QTimer::singleShot(2000, this, [] () { qDebug()<<"2秒后执行"; });

timer = new QTimer(this);
timer->setSingleShot(true);     // 设置触发一次

connect(timer,&QTimer::timeout,this,&MainWindow::close);
timer->start(3000);

// 3. 编辑框输入防抖
debounce_timer_ = new QTimer(this);
debounce_timer_->setSingleShot(true);   // 只触发一次
connect(debounce_timer_, &QTimer::timeout, this, &YourClass::onEditingFinished);

connect(text_edit_, &QTextEdit::textChanged, [this]() {
    debounce_timer_->start(500);        // 500ms 内再次输入时，定时器重置
});

void YourClass::onEditingFinished() {
    qDebug() << "User finished editing!";
}
```

# 获取用户输入

```
// 1.QMessageBox 获取警告、提示后的用户反馈
if (QMessageBox::question(this, "确认", "确定要退出吗？") == QMessageBox::Yes) {
    // 退出
} else {
    // 不退出
}

// 2. QInputDialog::getText 获取输入文本
bool ok;
QString newName = QInputDialog::getText(this, "重命名",
                                        "新名称:", QLineEdit::Normal, item->text(0), &ok);
if (ok && !newName.trimmed().isEmpty()) {
    item->setText(0, newName.trimmed());
}

// 3. 从文件浏览器中选择文件

QString title =  "Open Video File";
QString filter = "Video Files (*.mp4 *.avi *.mov *.mkv);;All Files (*)";

QString file = QFileDialog::getOpenFileName(this, title, filePath, filter);
if (file.isEmpty())
    return;

QFileInfo fileInfo(file);
if (fileInfo.exists() && fileInfo.isFile()) {
    QMetaObject::invokeMethod(decoder, "startDecoding", Qt::QueuedConnection, Q_ARG(QString, file));

    filePath = fileInfo.path();
}

```

# QThread 线程

```
find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets Concurrent)

target_link_libraries(mvs
    PRIVATE
        Qt::Core
        Qt::Widgets
        Qt::Concurrent
)
```

访问线程名:
```
// 1. 通过  QObject::thread() 访问
QThread* targetThread = obj->thread();
qDebug() << "目标对象在线程:" << targetThread;

// 2. 函数内部通过 qApp->thread() 访问
void MyClass::mySlot() {
    qDebug() << "当前线程:" << QThread::currentThread();
    qDebug() << "是否为主线程:" << (QThread::currentThread() == qApp->thread());
}

// 3. 断言访问
QMetaObject::invokeMethod(obj, []() {
    Q_ASSERT(QThread::currentThread() == obj->thread()); // 必须在 obj 的线程
    // ...
}, Qt::QueuedConnection);
```

## QThread 适用后台一直运行的长任务
```

void ImageControl::Open() {
    ......

    video_worker_ = new VideoWorker();
    worker_thread_ = new QThread();

    video_worker_->moveToThread(worker_thread_);

    connect(video_worker_, &VideoWorker::FrameReady, this, &ImageControl::ImageReady, Qt::QueuedConnection);
    connect(video_worker_, &VideoWorker::Finished, this, [this] () { Close(); });
    connect(video_worker_, &VideoWorker::Error, this, &ImageControl::Error);

    connect(worker_thread_, &QThread::finished, video_worker_, &QObject::deleteLater);
    connect(worker_thread_, &QThread::finished, worker_thread_, &QThread::deleteLater);

    worker_thread_->start();

    // 开始取流
    QMetaObject::invokeMethod(video_worker_, "Start", Q_ARG(QString, file_));
}
// 销毁线程
void ImageControl::Close() {
    if (video_worker_) {
        QMetaObject::invokeMethod(video_worker_, "Stop");
    }

    if (worker_thread_ && worker_thread_->isRunning()) {
        worker_thread_->quit();
        worker_thread_->wait(); // 等待退出
        worker_thread_ = nullptr;   // 避免重复 delete
    }

    video_worker_ = nullptr;
    worker_thread_ = nullptr;
}

// 槽函数设置 Gamma
void ImageControl::SetGamma(double gamma) {
    if (video_worker_) {
        // 在 video_worker_ 线程中调用 SetGamma 函数，此次调用在 video_worker_线程中是串行的
        QMetaObject::invokeMethod(video_worker_, "SetGamma", Q_ARG(double, gamma));
    }
}

// 槽函数设置帧率
void ImageControl::SetFps(int fps) {
    if (video_worker_) {
        QMetaObject::invokeMethod(video_worker_, "SetFps", Q_ARG(int, fps));
    }
}

```

## QThreadPool + QRunnable 线程池适合大量并发的短任务

```
class Task : public QRunnable {
    void run() override { /* ... */ }
};

QThreadPool::globalInstance()->start(new Task);

QThreadPool::globalInstance()->start([] () {
    // process 
});

```

## QtConcurrent::run 高级线程接口函数

```
#include <QtConcurrent>

QFutureWatcher<int> watcher;
connect(&watcher, &QFutureWatcher<int>::finished, []() {
    qDebug() << "Result:" << watcher.result();
});  

auto future = QtConcurrent::run([]() {
    return heavyComputation();
});                             // 立即执行异步线程的任务
                   
watcher.setFuture(future);      // 建议先连接信号槽再setFuture，避免出现setFuture 和 连接信号槽之间时，任务结束导致没有收到任务

```

## QMetaObject::invokeMethod 用法

跨线程投递到目标对象所属线程执行。

```
QMetaObject::invokeMethod(obj, "Start", Q_ARG(QString, file_), Qt::QueuedConnection);
```

- Qt::QueuedConnection:  obj 在哪个线程，此事件就会在哪个线程执行
- Qt::DirectConnection:  直接在工作线程中执行，如果 obj 不是线程安全时，会 Crush
- Qt::AutoConnection:  工作线程与 obj 线程不同时相当于 Qt::QueuedConnection, 工作线程与 obj 创建线程相同时相当于 Qt::DirectConnection

- 常用用法

```
// 创建线程
video_worker_ = new VideoWorker();
worker_thread_ = new QThread();

video_worker_->moveToThread(worker_thread_);

connect(worker_thread_, &QThread::finished, video_worker_, &QObject::deleteLater);
connect(worker_thread_, &QThread::finished, worker_thread_, &QThread::deleteLater);

worker_thread_->start();

// 开始取流，video_worker_->Start() 在 video_worker_ 线程中执行
QMetaObject::invokeMethod(video_worker_, "Start", Q_ARG(QString, file_), Qt::QueuedConnection);
```

```
// 销毁线程
if (video_worker_) {
    QMetaObject::invokeMethod(video_worker_, "Stop");
}

if (worker_thread_ && worker_thread_->isRunning()) {
    worker_thread_->quit();
    worker_thread_->wait(); // 等待退出
    worker_thread_ = nullptr;   // 避免重复 delete
}

video_worker_ = nullptr;
worker_thread_ = nullptr;
```

!!! 注意：
```
// 假设 this 在主线程，worker 在工作线程创建，此Lammda表达式在worker线程中执行，会 crash！
QMetaObject::invokeMethod(worker, [this]() {
    // ❌ 危险！如果 worker 在工作线程，这里也在工作线程！
    // 但 this 是主线程对象，访问 its 成员可能 crash！
});
```

# QImage 相关

```
// 1.加载 / 保存图片
QImage img("path/to/image.png");
if (img.isNull()) { 
    /* 处理加载失败 */ 
}
img.save("output.jpg", "JPG", 90);

// 2. 分配 QImage 内存
// 2.1 分配16位单通道数据，手动填充全黑
QImage img(width, height, QImage::Format_Grayscale16);
img.fill(0); // 初始化为全黑

// 2.2 零拷贝填充数据
uint16_t *rawBuffer = new uint16_t[width * height];
QImage img(reinterpret_cast<uchar*>(rawBuffer), 
           width, height, 
           width * sizeof(uint16_t),  // bytesPerLine = width * 2
           QImage::Format_Grayscale16);

// 3. 遍历像素
// 3.1. 8位像素，可以使用pixel / setPixel，但是性能较差
// 单通道
int gray = qGray(img.pixel(x, y));
img.setPixel(x, y, qRgb(gray, gray, gray));

// 3通道
QRgb pixel = img.pixel(x, y);

int a = qAlpha(pixelValue); // Alpha 通道 (0-255), 255: 完全不透明
int r = qRed(pixelValue);   // Red   通道 (0-255)
int g = qGreen(pixelValue); // Green 通道 (0-255)
int b = qBlue(pixelValue);  // Blue  通道 (0-255)

img.setPixel(x, y, qRgb(255, 0, 0));

// 3.2. 8位像素，多像素访问和赋值
uchar *data = img.bits();
for (int y = 0; y < img.height(); ++y) {
    uchar *line = data + y * img.bytesPerLine(); // bytesPerLine已经包含对齐填充字节，width*bpp 而非 bytesPerLine
    // 直接通过 line[x] 读写像素数据
}

// 3.3. 16位像素访问预赋值

// 16位像素遍历
for (int y = 0; y < img.height(); ++y) {
    const uint16_t *row = reinterpret_cast<const uint16_t*>(img.constScanLine(y));
    for (int x = 0; x < img.width(); ++x) {
        uint16_t pixel = row[x];
        // 处理 pixel...
    }
}

// 16位像素赋值
uint16_t *line = reinterpret_cast<uint16_t*>(img.scanLine(y));
line[x] = 4096; // 16位范围: 0 ~ 65535

// 整行填充（高性能）
uint16_t *row = reinterpret_cast<uint16_t*>(img.scanLine(y));
std::fill(row, row + width, static_cast<uint16_t>(32768));

memcpy(img.scanLine(y), sourceRow, width * sizeof(uint16_t));

// 4. 几何变换
// 4.1. 缩放（推荐 FastTransformation(最邻近) 用于实时预览，SmoothTransformation(双线性) 用于最终输出）

/*
| 模式 | 行为 | 结果尺寸 | 视觉效果 |
| :--- | :--- | :--- | :--- |
| `Qt::IgnoreAspectRatio` | 强行拉伸到目标尺寸 | 800×600 | 图像变形（圆变椭圆） |
| `Qt::KeepAspectRatio` | 等比缩放，使结果完全放入目标矩形内 | 800×450 | 无变形，但可能留有空白区域 |
| `Qt::KeepAspectRatioByExpanding` | 等比缩放，使结果完全覆盖目标矩形 | 1067×600 | 无变形，但超出部分被裁切 |
*/
QImage scaled = img.scaled(800, 600, Qt::KeepAspectRatio, Qt::SmoothTransformation);

// 4.2. 旋转 / 镜像
QImage rotated = img.transformed(QTransform().rotate(90));
QImage mirrored = img.mirrored(true, false); // 水平翻转

// 4.3. 格式转换（如转灰度、转 RGBA）
QImage gray = img.convertToFormat(QImage::Format_Grayscale8);

// 5. QPainter 绘图
QImage canvas(800, 600, QImage::Format_ARGB32_Premultiplied);
canvas.fill(Qt::transparent);

QPainter painter(&canvas);
painter.drawImage(0, 0, backgroundImg);
painter.drawText(100, 100, "Overlay Text");
painter.end(); // 必须调用 end() 完成绘制

// !! QPainter 使用 QImage 只能使用 Format_ARGB32_Premultiplied 格式， 这个格式已经把透明度预乘 alpha / 255 到RGB分量

painter.fillRect(rect, QColor(255, 0, 0, 128)); // 半透明红
QRgb p = img.pixel(x, y);
int red = qRed(p); // red并不是255, 而是 255 * 128 / 255 = 128

// ✅ 如果要做像素级处理，先转为非预乘格式
QImage straight = img.convertToFormat(QImage::Format_ARGB32); 

// 6. 与QPixmap / QVideoFrame 互转
// 6.1 QPixmap转换
QPixmap pixmap = QPixmap::fromImage(img);
QImage processed = pixmap.toImage();

// 6.2. 从原始缓冲区构造（零拷贝场景，如相机 SDK 回调）
QImage fromBuffer(rawData, width, height, bytesPerLine, QImage::Format_RGB888);

```

#  QPixmap

!! 预渲染使用的位图要基于物理像素制作的位图

| 概念 | 说明 |
| :--- | :--- |
| 逻辑像素 | Qt 布局系统使用的单位，例如 `width() = 80` |
| 物理像素 | 屏幕上实际的像素点数量 |
| DPR | 物理像素 / 逻辑像素的比值 |

```
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
```

## QPainter

1. 基础元绘制

| 方法 | 作用 | 💡 实战要点 |
| :--- | :--- | :--- |
| `drawLine()` / `drawLines()` | 绘制直线/批量直线 | 批量绘制用 `drawLines(QVector<QLineF>)` 比循环调用 `drawLine` 快数十倍 |
| `drawRect()` / `drawRoundedRect()` | 矩形/圆角矩形 | 圆角矩形是 UI 中最常用的形状，支持统一或四角独立半径 |
| `drawEllipse()` | 椭圆/正圆 | 传入 `QRectF` 时注意：矩形宽高相等才是正圆 |
| `drawArc()` / `drawPie()` | 弧线/扇形 | 角度单位是 1/16度（如90°=1440），用于仪表盘、进度环 |
| `drawPath()` | 绘制 QPainterPath | ⭐ 性能之王：复杂形状先构建 Path 再一次性绘制 |
| `drawText()` | 绘制文本 | 支持富文本标志位；精确布局建议用 `QFontMetrics` 预计算 |
| `drawImage()` / `drawPixmap()` | 绘制图片 | Pixmap 在 GPU 上缓存，频繁绘制优先用 Pixmap 而非 Image |

- drawRect(int x, int y, int w, int h) → 像素对齐，适合边框、分割线
- drawRect(const QRectF&) → 亚像素精度，配合抗锯齿使用，适合平滑动画
- 混用两者会导致视觉抖动，在一个控件内尽量统一选择一种

```
painter.setPen(m_textColor);
painter.setBrush(Qt::NoBrush);

QFont font = this->font();
font.setPixelSize(ringRect.height() * 0.25);    // 设置字体尺寸
font.setBold(true);                             // 加粗
painter.setFont(font);

QString text = QString("%1%").arg(static_cast<int>(m_progress * 100));
painter.drawText(ringRect, Qt::AlignCenter, text); // 居中写字
```

画笔的“居中描边”机制 (Pen Alignment)

在 Qt（以及绝大多数 2D 图形 API）中，QPainter 绘制形状轮廓时，画笔是以路径为中心向两侧扩展的。

| 场景 | 推荐写法 | 说明 |
| :--- | :--- | :--- |
| 纯填充无边框 | `rect().adjusted(0.5, 0.5, -0.5, -0.5)` | 仅需为抗锯齿留出亚像素空间 |
| 带边框 (penWidth=N) | `rect().adjusted(N/2.0+ε, N/2.0+ε, -(N/2.0+ε), -(N/2.0+ε))` | 必须为居中外扩留出完整空间 |
| 聚焦环 (penWidth=2) | `rect().adjusted(2, 2, -2, -2)` | 留 2px = 1px(半笔宽) + 1px(抗锯齿缓冲) |
| 阴影/光晕效果 | 需要更大的 margin | 模糊半径可能向外扩展数十像素 |

```
QRectF ringRect = rect().adjusted(m_ringWidth / 2, m_ringWidth / 2, -m_ringWidth / 2, -m_ringWidth / 2);

QPen bgPen(m_bgColor, m_ringWidth, Qt::SolidLine, Qt::RoundCap);
painter.setPen(bgPen);
painter.setBrush(Qt::NoBrush);
painter.drawEllipse(ringRect);
```

2. QPen & QBrush
```
// QPen 关键设置
QPen pen;
pen.setColor(QColor(70, 130, 230));
pen.setWidthF(2.5);           // 浮点宽度，配合抗锯齿
pen.setCapStyle(Qt::RoundCap); // 线帽：圆形端点（进度条末端圆润）
pen.setJoinStyle(Qt::RoundJoin); // 拐角：圆角连接
pen.setStyle(Qt::DashDotLine);   // 虚线样式
painter.setPen(pen);

QPen bgPen(m_bgColor, lineWidth, Qt::SolidLine, Qt::RoundCap);


// QBrush 关键类型
painter.setBrush(Qt::SolidPattern);      // 纯色填充
painter.setBrush(QGradient(...));        // 渐变填充（线性/径向/锥形）
painter.setBrush(QPixmap("pattern.png"));// 纹理填充
painter.setBrush(Qt::NoBrush);           // ⭐ 只画轮廓不填充时务必设置
```

3. 坐标变换

| 方法 | 作用 | 典型场景 |
| :--- | :--- | :--- |
| `translate(dx, dy)` | 平移原点 | 将原点移到控件中心，简化后续绘制逻辑 |
| `rotate(angle)` | 旋转坐标系 | 仪表指针、时钟刻度、环形布局 |
| `scale(sx, sy)` | 缩放 | 适配 HiDPI、缩略图预览 |
| `save()` / `restore()` | ⭐ 保存/恢复状态栈 | 最重要的方法之一，见下方详解 |


```
// ✅ 正确：局部变换不影响后续绘制
painter.save();
painter.translate(width()/2, height()/2); // 原点移到中心
painter.rotate(m_angle);                   // 旋转
painter.drawRoundedRect(-20, -5, 40, 10, 3, 3); // 以新坐标系绘制指针
painter.restore(); // ← 恢复到 save() 之前的状态

// ❌ 错误：忘记 restore，后续所有绘制都被旋转污染
painter.translate(width()/2, height()/2);
painter.rotate(m_angle);
painter.drawRoundedRect(...);
// 后面的文字、背景全歪了！
```

4. 裁剪与合成

| 方法 | 作用 | 应用场景 |
| :--- | :--- | :--- |
| `setClipRect()` | 矩形裁剪 | 滚动列表只绘制可见区域、进度条遮罩 |
| `setClipPath()` | 路径裁剪 | 异形头像、文字镂空效果 |
| `setCompositionMode()` | 混合模式 | 叠加发光、擦除效果、半透明混合 |

```
// 经典案例：圆形头像裁剪
QPainterPath clipPath;
clipPath.addEllipse(rect());
painter.setClipPath(clipPath);
painter.drawPixmap(rect(), avatarPixmap); // 只有圆形区域内的像素被绘制
painter.setClipping(false); // 记得关闭裁剪
```

5. 渲染质量开关
```
painter.setRenderHint(QPainter::Antialiasing);        // 几何抗锯齿
painter.setRenderHint(QPainter::SmoothPixmapTransform); // 图片缩放平滑
painter.setRenderHint(QPainter::TextAntialiasing);       // 文字抗锯齿
painter.setRenderHint(QPainter::VerticalSubpixelPositioning); // 文字垂直亚像素定位
```
⚡ 性能权衡：Antialiasing 会增加 CPU/GPU 开销。对于大量重复的简单矩形/直线（如表格网格），可以临时关闭；对于圆弧、旋转图形、HiDPI 下的文字，必须开启。

6. 项目测试

- 入门： 自绘一个带 hover/pressed 状态的圆形按钮。
- 进阶： 自绘一个平滑动画的环形进度条（结合 QPropertyAnimation）。
- 高级： 自绘一个支持缩放、拖拽、多选的时间轴控件。
- 专家： 自绘一个高性能实时波形显示器（60FPS，万级数据点）。

# 视频显示控件

## QLabel 简单显示

```
// 初始化
displayer = new QLabel(this);
setCentralWidget(displayer);

displayer->setScaledContents(true);             // 自动缩放到控件大小
displayer->setAlignment(Qt::AlignCenter);       // 居中显示
displayer->setBackgroundRole(QPalette::Dark);   // 可选：深色背景

// 显示
displayer->setPixmap(QPixmap::fromImage(img));
```
## QGraphicsView 2D 图像显示

| 组件 | 角色 | 类比 | 职责 |
| :--- | :--- | :--- | :--- |
| `QGraphicsScene` | Model | 画布/世界 | 管理所有图元、处理碰撞检测、维护Z序 |
| `QGraphicsItem` | Data Element | 画布上的物体 | 矩形、椭圆、图片、自定义形状等 |
| `QGraphicsView` | View + Controller | 相机/窗口 | 渲染场景、处理缩放/平移/鼠标事件 |

```
// 1. 创建场景
QGraphicsScene *scene = new QGraphicsScene(this);
scene->setSceneRect(0, 0, 800, 600);  // 定义"世界"坐标系范围

// 2. 添加图元
QGraphicsRectItem *rect = scene->addRect(50, 50, 200, 100,
    QPen(Qt::blue), QBrush(Qt::cyan));
rect->setFlag(QGraphicsItem::ItemIsMovable);   // 可拖拽
rect->setFlag(QGraphicsItem::ItemIsSelectable); // 可选中

QGraphicsEllipseItem *ellipse = scene->addEllipse(300, 200, 150, 150,
    QPen(Qt::red), QBrush(QColor(255, 200, 200)));

// 3. 创建视图并关联场景
QGraphicsView *view = new QGraphicsView(scene, this);
view->setRenderHint(QPainter::Antialiasing);       // 抗锯齿
view->setDragMode(QGraphicsView::ScrollHandDrag);  // 手型拖拽平移
view->setTransformationAnchor(QGraphicsView::AnchorUnderMouse); // 以鼠标为中心缩放

setCentralWidget(view);
```

<!-- | 坐标系 | 说明 | 转换方法 |
| :--- | :--- | :--- |
| Scene 坐标 | 逻辑世界坐标，图元的位置都基于此 | `item->pos()`, `scene->items()` |
| View/Widget 坐标 | 像素坐标，即 View 控件上的屏幕像素 | `view->mapToScene()`, `view->mapFromScene()` |
| Item 局部坐标 | 相对于图元自身原点(0,0)的坐标 | `item->mapToScene()`, `item->mapFromParent()` | -->

四大坐标系：

| 坐标系 | 别名 | 原点位置 | 单位 | 谁在动？ | 典型用途 |
| :--- | :--- | :--- | :--- | :--- | :--- |
| Scene | 场景坐标 | 逻辑世界的绝对原点 | 逻辑像素（浮点数） | 永远不动（作为基准） | 存储所有 Item 的位置、大小；`addItem`、碰撞检测 |
| Item | 图元局部坐标 | Item 自身的 `(0,0)` 点 | 逻辑像素（浮点数） | Item 调用 `setPos/setRotation/setScale` 时相对于 Scene 变化 | 绘制 Item 内容（`paint()`）、处理 Item 内部鼠标事件 |
| View | 视图坐标 | QGraphicsView 视口（viewport）左上角 | 物理像素（整数） | 用户滚动/缩放时变化 | 视口裁剪、`fitInView`、`ensureVisible` |
| Widget | 窗口坐标 | 应用程序窗口左上角 | 物理屏幕像素 | 窗口移动/系统 DPI 变化时变化 | 弹出菜单定位、跨窗口拖拽、系统级交互 |

!! Qt API中返回和使用的都是逻辑像素，如果需要转换物理像素需要乘以dpr(qApp->devicePixelRatio())

四大坐标系的转换：

| 从 → 到 | 转换方法 | 备注 |
| :--- | :--- | :--- |
| Item ↔ Scene | `item->mapToScene()` / `item->mapFromScene()` | 自动包含 Item 的 pos/rotation/scale/shear |
| Scene ↔ View | `view->mapToScene()` / `view->mapFromScene()` | 自动包含当前 zoom + scroll offset |
| View ↔ Widget | `widget->mapToGlobal()` / `widget->mapFromGlobal()` | 包含窗口边框、标题栏偏移 |
| Item ↔ View | `view->mapFromScene(item->mapToScene(localPoint))` | 需两步中转，无直接 API |
| 任意两点 | `QGraphicsItem::mapToItem(otherItem, point)` | Item 之间直接转换，最常用 |

转换顺序: Item坐标 <---> Scene坐标 <---> View坐标 <---> Widget坐标

```

// 典型场景：鼠标点击时获取 Scene 坐标，鼠标位置响应的坐标系是view坐标系
void MyView::mousePressEvent(QMouseEvent *event) {
    QPointF scenePos = mapToScene(event->pos());  // ✅ view → Scene
    qDebug() << "Clicked at scene pos:" << scenePos;
    
    // 查找该位置的图元
    QList<QGraphicsItem*> items = scene()->items(scenePos);
}

```

缩放与平移：

```
// 滚轮缩放（需重写 wheelEvent 或使用以下快捷方式）
view->scale(1.2, 1.2);   // 放大 20%
view->scale(1/1.2, 1/1.2); // 缩小

// 平移模式
view->setDragMode(QGraphicsView::ScrollHandDrag);     // 按住鼠标拖拽画布
view->setDragMode(QGraphicsView::RubberBandDrag);     // 框选模式（默认）

// 缩放锚点策略
view->setTransformationAnchor(QGraphicsView::AnchorUnderMouse);      // 以鼠标位置为中心
view->setResizeAnchor(QGraphicsView::AnchorViewCenter);              // resize 时以视图中心为基准

```

选择与多选

```
view->setSelectionMode(QAbstractItemView::ExtendedSelection); // Ctrl/Shift 多选
// 图元必须设置 ItemIsSelectable 标志才能被选中
```

性能优化(大场景必做)

```
// 空间索引：大量图元时加速碰撞检测和区域查询
scene->setItemIndexMethod(QGraphicsScene::BspTreeIndex); // 默认，适合静态场景
// scene->setItemIndexMethod(QGraphicsScene::NoIndex);   // 动态频繁移动时反而更快

// 渲染优化
view->setViewportUpdateMode(QGraphicsView::SmartViewportUpdate); // 智能刷新（推荐）
view->setCacheMode(QGraphicsView::CacheBackground);              // 缓存背景
view->setOptimizationFlags(QGraphicsView::DontSavePainterState); // 高级优化
```


自定义图元
```
class MyImageItem : public QGraphicsItem {
public:
    MyImageItem(const QImage &img) : m_image(img) {}

    // ✅ 必须实现：返回图元的边界矩形（局部坐标）
    QRectF boundingRect() const override {
        return QRectF(0, 0, m_image.width(), m_image.height());
    }

    // ✅ 必须实现：绘制逻辑
    void paint(QPainter *painter, 
               const QStyleOptionGraphicsItem *option,
               QWidget *widget) override {
        painter->drawImage(0, 0, m_image);
    }

private:
    QImage m_image;
};
```

!!  如果只需要简单组合现有图元，用 QGraphicsItemGroup 或 QGraphicsObject（支持信号槽和属性动画）更方便，不必从头实现 boundingRect 和 paint

典型用法

| 需求 | 实现方式 |
| :--- | :--- |
| 显示大图并支持缩放/平移浏览 | `QGraphicsPixmapItem` + `ScrollHandDrag` + 滚轮缩放 |
| 在图像上标注 ROI 区域 | 自定义 `QGraphicsRectItem` + `ItemIsMovable` + `ItemIsResizable` |
| 实时预览滤镜效果 | 将处理后的 QImage 更新到 PixmapItem，或用 ShaderEffect |
| 多图层叠加显示 | 利用 Z 值 (`setZValue`) 控制图层顺序 |
| 测量工具（距离/角度） | 自定义 LineItem + 鼠标事件计算 Scene 坐标距离 |
| 缩略图导航 | 第二个小 `QGraphicsView` 共享同一个 Scene，固定显示全貌 |

流程：

1. 先跑通最小示例（上面的代码），感受 Scene/View/Item 三者关系
2. 练习坐标转换：写一个鼠标跟踪显示 Scene 坐标的小工具
3. 实现自定义图元：从简单的矩形开始，逐步加入交互
4. 研究官方示例：Qt 自带的 diagramscene、elasticnodes、collidingmice 是最佳教材
5. 性能调优：当图元超过数千个时，研究 BSP Tree、LOD、视口裁剪策略


## QOpenGLWidget 3D 图像显示

```
// myglwidget.h
class MyGLWidget : public QOpenGLWidget, protected QOpenGLFunctions_3_3_Core {
    Q_OBJECT
public:
    explicit MyGLWidget(QWidget *parent = nullptr);
    ~MyGLWidget() override;

protected:
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;

private:
    GLuint m_vao = 0, m_vbo = 0;
    std::unique_ptr<QOpenGLShaderProgram> m_program;
};

// myglwidget.cpp
MyGLWidget::MyGLWidget(QWidget *parent) : QOpenGLWidget(parent) {}

MyGLWidget::~MyGLWidget() {
    makeCurrent(); // ⚠️ 析构时必须确保上下文激活
    glDeleteVertexArrays(1, &m_vao);
    glDeleteBuffers(1, &m_vbo);
    doneCurrent();
}

void MyGLWidget::initializeGL() {
    initializeOpenGLFunctions(); // 初始化函数指针

    // 设置背景色
    glClearColor(0.1f, 0.1f, 0.15f, 1.0f);

    // 编译着色器程序
    m_program = std::make_unique<QOpenGLShaderProgram>(this);
    m_program->addShaderFromSourceCode(QOpenGLShader::Vertex, R"(
        #version 330 core
        layout(location=0) in vec3 aPos;
        void main() { gl_Position = vec4(aPos, 1.0); }
    )");
    m_program->addShaderFromSourceCode(QOpenGLShader::Fragment, R"(
        #version 330 core
        out vec4 fragColor;
        void main() { fragColor = vec4(0.2, 0.7, 1.0, 1.0); }
    )");
    m_program->link();

    // 三角形顶点数据
    float vertices[] = { -0.5f,-0.5f,0,  0.5f,-0.5f,0,  0,0.5f,0 };

    glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao);

    glGenBuffers(1, &m_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), nullptr);
    glEnableVertexAttribArray(0);
}

void MyGLWidget::resizeGL(int w, int h) {
    glViewport(0, 0, w, h);
}

void MyGLWidget::paintGL() {
    glClear(GL_COLOR_BUFFER_BIT);
    m_program->bind();
    glBindVertexArray(m_vao);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    m_program->release();
}
```

1. 先跑通上面的三角形示例，确认环境正确
2. 练习纹理加载：用 QOpenGLTexture + QImage 显示一张图片
3. 实现相机控制：鼠标拖拽旋转 + 滚轮缩放（结合 QMatrix4x4）
4. 集成到实际项目：将 GL Widget 嵌入你的图像处理工具界面，旁边放参数面板
5. 研究官方示例：openglwindow、textures、hellogl2、computegles31

# 杂项

## 窗口固定大小尺寸

```
setWindowFlags(windowFlags() & ~Qt::WindowMinMaxButtonsHint);
setFixedSize(size());
```

## 编辑框只能输入数字

```
// 1. 只能输入整数，前导0会被删掉
QIntValidator* validator = new QIntValidator(0, 100, line_edit_); // 只能输入0-100数据
line_edit_->setValidator(validator);

// 2. 只能输入小数，前导0会被删掉
QDoubleValidator* validator = new QDoubleValidator(-999.99, 999.99, 2, this); // 允许输入 -999.99 ~ 999.99，最多 2 位小数
validator->setNotation(QDoubleValidator::StandardNotation); // 避免科学计数法
score->setValidator(validator);

// 3. 使用正则表达式，不会删除前导0
QRegularExpression regExp("^[0-9]*$"); // 只允许数字，可为空
id->setValidator(new QRegularExpressionValidator(regExp, this));

```

## QGroupBox & QToolButton 下拉菜单 + QActionGroup

```
// 1. QGroupBox：分组容器控件，一个包围框把这些控件圈起来
QGroupBox* group = new QGroupBox(tr("SCAN"));

// GroudBox布局： 一定要为 QGroupBox 分配布局
QVBoxLayout* group_layout = new QVBoxLayout(group);
group_layout->addWidget(toolbar);
group_layout->addWidget(property_);

// 主布局
QVBoxLayout* layout = new QVBoxLayout(property_view_);
layout->addWidget(group);
property_view_->setLayout(layout);

// 2. QToolButton + QMenu：画面布局（下拉式菜单）: 单画面、四画面、九画面
screen_layout_ = new QToolButton(this);
screen_layout_->setText(tr("Screen Layout"));
screen_layout_->setIcon(QIcon(":/png/screen_layout.png"));

// screen_layout_->setPopupMode(QToolButton::DelayedPopup);    // 短按执行动作、长按出现下拉菜单
// screen_layout_->setPopupMode(QToolButton::MenuButtonPopup); // 点击按钮执行动作、点击下拉按钮出现下拉菜单
screen_layout_->setPopupMode(QToolButton::InstantPopup);    // 点击开启下拉菜单

QMenu* screen_menu = new QMenu(tr("Screen Menu"), screen_layout_);
QAction* single_screen = screen_menu->addAction(tr("Single Screen"));
QAction* four_screen = screen_menu->addAction(tr("Four Screen"));
QAction* nine_screen = screen_menu->addAction(tr("Nine Screen"));

screen_layout_->setMenu(screen_menu);

// 3. QActionGroup：实现多个QAction选择一个(互斥)
QActionGroup* screen_group = new QActionGroup(screen_menu);
screen_group->setExclusive(true);
screen_group->addAction(single_screen);
screen_group->addAction(four_screen);
screen_group->addAction(nine_screen);

single_screen->setCheckable(true);
four_screen->setCheckable(true);
nine_screen->setCheckable(true);
single_screen->setChecked(true);    // 默认选择
```

# Q&A

## 点击按键 创建一个新的窗口: QPointer

```
void DeviceView::OnStartStreaming() {
    if (!scanner_) {                                    
        scanner_ = new ScannerWindow();                             // 懒加载
        scanner_->setAttribute(Qt::WA_DeleteOnClose);               // 设置属性: 窗口关闭时自动释放内存, 防止内存泄漏
    }
    scanner_->show();
    scanner_->raise();
    scanner_->activateWindow();
}

void DeviceView::OnStopStreaming() {
    if (scanner_) {
        scanner_->close(); // 触发 WA_DeleteOnClose ，自动销毁并触发 destroyed 信号将 scanner_ 置空
    }
}

QPointer<ScannerWindow> scanner_;

```

## setSizePolicy 配合布局(VBoxLayout) 实现控件填充

```
// 横向和纵向拉伸策略
setSizePolicy(QSizePolicy::Policy hor, QSizePolicy::Policy ver);

enum Policy {
    Fixed = 0,                              //  使用 sizeHint 的尺寸、不拉伸
    Minimum = GrowFlag,                     //  sizeHint尺寸 -> 最大尺寸
    Maximum = ShrinkFlag,                   //  最小尺寸 -> sizeHint尺寸
    Preferred = GrowFlag | ShrinkFlag,      // 布局的理想尺寸，布局可以拉伸窗口
    MinimumExpanding = GrowFlag | ExpandFlag,   //
    Expanding = GrowFlag | ShrinkFlag | ExpandFlag, // 主动填充窗口
    Ignored = ShrinkFlag | GrowFlag | IgnoreFlag
};

```
```
用户拉伸窗口 / 添加新控件 / setText() 触发 updateGeometry()
        │
        ▼
┌─────────────────────┐
│  布局管理器开始计算   │
│                     │
│  1. 调用 sizeHint()  │ ◄── "你理想尺寸是多少？"
│  2. 调用 minimumSizeHint() │ ◄── "你最小能缩到多少？"
│  3. 结合 SizePolicy  │
│     + 其他控件的 hint │
│     + 可用空间       │
│  4. 计算出最终分配尺寸 │
└─────────┬───────────┘
          │
          ▼
┌─────────────────────┐
│  setGeometry(rect)  │ ◄── 布局将最终尺寸应用到控件
└─────────┬───────────┘
          │
          ▼
┌─────────────────────┐
│   resizeEvent()     │ ◄── "你的尺寸现在是 W×H 了"
│   (此时 rect() 已更新)│
│                     │
│   → 更新内部状态     │
│   → 触发 update()   │
└─────────────────────┘
```

## 应用程序元信息

```
QApplication app(argc, argv);

// 设置应用元信息
app.setApplicationName("MyMVS");
app.setApplicationVersion("1.0");
app.setOrganizationName("YourCompany");
app.setOrganizationDomain("yourCompany.com");

// 获取应用元信息
qDebug() << "app:" << QApplication::applicationName();
qDebug() << "version:" << QApplication::applicationVersion();
qDebug() << "organization:" << QApplication::organizationName();
qDebug() << "organization domain:" << QApplication::organizationDomain();
```

## QT 中 CMake的相关配置 

CMAKE_AUTORCC： 自动处理 .qrc 资源文件： 图标、字体、stylesheet

```
set(CMAKE_AUTORCC ON) 
```

触发条件:

1. CMake 中使用 target_sources() 设置项目依赖 .qrc文件
```
target_sources(mvs PRIVATE resources.qrc)
或者
qt_add_executable(mvs resource/resources.qrc)
```

CMAKE_AUTOMOC: 自动生成QT 元对象系统，可以使用信号槽、运行时类型信息等

```
set(CMAKE_AUTOMOC ON)  
```

触发条件: 

1. 头文件（.h）中包含 Q_OBJECT、Q_GADGET、Q_NAMESPACE 等宏
2. 源文件(cpp) 中包含 #include "management_window.moc"

CMAKE_AUTOUIC: 自动将 .ui 文件转换为 C++ 头文件（ui_xxx.h）

```
set(CMAKE_AUTOUIC ON)
```

触发条件：

1. 源码中 #include "ui_xxx.h"
2. CMake 中使用了 target_sources() 设置项目依赖 .ui 文件

## Qt 的智能指针 QPointer, QSharedPointer, QWeakPointer， QScopedPointer

- QPointer: 弱引用观察者, 仅适用于 QObject 及其子类通过对象树管理的场景，对象被删除后自动置为 nullptr 
- QSharedPointer：共享所有权，适用于非 QObject 对象。
- QWeakPointer: 共享所有权(弱引用)
- QScopedPointer： 独占所有权

!! 对于非 QObject 对象，强烈建议使用标准库的 std::shared_ptr + std::unique_ptr, 除非是维护老旧的Qt项目不得以使用Qt的智能指针。

# 部署 & 发布

打包命令：

```
windeployqt6.exe mysql.exe
```


