
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
| QCheckBox | 复选框 |
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

## 树形控件(QTreeView & QTreeWidget)


| 特性 | QTreeWidget | QTreeView |
|------|-------------|----------|
| 架构 | 继承自 QTreeView，但内置了数据存储（基于 QTreeWidgetItem） | 纯 View（视图），必须搭配 Model（模型） 使用 |
| MVC 模式 | ❌ 不符合 MVC（数据和视图紧耦合） | ✅ 严格遵循 Model-View-Controller 架构 |
| 数据类型 | QTreeWidgetItem（每个节点是一个对象）| 任意 QAbstractItemModel 子类（如 QStandardItemModel, QFileSystemModel） |
| 内存管理 | 手动管理 QTreeWidgetItem 生命周期 | 模型自动管理数据，视图只负责显示 |

1. QStandardItemModel 模型

- void QStandardItemModel::setHorizontalHeaderLabels(const QStringList &labels): 设置表头
- void QStandardItemModel::appendRow(QStandardItem *aitem): 模型根目录下添加一个新节点
- QStandardItem *QStandardItemModel::itemFromIndex(const QModelIndex &index): 行索引转换成节点
- QModelIndex QStandardItemModel::indexFromItem(const QStandardItem *item)： 节点转换为行索引
- QStandardItem *QStandardItemModel::invisibleRootItem(): 返回Tree的虚拟根节点
- void QStandardItem::appendRow(QStandardItem *aitem): 节点下添加新节点
- void QStandardItem::appendRow(const QList<QStandardItem*> &aitems): 节点下添加一行新数据
- void QStandardItem::appendRows(const QList<QStandardItem*> &aitems): 节点下添加多行数据，每一行只有一列

```
// 1. 加载模型数据 
void TreeWindow::onLoadDB() {
    model_->clear();
    
    model_->setHorizontalHeaderLabels(QStringList() << "名称" << "类型" << "大小");

    QStandardItem* root = new QStandardItem("/");   // 分配新节点 root
    root->appendRow({
        new QStandardItem("file_0.txt"),
        new QStandardItem("文本文件"),
        new QStandardItem("1 KB")
    });

    root->appendRow({                               // root节点下添加一行数据
        new QStandardItem("file_1.png"),
        new QStandardItem("图像文件"),
        new QStandardItem("2 MB")
    });

    QStandardItem* node_0 = new QStandardItem("node_0");    // 分配新节点 node_0
    node_0->appendRow({
        new QStandardItem("file_1.jpg"),
        new QStandardItem("图像文件"),
        new QStandardItem("2 MB")
    });

    QStandardItem* subnode_0 = new QStandardItem("subnode_0");
    subnode_0->appendRow({
        new QStandardItem("file_1.jpg"),
        new QStandardItem("图像文件"),
        new QStandardItem("2 MB")
    });
    node_0->appendRow(subnode_0);                   // node_0 节点下添加子节点 subnode_0
    root->appendRow(node_0);                        // root 节点下添加子节点 node_0
    model_->appendRow(root);
}
```

