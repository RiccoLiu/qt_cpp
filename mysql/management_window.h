#ifndef MANAGEMENT_WINDOW_H
#define MANAGEMENT_WINDOW_H


#include <QWidget>

#include <QListWidget>
#include <QStackedWidget>

namespace Ui {
class ManagementWindow;
}

struct PageItem {
    QString text;
    QString icon;
    QWidget* page;
};

class ManagementWindow : public QWidget
{
    Q_OBJECT

public:
    explicit ManagementWindow(QWidget *parent = nullptr);
    ~ManagementWindow();

    std::vector<PageItem> CreatePage();

private slots:
    void onNavigationItemChanged(int currentRow);

private:
    Ui::ManagementWindow *ui;

    QListWidget* navList;
    QStackedWidget* stackedWidget;
};

#endif // MANAGEMENT_WINDOW_H
