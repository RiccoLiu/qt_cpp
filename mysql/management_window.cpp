#include "management_window.h"
#include "ui_management_window.h"
#include "usermanager_window.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QMainWindow>

class DashboardPage : public QWidget {
    Q_OBJECT

public:
    DashboardPage(QWidget *parent = nullptr) {
        setWindowTitle("仪表盘页面");
    }
    virtual ~DashboardPage() {}
};

class UserManagePage : public QWidget {
    Q_OBJECT

public:
    UserManagePage(QWidget *parent = nullptr) {
        setWindowTitle("用户管理页面");
    }
    virtual ~UserManagePage() {}
};


class SystemManagePage : public QWidget {
    Q_OBJECT

public:
    SystemManagePage(QWidget *parent = nullptr) {
        setWindowTitle("系统管理页面");
    }
    virtual ~SystemManagePage() {}
};

class LogViewPage : public QWidget {
    Q_OBJECT

public:
    LogViewPage(QWidget *parent = nullptr) {
        setWindowTitle("日志查看页面");
    }
    virtual ~LogViewPage() {}
};


std::vector<PageItem> ManagementWindow::CreatePage() {
    std::vector<PageItem> page = {
        {"仪表盘", "", new DashboardPage},
        {"用户管理", "", new UserManagePage},
        {"系统设置", "", new SystemManagePage},
        {"日志查看", "", new LogViewPage},
        {"数据库管理", "", new UserManagerWindow}
    };
    return page;
};


// QListWidget + QStackedWidget 实现分页
ManagementWindow::ManagementWindow(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ManagementWindow)
{
    ui->setupUi(this);

    setWindowTitle("管理端");

    navList = new QListWidget();
    navList->setFixedWidth(200);                // 设置固定宽度
    navList->setStyleSheet(R"(
        QListWidget {
            background-color: #2c3e50;
            border: none;
            color: #ecf0f1;
            font-size: 14px;
        }
        QListWidget::item {
            padding: 12px 20px;
            border-bottom: 1px solid #34495e;
        }
        QListWidget::item:selected {
            background-color: #3498db;
            color: white;
        }
        QListWidget::item:hover {
            background-color: #34495e;
        }
    )");

    stackedWidget = new QStackedWidget();

    std::vector<PageItem> all_page = CreatePage();
    for (int i = 0; i < all_page.size(); i++) {
        QListWidgetItem* item = new QListWidgetItem(all_page[i].text, navList);
        item->setTextAlignment(Qt::AlignVCenter | Qt::AlignLeft);
        if (all_page[i].icon.size() > 0) {
            item->setIcon(QIcon(all_page[i].icon));
        }
        // all_page[i].page->setStyleSheet("background-color: #000000;");
        stackedWidget->addWidget( all_page[i].page);
    }

    connect(navList, &QListWidget::currentRowChanged, this, &ManagementWindow::onNavigationItemChanged);

    QHBoxLayout* mainLayout = new QHBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0); // 客户区留白
    mainLayout->setSpacing(0);                  // 布局内相邻子控件的间距
    mainLayout->addWidget(navList);
    mainLayout->addWidget(stackedWidget);
}

ManagementWindow::~ManagementWindow()
{
    delete ui;
}

void ManagementWindow::onNavigationItemChanged(int currentRow) {
    stackedWidget->setCurrentIndex(currentRow);
}


#include "management_window.moc"    //