2. 增删改查
```
// 1.增加节点
void TreeWindow::onAddNode() {
    QModelIndex currentIndex = tree_view_->currentIndex();
    QStandardItem* parentItem = nullptr;

    if (currentIndex.isValid()) {
        parentItem = model_->itemFromIndex(currentIndex);
    } else {
        parentItem = model_->invisibleRootItem(); // 虚拟根节点
    }
    QList<QStandardItem*> newRow;
    newRow << new QStandardItem("新节点")
           << new QStandardItem("未知")
           << new QStandardItem("0 B");
    parentItem->appendRow(newRow);

    if (parentItem != model_->invisibleRootItem()) {
        tree_view_->expand(model_->indexFromItem(parentItem)); // 父节点展开
    }
}

// 2.删除节点
void TreeWindow::onDeleteNode() {
    QModelIndex index = tree_view_->currentIndex(); // tree view第一次启动默认会将 currentIndex 自动设为第一项, 所以启动后删除键会删除第一个节点
    if (!index.isValid()) {
        QMessageBox::warning(this, "提示", "请先选中一个节点！");
        return;
    }
    // if (!tree_view_->selectionModel()->hasSelection()) { // 如果想要区分用户自己选择还是程序自动选择，可以使用selectionModel()
    //     QMessageBox::warning(this, "提示", "请先选中一个节点！");
    //     return;
    // }

    QStandardItem* item = model_->itemFromIndex(index);
    QStandardItem* parent = item->parent();
    if (!parent) {
        parent = model_->invisibleRootItem();
    }
    parent->removeRow(item->row()); // 删除整行
}

// 3.修改节点
void TreeWindow::onRenameNode() {
    QModelIndex index = tree_view_->currentIndex();
    if (!index.isValid())
        return;

    QString newName = QInputDialog::getText(this, "重命名窗口", "新名称:");
    if (newName.isEmpty()) {
        return;
    }

    // index 和 QStandardItem 只是对应某行列中的一个元素，所以这里只是修改光标选中的一个元素
    // QStandardItem* item = model_->itemFromIndex(index);
    // item->setText(newName);
#if 0
    // 整行修改1： 通过parentItem->child(row, col)->setText() 修改
    int row = index.row();
    QStandardItem* parentItem = model_->itemFromIndex(index)->parent();
    if (!parentItem) {
        parentItem = model_->invisibleRootItem();
    }
    parentItem->child(row, 0)->setText(newName);
    parentItem->child(row, 1)->setText("新类型");
    parentItem->child(row, 2)->setText("999 KB");
#else
    // 
    QModelIndex nameIdx = index.sibling(index.row(), 0);
    model_->setData(nameIdx, newName, Qt::EditRole);

    QModelIndex typeIndex = index.sibling(index.row(), 1);
    model_->setData(typeIndex, "新类型", Qt::EditRole);

    QModelIndex sizeIndex = index.sibling(index.row(), 2);
    model_->setData(sizeIndex, "999 KB", Qt::EditRole);
#endif
}

// 4. 查找节点
QStandardItem* TreeWindow::findItemByText(const QString& text, QStandardItem* parent) {
    if (!parent) {
        parent = model_->invisibleRootItem();
    }

    for (int i = 0; i < parent->rowCount(); ++i) {
        QStandardItem* child = parent->child(i, 0); // 搜索第0列（名称列）
        if (child && child->text() == text) {
            return child;
        }
        QStandardItem* found = findItemByText(text, child);
        if (found) 
            return found;
    }
    return nullptr;
}

void TreeWindow::onFindNode() {
    QString keyword = QInputDialog::getText(this, "查找窗口", "查找节点名称:");
    if (keyword.isEmpty())
        return;

    QStandardItem* search_item = nullptr;
    QModelIndex index = tree_view_->currentIndex();
    if (index.isValid()) {
        search_item = model_->itemFromIndex(index);
    }
    QStandardItem* item = findItemByText(keyword, search_item);
    if (item) {
        QModelIndex search_index = model_->indexFromItem(item);
        tree_view_->setCurrentIndex(search_index);
        tree_view_->expand(search_index);
        tree_view_->scrollTo(search_index);
    } else {
        QMessageBox::information(this, "未找到", "未找到名称为 \"" + keyword + "\" 的节点。");
    }
}

```

2. QTreeView

- QModelIndex currentIndex(): 获取当前选中数据的行索引
- void setEditTriggers(EditTriggers triggers): 设置编辑触发模式
- void setModel(QAbstractItemModel *model): 设置模型
- void expand(const QModelIndex &index): 展开节点索引
- void expandAll()：展开所有节点
- void collapse(const QModelIndex &index): 收缩指定节点
- void collapseAll(): 收缩所有节点
- void show(): 树状图显示
- void setCurrentIndex(const QModelIndex &index):  设置导航或者程序的当前项,当前项被光标选中
- QModelIndex currentIndex(): 键盘导航或者程序设置的当前项
- void scrollTo(const QModelIndex &index, ScrollHint hint = EnsureVisible): 滚动视图，使目标可见

```
// 设置列宽的三种方式
QHeaderView* header = tree_view_->header();

// 方法 1：固定列宽（像素）
// header->resizeSection(0, 200); // 第0列（名称）宽 200px
// header->resizeSection(1, 100); // 第1列（类型）宽 100px
// header->resizeSection(2, 80);  // 第2列（大小）宽 80px

// 方法 2：让某列自动拉伸填充剩余空间（推荐用于最后一列或主列）
header->setSectionResizeMode(0, QHeaderView::Stretch); // 名称列自适应
header->setSectionResizeMode(1, QHeaderView::ResizeToContents);
header->setSectionResizeMode(2, QHeaderView::Interactive);

// 方法 3：全部根据内容自动调整（适合数据量小）
// tree_view_->resizeColumnToContents(0);
// tree_view_->resizeColumnToContents(1);
// tree_view_->resizeColumnToContents(2);
tree_view_->show();
```

3. QTreeWidget & QTreeWidgetItem

