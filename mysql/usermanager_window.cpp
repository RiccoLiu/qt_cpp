
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QHeaderView>
#include <QMessageBox>

#include "usermanager_window.h"
#include "ui_usermanager_window.h"
#include "userinfo_dlg.h"

#include "db_utils.h"

UserManagerWindow::UserManagerWindow(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::UserManagerWindow)
    , model(new QStandardItemModel(this))
    , view(new QTableView(this))
    , tableRowCount(5)
{
    ui->setupUi(this);

    setWindowTitle("用户管理窗口");

    MySqlConfig mysql_config;
    mysql_config.host = "localhost";
    mysql_config.port = 33060;
    mysql_config.username = "lc";
    mysql_config.passwd = "123456lc";
    mysql_config.schema = "lc_test";

    SqliteConfig sqlite_config;
    sqlite_config.db_file = "user.db";

    db = DatabaseFactory::Create(sqlite_config);

    TableInit();
    LayoutInit();
}

UserManagerWindow::~UserManagerWindow()
{
    delete ui;
}

int UserManagerWindow::UpdateUserCount() {
    userCount = db->GetUserCount();
    pageCount = userCount / tableRowCount + 1;
    totalPage->setText(QString("共 %1 页").arg(pageCount));
    currnetPage->setMinimum(1);
    currnetPage->setMaximum(pageCount);
    return 0;
}

int UserManagerWindow::TableInit() {
    model->clear();

    QStringList h_header;
    h_header << "id" << "name" << "gender" << "age" << "image_path" << "image";

    model->setHorizontalHeaderLabels(h_header);

    QStringList v_header;
    for (int i = 1; i <= tableRowCount; i++) {
        v_header << QString::number(i);
    }
    model->setVerticalHeaderLabels(v_header);
    view->setModel(model);

    view->horizontalHeader()->setStyleSheet(
        "QHeaderView::section {"
        "    background-color: #4A90E2;"      // 蓝色表头
        "    color: white;"
        "    padding: 4px;"
        "    font-weight: bold;"
        "    border: 1px solid #346789;"
        "}"
        );

    view->verticalHeader()->setStyleSheet(
        "QHeaderView::section {"
        "    background-color: #F0F0F0;"
        "    padding: 2px;"
        "    border: 1px solid #D0D0D0;"
        "}"
        );

    view->setStyleSheet(
        "QTableView {"
        "    gridline-color: #D0D0D0;"
        "    selection-background-color: #CCE5FF;"
        "    font-size: 12px;"
        "    alternate-background-color: #FAFAFA;"
        "}"
        );

    view->setAlternatingRowColors(true);                        // 启用隔行变色
    /*
        enum SelectionBehavior {
            SelectItems,            // 单个单元格选中
            SelectRows,             // 整行单元格选中
            SelectColumns           // 整列单元格选中
        };
    */
    view->setSelectionBehavior(QAbstractItemView::SelectRows);

    /*
        enum SelectionMode {
            NoSelection,            // 用户无法选中任何项（点击无反应）
            SingleSelection,        // 用户只能选中一个项目
            MultiSelection,         // 用户可以多选 (ctrl 单选, shift 连续多选)
            ExtendedSelection,      // 用户可以多选
            ContiguousSelection     // 仅允许连续多选 ()
        };
    */
    view->setSelectionMode(QAbstractItemView::SingleSelection);

    /*
        enum EditTrigger {
            NoEditTriggers = 0,     // 禁止任何方式触发编辑
            CurrentChanged = 1,     // 当前选中的单元格改变时触发编辑
            DoubleClicked = 2,      // 双击单元格触发编辑
            SelectedClicked = 4,    // 选中单元格，再次单击触发编辑
            EditKeyPressed = 8,     // 选中单元格，按下编辑键(F2)触发编辑
            AnyKeyPressed = 16,     // 选中单元格，按下任意键触发编辑
            AllEditTriggers = 31    // 以上任意一种模式都会触发进入编辑模式
        };
        QAbstractItemView::DoubleClicked | QAbstractItemView::EditKeyPressed
    */
    view->setEditTriggers(QAbstractItemView::NoEditTriggers);

    // 根据表头自动调整列宽
    for (int col = 0; col < model->columnCount(); ++col) {
        view->horizontalHeader()->setSectionResizeMode(col, QHeaderView::Stretch);
    }
    // 根据表头自动调整行高
    for (int row = 0; row < model->rowCount(); ++row) {
        view->verticalHeader()->setSectionResizeMode(row, QHeaderView::Stretch);
    }

    // view->verticalHeader()->setDefaultSectionSize(30);
    view->show();
    return 0;
}

