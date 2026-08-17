
# 搭建开发环境

下载 qt-online-installer-windows-x64-4.10.0 运行安装程序，选择 MSVC & MinGW 编译器支持，CDB 支持。

下载 Window-SDK 运行安装程序，CDB 支持。

# TIPS

## QTimer
### 标准模板
```
timer = new QTimer(this);

connect(timer,&QTimer::timeout,
        this,&MainWindow::slotFunc);

timer->start(1000); // 默认循环触发，每1s调用一次超时函数

timer->stop(); // 定时器停止
```

### 延迟初始化 & 反初始化

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

### 防抖

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

## 打印调试
```
// MessageBox 打印
QMessageBox::information(this, "Title", "Open Button");
```

## 便捷输入框

```
bool ok;
QString newName = QInputDialog::getText(this, "重命名",
                                        "新名称:", QLineEdit::Normal, item->text(0), &ok);
if (ok && !newName.trimmed().isEmpty()) {
    item->setText(0, newName.trimmed());
}
```

##  QStringList 转 QString
```
QStringList currentList = model_->stringList();
QString text = currentList.join("\n"); // 每项一行
```

## 窗口固定尺寸，禁止窗口最大化最小化

```
setWindowFlags(windowFlags() & ~Qt::WindowMinMaxButtonsHint);
setFixedSize(size());
```

# QT 基础

## 创建 QT 工程

| 对比项     | QWidget | QDialog | QMainWindow |
| ------- | ------- | ------- | ----------- |
| 是否有菜单栏  | ❌       | ❌       | ✅           |
| 是否有工具栏  | ❌       | ❌       | ✅           |
| 是否有状态栏  | ❌       | ❌       | ✅           |
| 是否有中央区域 | ❌       | ❌       | ✅           |
| 是否支持模态  | ❌       | ✅       | ❌           |
| 典型用途    | 普通窗口    | 弹窗      | 主界面         |

## Q_PROPERTY  & Q_OBJECT

```
Q_PROPERTY(QString text         // 声明 QString text 变量
            READ text           // 读取函数（必须存在）
            WRITE setText       // 写入函数（可选，省略则为只读）
            NOTIFY textChanged  // 变更信号（可选，QML绑定时强烈建议加）
            MEMBER m_text       // 替代 READ/WRITE：直接绑定成员变量（简单场景可用）
            CONSTANT            // 标记为常量，无 WRITE 无 NOTIFY
            FINAL               // 禁止子类覆盖此属性
        )
```

```
class MyLabel : public QObject
{
    Q_OBJECT
    // ✅ 声明属性
    Q_PROPERTY(QString text READ text WRITE setText NOTIFY textChanged)

public:
    explicit MyLabel(QObject *parent = nullptr);

    // READ 函数
    QString text() const;

    // WRITE 函数
    void setText(const QString &newText) {
        if (m_text == newText)
            return;

        m_text = newText;
        emit textChanged(m_text);
    }

signals:
    // NOTIFY 信号（参数类型应与属性类型一致）
    void textChanged(const QString &text);

private:
    QString m_text;  // 实际存储的成员变量
};
```

C++ 中动态访问(反射)

```
MyLabel label;

// 通过字符串名称读写属性
label.setProperty("text", "动态设置的文本");
QString val = label.property("text").toString();
```

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
void MainWindow::closeSubWindow() {
    subWindow->deleteLater();
}
```

```
// 异步线程
QThread* thread = new QThread;
connect(thread, &QThread::finished, thread, &QObject::deleteLater);

```

```
// 异步网络编程
QTcpSocket* socket = new QTcpSocket;
connect(socket, &QTcpSocket::disconnected, socket, &QObject::deleteLater);
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

```
// 窗口布局
MainWindow::MainWindow() {
    //  QMainWindow 不支持 QGridLayout， 如果使用布局建议使用中央控件的方式
    QWidget* centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    QWidget* gridArea = new QWidget(centralWidget);
    QGridLayout* gridLayout = new QGridLayout(gridArea);

    gridLayout->addWidget(cmbb_, 0, 0);
    gridLayout->addWidget(cmbb_label_, 0, 1);
    gridLayout->addWidget(ui->font_cmbb, 1, 0);
    gridLayout->addWidget(ui->font_label_, 1, 1);

    // 
    QHBoxLayout* mainLayout = new QHBoxLayout(centralWidget);

    // 垂直方向占 1/3
    QVBoxLayout* vLayout = new QVBoxLayout();
    vLayout->addWidget(gridArea, 1);
    vLayout->addStretch(2);

    // 水方向占 1/4
    mainLayout->addLayout(vLayout, 1);
    mainLayout->addStretch(3);

    // 数据流向： QHBoxLayout <- QVBoxLayout <- QWidget <- QGridLayout
}
```