## 常用API
- void setHeaderLabels(const QStringList &labels)：设置表格头
- void setColumnCount(int columns)： 设置一共有多少列
- void setAlternatingRowColors(bool enable): 设置奇数行和偶数行显示不同的颜色

- 

启用复选框
```
worldUniversity->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled | Qt::ItemIsSelectable); // 启动复选框
worldUniversity->setCheckState(0, Qt::Unchecked);
```

## 表单控件(TableView / TableWidget)

TableWidget
```
1. 设置表格水平表头、行数、列数
2. 设置单元格编辑状态、单元格内容
3. 单元格编辑后触发 cellChange 信号
4. 单元格如何添加 ComboBox 部件
```

TableView & 


## 列表控件(ListView)

- QString toPlainText(): 获取文本
- void setPlainText(const QString &text): 设置文本

## QPlainTextEdit 控件(文本编辑器、日志视图、代码编辑器等)

功能显示和编辑纯文本的控件。他集成 QAbstractScrollArea 类，可以在其中添加文本并允许用户编辑。常应用于:文本编辑器、日志视图、代码编辑器等应用程序


## QGraphicView 控件

用于显示和编辑图形的控件，比如：显示多个图形项，支持平移、缩放、旋转、选择、拖放等。常用于CAD 软件、地图应用、绘图工具等。

## OpenGLWidget

显示 OpenGL 图形的 Wdiget 控件。

## QSpinBox & QCalendarwidget

QSpinBox: 微调框控件

QCalendarwidget: 日历控件


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

# QT 主窗口

## 菜单栏

```

class Ui_DFScanWindow
{
public:
    QMenuBar *menubar;  // 菜单栏
    QMenu *menuFile;    // 菜单栏-文件
    QAction *open;      // 菜单栏-文件-打开
}

void setupUi(QMainWindow *DFScanWindow) {
    open = new QAction(DFScanWindow);
    open->setObjectName("open");

    menubar = new QMenuBar(DFScanWindow);
    menubar->setObjectName("menubar");
    menubar->setGeometry(QRect(0, 0, 800, 24));
    
    menuFile = new QMenu(menubar);
    menuFile->setObjectName("menuFile");

    DFScanWindow->setMenuBar(menubar);

    menubar->addAction(menuFile->menuAction());
    menuFile->addAction(open);
}

void retranslateUi(QMainWindow *DFScanWindow) {
    open->setText(QCoreApplication::translate("DFScanWindow", "\346\211\223\345\274\200", nullptr));
    menuFile->setTitle(QCoreApplication::translate("DFScanWindow", "\346\226\207\344\273\266", nullptr));
}

```

## 工具栏

```
class Ui_DFScanWindow
{
public:
    QToolBar *toolBar;  // 工具栏
    QAction *startScan; // 工具栏-开始扫描
}


void setupUi(QMainWindow *DFScanWindow) {
    startScan = new QAction(DFScanWindow);
    startScan->setObjectName("startScan");
    QIcon icon1;
    icon1.addFile(QString::fromUtf8(":/res/icon/start32.png"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
    startScan->setIcon(icon1);
    startScan->setMenuRole(QAction::MenuRole::NoRole);

    toolBar = new QToolBar(DFScanWindow);
    toolBar->setObjectName("toolBar");
    DFScanWindow->addToolBar(Qt::ToolBarArea::TopToolBarArea, toolBar);

    toolBar->addAction(startScan);

    startScan->setText(QCoreApplication::translate("DFScanWindow", "\345\274\200\345\247\213", nullptr));
    startScan->setToolTip(QCoreApplication::translate("DFScanWindow", "\345\274\200\345\247\213\346\211\253\346\217\217", nullptr));
}

DFScanWindow::DFScanWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::DFScanWindow)
{
    ui->toolBar->setIconSize(QSize(32, 32)); // 设置图标大小
}
```

## 状态栏

```

int MainWindow::ControlInitialize() {

    // 8.1. 状态栏显示临时消息
    statusBar()->showMessage("未登录");
    // statusBar()->showMessage("未登录", 5000);  // 5秒后消失

    // 8.2. 添加一个永久标签
    status_label_ = new QLabel("状态栏标签");
    status_label_->setFrameStyle(QFrame::Panel | QFrame::Sunken);
    statusBar()->addPermanentWidget(status_label_);

    status_label_->setText("状态栏标签:");

    // 8.3 进度条
    progressbar_ = new QProgressBar(this);
    progressbar_->setRange(0, 100);
    progressbar_->setValue(0);
    statusBar()->addPermanentWidget(progressbar_);

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

## QAction槽函数

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

# 试题