int UserManagerWindow::LayoutInit() {
    // 功能区布局
    QHBoxLayout *h_layout = new QHBoxLayout();

    QPushButton* btnQuery = new QPushButton("查询", this);
    QPushButton* btnDelete = new QPushButton("删除", this);
    QPushButton* btnUpdate = new QPushButton("更新", this);
    QPushButton* btnInsert = new QPushButton("添加", this);

    h_layout->addWidget(btnQuery);
    h_layout->addWidget(btnDelete);
    h_layout->addWidget(btnUpdate);
    h_layout->addWidget(btnInsert);

    connect(btnQuery, &QPushButton::clicked, this, &UserManagerWindow::on_query_clicked);
    connect(btnDelete, &QPushButton::clicked, this, &UserManagerWindow::on_delete_clicked);
    connect(btnUpdate, &QPushButton::clicked, this, &UserManagerWindow::on_update_clicked);
    connect(btnInsert, &QPushButton::clicked, this, &UserManagerWindow::on_insert_clicked);

    // table分页布局
    QHBoxLayout* page_layout = new QHBoxLayout();

    QPushButton* btnFirstPage = new QPushButton("«");
    QPushButton* btnPrePage = new QPushButton("‹");
    QPushButton* btnNextPage = new QPushButton("›");
    QPushButton* btnLastPage = new QPushButton("»");

    currnetPage = new QSpinBox;
    totalPage = new QLabel();

    page_layout->addWidget(btnFirstPage);
    page_layout->addWidget(btnPrePage);
    page_layout->addWidget(new QLabel("第"));
    page_layout->addWidget(currnetPage);
    page_layout->addWidget(new QLabel("页"));
    page_layout->addWidget(totalPage);
    page_layout->addWidget(btnNextPage);
    page_layout->addWidget(btnLastPage);
    page_layout->addStretch(); // 右侧留空

    UpdateUserCount();

    connect(btnFirstPage, &QPushButton::clicked, this, [this]() {
        currnetPage->setValue(1);
    });
    connect(btnPrePage, &QPushButton::clicked, this, [this]() {
        currnetPage->setValue(currnetPage->value() - 1);
    });
    connect(btnNextPage, &QPushButton::clicked, this, [this]() {
        currnetPage->setValue(currnetPage->value() + 1);
    });
    connect(btnLastPage, &QPushButton::clicked, this, [this]() {
        currnetPage->setValue(pageCount);
    });

    connect(btnFirstPage, &QPushButton::clicked, currnetPage, &QSpinBox::valueChanged);
    connect(btnPrePage, &QPushButton::clicked, currnetPage, &QSpinBox::valueChanged);
    connect(btnNextPage, &QPushButton::clicked, currnetPage, &QSpinBox::valueChanged);
    connect(btnLastPage, &QPushButton::clicked, currnetPage, &QSpinBox::valueChanged);
    connect(currnetPage, &QSpinBox::valueChanged, this, &UserManagerWindow::on_page_changed);

    // 主布局
    QVBoxLayout* v_layout = new QVBoxLayout();
    v_layout->addLayout(h_layout);
    v_layout->addWidget(view);
    v_layout->addLayout(page_layout);
    setLayout(v_layout);

    return 0;
}