- QHBoxLayout* btnLayout = new QHBoxLayout(); // 子布局
- QVBoxLayout* mainLayout = new QVBoxLayout(this): // 主布局，内部会自动调用 setLayout

```
void ListWindow::setupUi()
{
    model_->setStringList(QStringList() << "清华大学" << "北京大学" << "浙江大学" << "湖南大学");
    view_->setModel(model_);
    view_->show();

    QPushButton* btnAdd = new QPushButton("插入", this);
    QPushButton* btnDel = new QPushButton("删除", this);
    QPushButton* btnRename = new QPushButton("重命名", this);
    QPushButton* btnFind = new QPushButton("查找", this);
    QPushButton* btnClear = new QPushButton("清除", this);
    QPushButton* btnReLoad = new QPushButton("重新加载", this);

    QHBoxLayout* btnLayout = new QHBoxLayout(); // 这里 new QHBoxLayout(this) 会影响布局，这是为什么？
    btnLayout->addWidget(btnAdd);
    btnLayout->addWidget(btnDel);
    btnLayout->addWidget(btnRename);
    btnLayout->addWidget(btnFind);
    btnLayout->addWidget(btnClear);
    btnLayout->addWidget(btnReLoad);

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(view_);
    mainLayout->addLayout(btnLayout);
    setLayout(mainLayout);
}
```

| 函数 | 作用 | 是否影响布局 |
|------|-----|-------------|
| sizeHint | 理想尺寸（布局优先参考） | 是 |
| minimumSizeHint |最小合理尺寸（比 setMinimumSize 更智能） | 是 |
| setMinimumSize(w, h) | 强制最小尺寸 | 是 |
| setMaximumSize(w, h) | 强制最大尺寸 | 是 |
| resize(w, h) | 直接设置当前尺寸 | 不影响布局建议 |

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

# QT控件

| 控件 | 功能 |
|----------|----------|
| QLabel | 标签 | setText / text | 
| QPushButton | 按钮 |
| QComboBox / QFontComboBox | 组合框(下拉列表) |
| QLineEdit | 单行文本编辑 |
| QTextEdit | 多行文本编辑 |
| QCheckBox | 复选框(打开 or 关闭) |
| QTableWidget | 表格 |
| QRadioButton | 单选按钮 |
| QListWidget | 列表 |

## QPushButton(按键)

### 常用API
- text(): 获取按键文本
- setText(): 设置按键文本
- setCheckable(bool ): 设置按键开启可以选中模式，比如视频的播放/暂停按键
- setChecked(bool ): 设置按键选中 / 未选中

### 信号
- void pressed(): 按键按下信号，配合定时器可以实现长按功能
- void released(): 按键释放，在按键按下的状态下移动超出按键范围 或者 在按键上抬起按键都会调用
- void clicked(bool checked = false): 在按键上按下，再在当前按键上抬起会发出此信号
- void toggled(bool checked): 开启选中模式后，模式切换时会发出此信号

**信号调用顺序： pressed() → released() → clicked(true) → toggled(true)**

## QComboBox(组合框)

### 常用API

- addItem / addItems: 添加新元素
- currentText: 返回组合框当前的QString
- currentIndex: 返回组合框当前的下标
- itemText: 下标转QString
- setEditable: 设置为可编辑模式

### 信号

- void editTextChanged(const QString &): 仅在编辑模式有效，编辑框输入文本就会触发，每次按键都会触发
- void activated(int index):  只有用户操作时触发，用户选择下拉框的某一项或者编辑框回车后会(点击-回车)触发消息
- void textActivated(const QString &):
- void highlighted(int index): 组合框下拉展开时高亮跟随鼠标的一行
- void textHighlighted(const QString &): 
- void currentIndexChanged(int index): 组合框当前项发生变化就会触发这个消息
- void currentTextChanged(const QString &): 

**编辑时每次按键都会调用: editTextChanged 和 currentTextChanged**

**编辑后回车调用顺序：editTextChanged -> currentTextChanged -> highlighted -> textHighlighted -> currentIndexChanged -> activated -> textActivated**

## QLineEdit(单行文本编辑)

### 常用API

- text:
- setText:
- QValidator: 对编辑框的输入数据做限制
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
 
### 信号
- void textChanged(const QString &): 文本发生任何变化(用户编辑&代码触发都)
- void textEdited(const QString &): 用户编辑文本时，每一个按键都会触发这个消息
- void cursorPositionChanged(int, int): 光标位置发生变化触发这个消息
- void returnPressed(): 用户按下return/enter按键
- void editingFinished(): 按下enter键触发消息; 文本发生变化，移动到其他控件也会触发这个消息
- void selectionChanged(): 选中文本发生变化，比如：选中2个字符，取消选中等操作。
- void inputRejected(): 设置QValidator后，用户输入数据不符合限制规则后会触发这个消息。

