
# 搭建开发环境

下载 qt-online-installer-windows-x64-4.10.0 运行安装程序，选择 MSVC & MinGW 编译器支持，CDB 支持。

下载 Window-SDK 运行安装程序，CDB 支持。

# QT 基础

## 内存管理

Qt 提供了一套基于父子对象树自动释放内存的机制，每个继承于 QObject 的类都有一个 QObject* parent; 对象，当父对象被销毁时会自动 delete 所有子对象，所以 UI 对象建议都传 parent, 对于QObject对象使用父子树+ deleteLater管理。

```
MainDialog::MainDialog(QWidget *parent)
    : QDialog(parent) {
    disp_label_ = new QLabel(this);
    // tr: 支持将字符串转换成多国语言， 建议是将用户可见的文本都用tr来包装，
    disp_label_->setText(tr("请输入圆的半径："));

    input_edit_ = new QLineEdit(this);
    res_label_ = new QLabel(this);
    apply_btn_ = new QPushButton(this);
}
```

异步对象QObject 删除事件: deleteLater, 当 deleteLater调用后会向事件队列中发送一个“延迟删除事件”，等当前事件处理完，事件队列为空后，再执行自动删除delete 操作。

```
[ 当前槽函数执行中 ]
        ↓
调用 deleteLater()
        ↓
槽函数执行结束
        ↓
返回事件循环
        ↓
Qt 处理 DeferredDelete 事件
        ↓
真正 delete 对象
```

deleteLater 的典型使用场景
```
// 槽函数中销毁UI
void MainWindow::closeSubWindow()
{
    subWindow->deleteLater();
}
```

```
// 异步线程
QThread* thread = new QThread;

connect(thread, &QThread::finished,
        thread, &QObject::deleteLater);

```

```
// 异步网络编程
QTcpSocket* socket = new QTcpSocket;

connect(socket, &QTcpSocket::disconnected,
        socket, &QObject::deleteLater);
```


非QObeject对象建议使用 Qt 的智能指针：QScopedPointer(独占指针), QSharedPointer(强指针), QPointer(弱指针)

```
QScopedPointer<MyClass> ptr(new MyClass); // 类似 Unique_ptr
QSharedPointer<MyClass> ptr = QSharedPointer<MyClass>::create(); // 类似shared_ptr
QPointer<QObject> ptr = obj; // 类似 weak_ptr
```

隐式共享内存： QString、QByteArray、QImage、QVector、QList、QMap等对象会采用一种 写时拷贝 的策略。
```
QString str1 = "hello";
QString str2 = str1;    // 这里并不会分配新的内存

std::cout << "str1 data: " << static_cast<const void*>(str1.constData()) << std::endl;
std::cout << "str2 data: " << static_cast<const void*>(str2.constData()) << std::endl;

str2[0] = 'H';          // 写拷贝机制，这里会分配新的内存
// str2.detach();       // detach 会强制分配新的内存

std::cout << "After modify:" << std::endl;
std::cout << "str1 data: " << static_cast<const void*>(str1.constData()) << std::endl;
std::cout << "str2 data: " << static_cast<const void*>(str2.constData()) << std::endl;
```

## 界面布局

| 函数 | 功能 |
|-----------| ----------|
| QVBoxLayout | 垂直布局 |
| QHBoxLayout | 水平布局 |
| QGridLayout | 网格布局 |

## 信号槽

QT 信号槽是一种对象之间的类型安全的事件通信机制。主要用于替代 回调函数、观察者模式、消息机制、函数指针的场景。

信号槽函数connect的用法：
```
// 信号槽连接
QMetaObject::Connection conn = QObject::connect(
    sender,                 // 发信号的对象
    &SenderClass::signal,   // 信号函数
    receiver,               // 接受对象
    &ReceiverClass::slot    // 槽函数
    Qt::ConnectionType      // 信号槽的连接类型
);

ConnectionType：
    AutoConnection(默认)：同一个线程立即调用，不同线程放入事件队列，按照队列顺序执行
    DirectConnection：立即调用，不经过事件队列
    QueuedConnection：放入事件队列，按照队列顺序执行
    BlockingQueuedConnection：阻塞调用线程，直到槽执行完

// 信号槽断开连接
disconnect(conn)

// 信号槽全部断开连接
disconnect(sender, nullptr, receiver, nullptr);

// 信号槽断开指定连接
disconnect(btn, &QPushButton::clicked, this, &MainWindow::onClicked);

// 当 receiver 被销毁时，信号槽自动断开连接
```

典型用法如下：
```
// 按键点击消息，槽函数 onClicked 进行响应
connect(button, &QPushButton::clicked, this, &MainWindow::onClicked);

// 编辑框更改消息，槽函数 updateText 进行响应
connect(lineEdit, &QLineEdit::textChanged, this, &MainWindow::updateText);

// 编辑框更改消息，槽函数 Lambda 表达式进行响应
connect(lineEdit, &QLineEdit::textChanged, this,
        [this](const QString& text)
        {
            qDebug() << text;
        });

// 连接到其他信号，A::sig发送信号后，B::Sig也发送信号
connect(obj1, &A::sig, obj2, &B::sig);

// 跨线程信号槽
worker->moveToThread(thread);
connect(thread, &QThread::started, worker, &Worker::process);
```

## 控件

| 控件 | 功能 |
|----------|----------|
| QPushButton | 按键 |
| QLabel | 静态文本框 |
| QLineEdit | 编辑框 |

# TIPS

# Q&A