void UserManagerWindow::on_page_changed(int page) {

    on_query_clicked();

#if 0
    int row = 0;

    std::vector<User> users = db->QueryUser(UserCondition(), currnetPage->value() - 1, tableRowCount);
    for (const User& user : users) {
        model->setItem(row, 0, new QStandardItem(QString::number(user.id)));
        model->setItem(row, 1, new QStandardItem(QString::fromStdString(user.name)));
        model->setItem(row, 2, new QStandardItem(QString::fromStdString(user.gender == Gender::Male ? "Male" : "Female")));
        model->setItem(row, 3, new QStandardItem(QString::number(user.age)));
        model->setItem(row, 4, new QStandardItem(QString::fromStdString(user.image_path)));
        model->setItem(row, 5, new QStandardItem(QString::fromStdString("无图片")));

        for (int col = 0; col < model->columnCount(); col++) {
            // 文本显示单元格居中
            model->item(row, col)->setTextAlignment(Qt::AlignCenter);
            // model->item(row, col)->setFlags(item->flags() & ~Qt::ItemIsEditable);
        }
        ++row;

        if (row >= model->rowCount()) {
            break;
        }
    }
    // 清空其他行
    for (;row < model->rowCount(); row++) {
        for (int col = 0; col < model->columnCount(); col++) {
            model->setItem(row, col, new QStandardItem());
        }
    }
#endif
}

void UserManagerWindow::on_query_clicked() {
    // model->removeRows(0, model->rowCount());

    int row = 0;
    std::vector<User> users = db->QueryUser(UserCondition(), currnetPage->value() - 1, tableRowCount);

    for (const User& user : users) {
        model->setItem(row, 0, new QStandardItem(QString::number(user.id)));
        model->setItem(row, 1, new QStandardItem(QString::fromStdString(user.name)));
        model->setItem(row, 2, new QStandardItem(QString::fromStdString(user.gender == Gender::Male ? "Male" : "Female")));
        model->setItem(row, 3, new QStandardItem(QString::number(user.age)));
        model->setItem(row, 4, new QStandardItem(QString::fromStdString(user.image_path)));
        model->setItem(row, 5, new QStandardItem(QString::fromStdString("无图片")));

        for (int col = 0; col < model->columnCount(); col++) {
            // 文本显示单元格居中
            model->item(row, col)->setTextAlignment(Qt::AlignCenter);
            // model->item(row, col)->setFlags(item->flags() & ~Qt::ItemIsEditable);
        }
        ++row;

        if (row >= model->rowCount()) {
            break;
        }
    }
    // 清空其他行
    for (;row < model->rowCount(); row++) {
        for (int col = 0; col < model->columnCount(); col++) {
            model->setItem(row, col, new QStandardItem());
        }
    }
}

void UserManagerWindow::on_delete_clicked() {
    QModelIndex index = view->currentIndex();
    if (!index.isValid()) {
        return;
    }
    int row = index.row();

    UserCondition condition;
    condition[UserKey::Id] = model->item(row, 0)->text().toInt();

    db->DeleteUser(condition);
    // model->removeRow(row);

    on_query_clicked();
    UpdateUserCount();
}

void UserManagerWindow::on_update_clicked() {

    QModelIndex index = view->currentIndex();
    if (!index.isValid()) {
        return;
    }
    int row = index.row();
    // UserInfo where_user = {
    //     model->item(row, 0)->text().toStdString(),
    //     model->item(row, 1)->text().toStdString(),
    //     model->item(row, 2)->text().toInt(),
    //     model->item(row, 3)->text().toStdString(),
    //     std::vector<double>(),
    //     model->item(row, 5)->text().toStdString()
    // };

    UserCondition where;
    where[UserKey::Id] = model->item(row, 0)->text().toInt();

    UserInfoDlg dlg(this);
    if (dlg.exec() == QDialog::Accepted) {
        User user = dlg.GetUserInfo();
        if (QMessageBox::question(this, "问题",
                                  QString("确定要更新 \"%1\" 吗？").arg(user.name.c_str()))
            == QMessageBox::Yes) {
            db->UpdateUser(where, ToCondition(user));
            on_query_clicked();
        }
    }
}

void UserManagerWindow::on_insert_clicked() {
    UserInfoDlg dlg(this);
    if (dlg.exec() == QDialog::Accepted) {
        User user = dlg.GetUserInfo();
        if (QMessageBox::question(this, "问题",
                                  QString("确定要添加 \"%1\" 吗？").arg(user.name.c_str()))
                == QMessageBox::Yes) {
            db->AddUser(user);
            on_query_clicked();
            UpdateUserCount();
        }
    }
}