## QTextEdit(多行文本边界)

### 常用AP

- toPlainText: 获取纯文本，忽略所有格式
- toHtml: 获取HTML格式的文本
- setText: 设置文本

```
// 判断是否为空，编辑框内可能只有空格或换行使用trimmed过滤掉
if (!textEdit->toPlainText().trimmed().isEmpty()) { ... }
```

### 信号

- void textChanged(): 文本发生变化触发此消息，每一个按键都会触发
- void selectionChanged(): 选中文本发生变化触发此消息
- void cursorPositionChanged(): 输入光标位置变化触发此消息
- void copyAvailable(bool b):
- void undoAvailable(bool b):
- void redoAvailable(bool b):
- void currentCharFormatChanged(const QTextCharFormat &format):

## QCheckBox(复选框)

### 常用API

- text: 显示的文本
- setText: 设置显示的文本
- setTristate: 开启三态，支持半选
- setCheckState：设置复选框的状态，状态分为:Qt::Unchecked(未选), Qt::PartiallyChecked(半选)，Qt::Checked(已选)

### 信号

- void checkStateChanged(Qt::CheckState): 复选框状态变更时会触发此消息

## 树形图: QTreeView + QStandardItemModel (标准模型) / QTreeWidget

```
void MainWindow::CreateDeviceView() {
    // 创建模型和视图
    device_list_model_ = new QStandardItemModel(device_view_);
    device_list_ = new QTreeView(device_view_);
    device_list_->setModel(device_list_model_);
    device_list_->setHeaderHidden(true);

    device_info_model_ = new QStandardItemModel(device_view_);
    // device_info_model_->setColumnCount(2);
    // device_info_model_->setHorizontalHeaderLabels({"Property", "Value"});
    device_info_ = new QTreeView(device_view_);
    device_info_->setModel(device_info_model_);
    device_info_->setHeaderHidden(true);

    // 创建布局
    QVBoxLayout* layout = new QVBoxLayout(device_view_);
    layout->addWidget(toolbar);
    layout->addWidget(device_list_);
    layout->addWidget(device_info_);
    layout->setContentsMargins(0, 0, 0, 0); // 移除边距
    layout->setSpacing(0);                  // 移除间距

    // 设置 QTreeView 的 sizePolicy，使其可以拉伸
    device_list_->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);

    // 信号槽
    connect(device_list_->selectionModel(), &QItemSelectionModel::currentChanged, this, &MainWindow::OnDeviceSelected);
}

// 模型中导入数据
void MainWindow::PopulateDeviceList() {
    device_list_model_->clear(); // 清空旧数据

    auto devices = EnumerateDevices();
    for (const auto& dev : devices) {
        QStandardItem* item = new QStandardItem(dev.name);
        item->setEditable(false);
        item->setData(QVariant::fromValue(dev), Qt::UserRole); // 关键：保存完整设备信息
        device_list_model_->appendRow(item);
    }

    device_list_->expandAll();
}

// 信号槽函数中取出数据显示
void MainWindow::OnDeviceSelected(const QModelIndex& current, const QModelIndex& previous) {
    Q_UNUSED(previous);

    device_info_model_->clear();
    device_info_model_->setColumnCount(2);

    if (!current.isValid())
        return;

    // 从模型项中取出设备信息
    QStandardItem* item = device_list_model_->itemFromIndex(current);
    if (!item)
        return;

    QVariant data = item->data(Qt::UserRole);
    if (!data.canConvert<Device*>())
        return;

    Device* dev = data.value<Device*>();

    QStandardItem* interface_info = new QStandardItem("Interface Info");
    const QMap<QString, QString>& props = dev->GetProperty();
    for (auto it = props.cbegin(); it != props.cend(); ++it) {
        interface_info->appendRow( { new QStandardItem(it.key()) ,  new QStandardItem(it.value())} );
    }
    device_info_model_->appendRow(interface_info);

    // 展开并调整列宽
    device_info_->expandAll();
    device_info_->resizeColumnToContents(0);
    device_info_->resizeColumnToContents(1);
}

```

