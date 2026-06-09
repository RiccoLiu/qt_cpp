# QString 相关

```
QStringList currentList = model_->stringList();
QString text = currentList.join("\n"); // 每项一行
```

# QThread 相关

## QThread 适用后台一直运行的长任务
```
// 构造函数
Decoder* worker = new Decoder;      // 普通 QObject
QThread* thread = new QThread;
decoder->moveToThread(thread);      // 移动到线程
thread->start();                    // 启动线程
connect(decoder, &VideoDecoder::frameDecoded, detector, &FaceDetector::detect);    // 连接信号槽

QMetaObject::invokeMethod(decoder, "startDecoding", Q_ARG(QString, "video.mp4"));   // 在主线程调用子线程的开始解码函数

// 析构函数
QMetaObject::invokeMethod(decoder, "stop");     // 停止工作
decoderThread->quit();              // 请求退出事件循环
decoderThread->wait();              // 阻塞直到线程真正退出
delete decoder;
```

## QThreadPool + QRunnable 线程池适合大量并发的短任务

```
class Task : public QRunnable {
    void run() override { /* ... */ }
};

QThreadPool::globalInstance()->start(new Task);
```

## QtConcurrent::run 高级线程接口函数

```
#include <QtConcurrent>

auto future = QtConcurrent::run([]() {
    return heavyComputation();
});                             // 立即执行异步线程的任务

QFutureWatcher<int> watcher;
connect(&watcher, &QFutureWatcher<int>::finished, []() {
    qDebug() << "Result:" << watcher.result();
});                     
watcher.setFuture(future);      // 建议先连接信号槽再setFuture，避免出现setFuture 和 连接信号槽之间时，任务结束导致没有收到任务

```

# 视频显示控件

## QLabel

```
displayer = new QLabel(this);
setCentralWidget(displayer);

displayer->setScaledContents(true);             // 自动缩放到控件大小
displayer->setAlignment(Qt::AlignCenter);       // 居中显示
displayer->setBackgroundRole(QPalette::Dark);   // 可选：深色背景
```
## QGraphicsView

...

## QOpenGLWidget

...

# 定时器 QTimer
## 标准模板
```
timer = new QTimer(this);

connect(timer,&QTimer::timeout,
        this,&MainWindow::slotFunc);

timer->start(1000); // 默认循环触发，每1s调用一次超时函数

timer->stop(); // 定时器停止
```

## 延迟初始化

``` 
// 单次定时器，2s后执行一次， 相比与  QThread::sleep(3); 这种阻塞调用更好
QTimer::singleShot(2000, this, [] () {
                        qDebug()<<"2秒后执行";
                    });
```

```
timer->setSingleShot(true); // 设置定时器只触发一次
connect(timer,&QTimer::timeout,this,&MainWindow::close);
timer->start(3000);
```

## 编辑输入防抖

```
// 定时器在编辑框中防抖使用方法

// 头文件中声明
QTimer* debounce_timer_;

// 构造函数中初始化
debounce_timer_ = new QTimer(this);
debounce_timer_->setSingleShot(true); // 设置定时器只触发一次(默认循环触发)
connect(debounce_timer_, &QTimer::timeout, this, &YourClass::onEditingFinished);

// 编辑框 -> 定时器启动或更新 -> 定时器超时 -> 处理函数
connect(text_edit_, &QTextEdit::textChanged, [this]() {
    debounce_timer_->start(500); // 500ms 后触发，若期间再次输入则重置
});

// 处理函数
void YourClass::onEditingFinished() {
    qDebug() << "User finished editing!";
}
```

# 获取用户输入

## QMessageBox 获取警告、提示后的用户反馈

```
if (QMessageBox::question(this, "确认", "确定要退出吗？") == QMessageBox::Yes) {
    // 退出
} else {
    // 不退出
}
```

## QInputDialog 从对话框获取文本
```
bool ok;
QString newName = QInputDialog::getText(this, "重命名",
                                        "新名称:", QLineEdit::Normal, item->text(0), &ok);
if (ok && !newName.trimmed().isEmpty()) {
    item->setText(0, newName.trimmed());
}
```

## 从文件浏览器中选择文件
```
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

# QSS 样式

## 样式属性

| 属性 | 作用 | 示例 |
|-------|------|------|
| background-color | 背景色 | background-color: #f0f0f0ff; |
| color | 文字颜色 | color: red; |
| border | 边框（宽度+样式+颜色） | border: 2px solid #ccc; |
| border-radius | 圆角 | border-radius: 8px; |
| font-size | 字体大小 | font-size: 16px; |
| font-family | 字体 | font-family: "Microsoft YaHei"; |
| padding | 内边距 | padding: 10px; |
| margin | 外边距（部分控件支持） | margin: 5px; |

## 子控件属性

## 伪状态

```
button->setStyleSheet(R"(
    QPushButton {
        background-color: #4CAF50;
        color: white;
        border: none;
        padding: 8px;
    }
    QPushButton:hover {
        background-color: #45a049;  /* 鼠标悬停 */
    }
    QPushButton:pressed {
        background-color: #3e8e41;  /* 按下时 */
    }
    QPushButton:disabled {
        background-color: #cccccc;  /* 禁用状态 */
        color: #666666;
    }
)");

```


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

# Q&A

## UI操作耗时长会卡UI显示的问题