## 树形图 QTreeWidget
```
void FeatureTreeWidget::SetupUi()  {
    ...
    // 2.树形图
    // 2.1. 创建树形图
    feature_tree_ = new QTreeWidget(this);
    // feature_tree->setHeaderLabels({"Feature", ""}); // 第二列放控件
    feature_tree_->setHeaderHidden(true);
    feature_tree_->setColumnCount(2);

    feature_tree_->header()->setSectionResizeMode(0, QHeaderView::Stretch);  // 第一列自适应
    feature_tree_->header()->setSectionResizeMode(1, QHeaderView::Fixed);    // 第二列固定宽度
    feature_tree_->header()->resizeSection(1, 120);                          // 第二列列宽 120
    feature_tree_->header()->setStretchLastSection(true);                    // 最后一列自动拉伸
    feature_tree_->setAlternatingRowColors(false);                           // 斑马纹

    // 2.2. 获取所有 Feature,添加叶子节点
    FeatureWidgetFactory factory(feature_tree_);
    feature_manager_ = FeatureManager::CreateFeatureManager();
    for (FeatureControl* feature_control : feature_manager_->GetFeatureControl()) {
        QTreeWidgetItem* control_item = new QTreeWidgetItem(feature_tree_, {feature_control->GetName()});
        control_item->setData(0, Qt::UserRole, QVariant::fromValue(feature_control));
        control_item->setExpanded(true);
        control_item->setFirstColumnSpanned(true);

        for (Feature* feature : feature_control->GetFeature()) {
            QTreeWidgetItem* feature_item = new QTreeWidgetItem(control_item, {feature->GetName()});
            feature_item->setData(0, Qt::UserRole, QVariant::fromValue(feature));
            QWidget* feature_widget = factory.CreateFeatureWidget(*feature);
            if (feature_widget) {
                feature_tree_->setItemWidget(feature_item, 1, feature_widget);
            }
        }
    }

    connect(feature_tree_, &QTreeWidget::currentItemChanged, this, &FeatureTreeWidget::OnFeatureSelected);
    ...
}

// 叶子节点取数据，可视化
void FeatureTreeWidget::OnFeatureSelected(QTreeWidgetItem *current, QTreeWidgetItem *previous) {
    Q_UNUSED(previous);

    QVariant data = current->data(0, Qt::UserRole);

    FeatureBase* feature_base = nullptr;
    if (data.canConvert<Feature*>()) {
        feature_base  = data.value<Feature*>();
    }
    if (data.canConvert<FeatureControl*>()) {
        feature_base = data.value<FeatureControl*>();
    }
    if (!feature_base) {
        qDebug() <<  "get feature base failed..";
        return;
    }
    feature_info_->setText(feature_base->GetString());
}

// 查找叶子节点
void FeatureTreeWidget::OnSearch() {
    QString keyword = search_edit_->text().trimmed();
    if (keyword.isEmpty()) {
        return;
    }

    QList<QTreeWidgetItem*> items = feature_tree_->findItems(keyword, Qt::MatchContains | Qt::MatchRecursive, 0);
    if (items.isEmpty()) {
        QMessageBox::information(this, "查找结果", "未找到匹配项。");
    } else {
        QTreeWidgetItem* first = items.first();
        feature_tree_->setCurrentItem(first);
        feature_tree_->scrollToItem(first, QAbstractItemView::PositionAtCenter);
        feature_tree_->setFocus();
    }
}

```


## 手绘控件

```
CircleButton2::CircleButton2(const QString& text, QWidget *parent)
    : QWidget{parent}
    , m_text(text)
    , m_hoverFactor(0.0)
    , m_isPressed(false)
    , m_hoverAnim(nullptr)
{
    setFixedSize(QSize(80, 80));
    setMouseTracking(true);

    setFocusPolicy(Qt::StrongFocus); // 可以获得焦点： 可以点击 / 可以使用TAB切换到控件

    m_hoverAnim = new QPropertyAnimation(this, "hoverFactor", this);
    m_hoverAnim->setDuration(2000);
    m_hoverAnim->setEasingCurve(QEasingCurve::InOutQuad); // 平滑加减速
}

class CircleButton2 : public QWidget
{
    Q_OBJECT

    Q_PROPERTY(QString text READ text WRITE setText NOTIFY textChanged)
    
    // 外部通过运行时反射机制修改变量，0.0=Normal, 1.0=Hovered 的连续插值因子
    Q_PROPERTY(qreal hoverFactor READ hoverFactor WRITE setHoverFactor)

public:
    explicit CircleButton2(const QString& text = "", QWidget *parent = nullptr);

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

    void changeEvent(QEvent *) override;
};

```

# Qt 动画框架

## QPropertyAnimation 

在指定时间内，对 QObject 的某个 Q_PROPERTY 属性进行平滑插值。它不直接操作 UI，它只负责“按时间线修改一个数值”，UI 的变化是这个数值变化后触发重绘的自然结果。

运行时流程：

0. QPropertyAnimation 初始化时，绑定指定的属性
1. start() → 动画引擎注册到全局时间轴
2. 每帧（通常 16ms/60FPS）：引擎根据 easing curve 计算当前插值
3. 调用属性的 WRITE 函数（如 setHoverFactor(0.35)）
4. 属性的 setter 中调用 update() → 触发重绘
5. 到达终点 → 停止，发出 finished() 信号

缓动曲线效果: 

| 类别 | 推荐曲线 | 适用场景 | 视觉感受 |
| :--- | :--- | :--- | :--- |
| 通用过渡 | `InOutQuad` / `InOutCubic` | Hover、展开收起、颜色渐变 | 自然平滑，最安全的选择 |
| 进入/出现 | `OutBack` / `OutElastic` | 弹窗、元素入场 | 有过冲/回弹，活泼感 |
| 退出/消失 | `InBack` / `InQuad` | 关闭、隐藏 | 加速离开，干脆利落 |
| 强调/提醒 | `OutBounce` | 错误提示、通知 | 弹跳效果，吸引注意力 |
| 匀速 | `Linear` | 进度条、旋转加载 | 机械感，无加减速 |
| 自定义 | `Custom` + `setCustomType()` | 品牌专属动效 | 完全控制贝塞尔控制点 |

基本用法：

```
// 1. 基础配置
auto *anim = new QPropertyAnimation(target, "propertyName", parent);

anim->setDuration(300);              // 持续时间(ms)
anim->setStartValue(0.0);            // 起始值（可选，省略则取当前属性值）
anim->setEndValue(1.0);              // 结束值（必须设置）
anim->setEasingCurve(QEasingCurve::InOutQuad); // 缓动曲线
anim->setLoopCount(1);               // 循环次数，-1 为无限循环

// 2. 控制方法:
anim->start();                       // 开始（若已在运行，从头重启）
anim->stop();                        // 立即停止，停在当前位置
anim->pause();                       // 暂停
anim->resume();                      // 恢复
anim->setCurrentTime(150);           // 跳转到指定时间点（用于预览/调试）

// 3. 信号
connect(anim, &QPropertyAnimation::finished, [](){
    qDebug() << "动画完成";
});
connect(anim, &QPropertyAnimation::valueChanged, [](const QVariant &val){
    qDebug() << "当前值:" << val;  // 每帧触发，可用于调试
});
```

进阶用法:

```
// 1. 组合多个动画
// 1.1.顺序执行
auto *seq = new QSequentialAnimationGroup(this);
seq->addAnimation(moveAnim);
seq->addAnimation(fadeAnim);
seq->start();

// 1.2.并行执行
auto *par = new QParallelAnimationGroup(this);
par->addAnimation(moveAnim);
par->addAnimation(fadeAnim);
par->start();

// 1.3.嵌套：先并行移动+缩放，再淡出
auto *outer = new QSequentialAnimationGroup(this);
auto *inner = new QParallelAnimationGroup(outer);
inner->addAnimation(moveAnim);
inner->addAnimation(scaleAnim);
outer->addAnimation(inner);
outer->addAnimation(fadeAnim);

// 2.设置关键帧
anim->setKeyValueAt(0.0, 0.0);     // 起点
anim->setKeyValueAt(0.3, 0.8);     // 30% 时间时值为 0.8（快速上升）
anim->setKeyValueAt(0.7, 0.9);     // 70% 时间时值为 0.9（缓慢逼近）
anim->setKeyValueAt(1.0, 1.0);     // 终点

// 3. 自定义插值类型
MyStruct myStructInterpolator(const MyStruct &from, const MyStruct &to, qreal progress) {
    MyStruct result;
    result.value = from.value + (to.value - from.value) * progress;
    return result;
}

```

# QT 对话框

## 常用API

- show() / showNormal() / showMaximized() / showFullScreen(): 对话框以非模态方式显示
- exec(): 对话框以模态方式显示
- accept(): 接受对话框，触发finished(Accepted)信号, 返回 QDialog::Accepted
- reject(): 拒绝对话框，触发finished(reject)信号, 返回 QDialog::Rejected
- done(int r)： 返回r，关闭对话框，等价于: reject()
- raise(): 窗口放到最前端(z-order最前)
- activateWindow(): 窗口获得焦点
- resize(): 设置窗口宽、高
- setGeometry(): 设置窗口的显示位置和窗口宽高(建议使用布局替代)

## 自定义信号槽

1. 定义信号
```
class LoginDialog : public QDialog {
    Q_OBJECT
public:
    explicit LoginDialog(QWidget *parent = nullptr);
    ~LoginDialog();

    void closeEvent(QCloseEvent *event); // 对话框点击 X 事件 

signals:
    void loginSuccess();    //  登录成功信号
    void loginFailed();     //  登录失败信号
}
```

2. 抛出信号
```
void LoginDialog::on_login_clicked()
{
    if (ui->user->text() == "lc" && ui->passwd->text() == "12345") {
        qDebug() << "登录成功";
        QMessageBox::information(this, "提示", "登录成功");
        emit loginSuccess();  // 发射登录成功的信号
        accept(); // 接受对话 框，返回 QDialog::Accepted
    }
    else {
        qDebug() << "用户名或密码错误";
        QMessageBox::warning(this, "提示", "用户名或密码错误");
        // 为什么用户名和密码错误这个也会返回到主窗口呢？
    }
}

void LoginDialog::on_exit_clicked()
{
    qDebug() << "退出";
    emit loginFailed(); // 发射登录失败的信号
    reject(); // 拒绝对话框，返回 QDialog::Rejected
}

// 用户点 X 关闭也视为取消
void LoginDialog::closeEvent(QCloseEvent *event)
{
    int ret = QMessageBox::question(this, "确认", "确定要退出吗？");
    if (ret == QMessageBox::Yes) {
        emit loginFailed();
        reject();
    } else {
        event->ignore();
    }
}

```

3. 父窗口连接信号槽
```
MainWindow::MainWindow(QWidget *parent)
{
    login_label_ = new QLabel("登录对话框", this);
    btn_login_ = new QPushButton("登录", this);
    login_resp_ = new QLabel(this);

    connect(login_dialog_, &LoginDialog::loginSuccess, this, [this] () {
        login_resp_->setText("登录成功信号");
        login_dialog_->hide();
    });
    connect(login_dialog_, &LoginDialog::loginFailed, this, [this] () {
        login_resp_->setText("登录失败信号");
        login_dialog_->hide();
    });
}
```

# 菜单栏 & 工具栏 & 状态栏

## QMainWindow  菜单栏 & 工具栏 & 状态栏

```
void MainWindow::setupUI() {
    // 1. 菜单栏
    QMenu* file = menuBar()->addMenu(tr("File"));
    file->addAction(...);

    // 2. 工具栏
    QToolBar* toolbar = addToolBar(tr("Main"));
    toolbar->addAction(...);

    // 3. 客户端设置布局
    QWidget* central = new QWidget(this);
    setCentralWidget(central);

    QHBoxLayout* layout = new QHBoxLayout(central);
    ....
    central->setLayout(layout);

    // 4. 状态栏
    statusBar()->showMessage(tr("Ready")， 2000); // 2s后消失

    // 5. 进度条
    QProgressBar* progressbar_ = new QProgressBar(this);
    progressbar_->setRange(0, 100);
    progressbar_->setValue(0);
    statusBar()->addPermanentWidget(progressbar_);  // 状态栏右侧添加一个永久控件，它不会被showMessage的消息遮挡着
    // addWidget(QWidget *widget)                   // 状态栏左侧添加一个控件，优先级低于临时消息

    connect(&process_timer_,&QTimer::timeout, this, [this] () {
        qDebug() << "process timer: timeout";
        int value = progressbar_->value();
        if (value >= 100) {
            process_timer_.stop();
            statusBar()->removeWidget(progressbar_);
            status_label_->setText("状态栏标签： 已完成");
        }
        else {
            qDebug() << "--- set process bar --" ;
            progressbar_->setValue(value + 10);
        }
    });

    process_timer_.start(1000); // 默认循环触发，每1s调用一次超时函数

}

```

## QWidget 菜单栏 & 工具栏 & 状态栏

```
void ScannerWindow::setupUI() {
    // 0. 行动项
    QAction* save = new QAction(tr("Save"), this); // 菜单栏和工具栏共有的行动项

    // 1. 菜单栏
    QMenuBar* menubar = new QMenuBar(this);

    QMenu* file = menubar->addMenu(tr("File"));
    QAction* open = file->addAction(tr("Open"));
    QMenu* openRecent = file->addMenu("Open Recent");
    QAction* recentFile = openRecent->addAction(tr("/home/face.png"));
    QAction* close = file->addAction(tr("Close"));
    file->addAction(save);

    QMenu* view = menubar->addMenu(tr("View"));
    QMenu* dispMethod = view->addMenu(tr("Display Method"));
    dispMethod->setEnabled(false);

    // 2. 工具栏
    QToolBar* toolbar = new QToolBar(tr("Scanner"), this);

    QAction* startScan = toolbar->addAction(tr("Start Scan"));
    QAction* stopScan = toolbar->addAction(tr("Stop Scan"));
    toolbar->addAction(save);

    connect(startScan, &QAction::triggered, this, &ScannerWindow::onStartScan);
    connect(stopScan, &QAction::triggered, this, &ScannerWindow::onStopScan);
    connect(save, &QAction::triggered, this, &ScannerWindow::onSave);

    // 3. 状态栏
    m_statusBar = new QStatusBar(this);
    m_statusBar->showMessage(tr("Ready To Scan"));

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0); // 移除边距
    layout->setSpacing(0);                  // 移除间距
    layout->addWidget(menubar);
    layout->addWidget(toolbar);
    layout->addWidget(m_scanner);
    layout->addWidget(m_statusBar);
    setLayout(layout);
}
```

## 行动项 QAction 配置

```
void MainWindow::CreateActions() {
    // 1. 设置图标、快捷键
    open_act_ = new QAction(tr("&Open"), this);
    open_act_->setIcon(QIcon::fromTheme("document-open"));   // 设置图标 QIcon(":/icons/refresh.png")
    open_act_->setShortcut(QKeySequence::Open);              // 设置快捷键
    save_act_->setEnabled(false);                           // 默认置灰

    QAction* single_screen = screen_menu->addAction(tr("Single Screen"));
    QAction* four_screen = screen_menu->addAction(tr("Four Screen"));
    QAction* nine_screen = screen_menu->addAction(tr("Nine Screen"));

    // 2. 下拉式菜单: QToolButton + QMenu
    QToolButton* screen_layout_ = new QToolButton(this);
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

    // 3. 对勾单选： QActionGroup 实现 互斥 3选1
    QActionGroup* screen_group = new QActionGroup(screen_menu);
    screen_group->setExclusive(true);
    screen_group->addAction(single_screen);
    screen_group->addAction(four_screen);
    screen_group->addAction(nine_screen);

    single_screen->setCheckable(true);              // 开启选择框
    four_screen->setCheckable(true);
    nine_screen->setCheckable(true);
    single_screen->setChecked(true);                // 勾选选择框
}

```

# QEvent 事件

## 常见的QT 事件

```
// 绘图事件
void paintEvent(QPaintEvent *event) override;

// 鼠标移动到控件事件, 触发一次消息，需要先设置: setMouseTracking(true)
void enterEvent(QEnterEvent *event) override;

// 鼠标移出控件事件, 触发一次消息，需要先设置: setMouseTracking(true)
void leaveEvent(QEvent *event) override;

// 鼠标按下事件
void mousePressEvent(QMouseEvent *event) override;

// 鼠标抬起事件
void mouseReleaseEvent(QMouseEvent *event) override;

// 按键按下事件
void keyPressEvent(QKeyEvent *event) override;

// 按键抬起事件
void keyReleaseEvent(QKeyEvent *event) override;

// 获得焦点事件
void focusInEvent(QFocusEvent *event) override;

// 失去焦点事件
void focusOutEvent(QFocusEvent *event) override;

// 
void changeEvent(QEvent *) override;

// 获取理想尺寸事件
QSize sizeHint() const override;

// 控件尺寸变化事件
void resizeEvent(QResizeEvent *event) override;

// 拖拽文件进入控件事件, 需要先设置: setAcceptDrops(true);
void dragEnterEvent(QDragEnterEvent *event) override;
    
// 拖拽文件离开控件事件, 需要先设置: setAcceptDrops(true);
void dragLeaveEvent(QDragLeaveEvent *event) override;

// 拖拽文件在控件内放下事件，需要先设置: setAcceptDrops(true);
void dropEvent(QDropEvent *event) override;

// 鼠标进入控件事件, 连续触发消息， 需要先设置: setMouseTracking(true)
void mouseMoveEvent(QMouseEvent *event) override;
```

## Qt事件的消息处理

消息分发的顺序：

```
QCoreApplication::notify()
    │
    ├─ 1. 事件过滤器 (installEventFilter)     ← 最高优先级，可拦截一切
    │
    ├─ 2. 目标对象的 event() 函数             ← 虚函数，可按 type 分发
    │
    ├─ 3. 具体事件处理器 (mousePressEvent等)  ← 你最常重写的地方
    │
    └─ 4. 若未 accept → 向父级冒泡           ← 仅针对输入事件
```

事件过滤器：

```
// 在 MainWindow 中拦截所有子控件的鼠标按下
MainWindow::MainWindow() {
    ui->lineEdit->installEventFilter(this);
    ui->pushButton->installEventFilter(this);
}

bool MainWindow::eventFilter(QObject *obj, QEvent *event) {
    if (event->type() == QEvent::MouseButtonPress) {
        qDebug() << "拦截到鼠标按下:" << obj->objectName();
        return true;  // ✅ 返回 true = 吞掉事件，不再传递给目标对象
    }
    return QObject::eventFilter(obj, event); // ❌ 返回 false = 放行
}
```

> 为什么用事件过滤器而不是继承？ 
> > 当你需要给一批第三方控件或动态创建的控件添加统一行为（如全局快捷键、日志记录、输入验证），而不想为每种控件写子类时，事件过滤器是唯一正解。

## 事件与信号槽的选择

| 维度 | 事件 | 信号槽 |
| :--- | :--- | :--- |
| 方向 | 自底向上冒泡（子→父） | 任意连接，无方向限制 |
| 耦合度 | 紧耦合（必须知道目标类） | 松耦合（只需知道信号签名） |
| 跨线程 | ❌ 不能跨线程投递 | ✅ 支持 QueuedConnection |
| 返回值 | 可通过 accept/ignore 反馈 | 单向通知，无反馈 |
| 典型用途 | 控件自身行为、父子交互 | 业务逻辑、模块间通信 |

- 控件内部的视觉/交互反馈 → 事件
- 控件向外部通知状态变化 → 信号
- 父容器统一管理子控件行为 → 事件过滤器
- 跨模块/跨线程通信 → 信号槽

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

# Q&A

1. 下面代码有异常报错: "QDialog::exec: Recursive call detected"  
```
void MainWindow::on_login_clicked(bool checked)
{
    qDebug() << "--- on_login_clicked ---";
    if (login_dialog_->exec() == QDialog::Accepted) {
        qDebug() << "--- 展示登录成功的界面 ---";
    }
    else {
        qDebug() << "--- 展示登录失败的界面 ---";
    }
}
```
A: 模态嵌套问题, login_dialog_ 的父窗口是 MainWindow, login_dialog_ 中使用QMessageBox打印后，消息会返回到MainWindow，导致此槽函数被二次触发调用到: login_dialog_->exec(), 而前一次模态调用还未结束，导致异常

```
#if 0
    // A: 修复方式一: 不要复用对话框资源
    LoginDialog dlg;
    if (dlg.exec() == QDialog::Accepted) {
        qDebug() << "--- 展示登录成功的界面 ---";
    }
    else {
        qDebug() << "--- 展示登录失败的界面 ---";
    }
#endif

#if 0
    // A: 修复方式二： isVisible() 判断模态对话框是否显示，如果显示这次就不要exec调用
    if (login_dialog_->isVisible()) {
        qDebug() << "--- login_dialog is visible, return ----";
        return;
    }

    if (login_dialog_->exec() == QDialog::Accepted) {
        qDebug() << "--- 展示登录成功的界面 ---";
    }
    else {
        qDebug() << "--- 展示登录失败的界面 ---";
    }
#endif
```

2. 运行时，设置的工具栏图标没有显示

A: 加入调试信息测试图标是否加载成功，发现图标加载失败

```
qDebug() << "Save icon loaded:" << !icon.isNull();
qDebug() << "File exists in resource:" << QFile::exists(":/res/icon/save32.png");
```

检查 resource.qrc配置的图标目录是否有问题
```
res/icon/continue32.png  // 目录配置没有问题
```

检查 resource.qrc 是否集成到项目中

```
set(CMAKE_AUTORCC ON)  # 自动处理 .qrc 文件
add_executable(your_app
    main.cpp
    resource.qrc     # 必须列出 .qrc 文件
)
```

Q: 按键消息耗时太长，导致UI卡住的优化方式

```
#include <QtConcurrent>
#include <QFutureWatcher>

// 在 .cpp 中
void CPicPreviewDlg::OnBnClickedButtonAutoflip()
{
    if (ui->buttonAutoflip->isEnabled() == false)
        return;

    ui->buttonAutoflip->setEnabled(false); // 按键置灰，防止多次重复按下
    QApplication::setOverrideCursor(Qt::WaitCursor);

    QFuture<void> future = QtConcurrent::run([this]() {
        m_show.AutoFlip(); // 异步调用耗时操作
    });

    auto* watcher = new QFutureWatcher<void>(this);
    connect(watcher, &QFutureWatcher<void>::finished, this, [this, watcher]() {
        // 耗时操作结束后，更新UI, 恢复按钮
        QApplication::restoreOverrideCursor();
        ui->buttonAutoflip->setEnabled(true);
        // updateImageView();
        watcher->deleteLater();
    });
    watcher->setFuture(future);
}
```


# 附录

QIcon QIcon::fromTheme(const QString &name, const QIcon &fallback) 


```
通用操作:
document-new	新建文档
document-open	打开
document-save	保存
document-save-as	另存为
edit-copy	复制
edit-cut	剪切
edit-paste	粘贴
edit-delete	删除
edit-undo	撤销
edit-redo	重做
system-search	搜索
view-refresh	刷新
window-close	关闭窗口

导航与视图:
go-previous	上一页 / 后退
go-next	下一页 / 前进
go-up	上级目录
go-home	主页 / 首页
zoom-in	放大
zoom-out	缩小
zoom-fit-best	最佳缩放
view-list-icons	图标视图
view-list-details	详细列表

网络与设备
network-wired	有线网络
network-wireless	无线网络
network-connect	连接
network-disconnect	断开连接
media-playback-start	播放
media-playback-pause	暂停
media-playback-stop	停止
media-seek-forward	快进
media-seek-backward	快退

应用于系统
application-exit	退出应用
help-about	关于
preferences-system	系统设置
system-run	运行
dialog-ok	确定
dialog-cancel	取消
dialog-warning	警告
dialog-error	错误

```
