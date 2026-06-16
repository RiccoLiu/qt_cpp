#ifndef USERMANAGER_WINDOW_H
#define USERMANAGER_WINDOW_H

#include <QWidget>
#include <QTableWidget>
#include <QStandardItemModel>
#include <QTableView>
#include <QLabel>
#include <QSpinBox>

#include "database.h"

namespace Ui {
class UserManagerWindow;
}

class UserManagerWindow : public QWidget
{
    Q_OBJECT

public:
    explicit UserManagerWindow(QWidget *parent = nullptr);
    ~UserManagerWindow();

    int LayoutInit();
    int TableInit();
    int UpdateUserCount();

private slots:
    void on_query_clicked();
    void on_delete_clicked();
    void on_update_clicked();
    void on_insert_clicked();
    void on_page_changed(int);

private:
    Ui::UserManagerWindow *ui;

    std::unique_ptr<Database> db;

    QStandardItemModel* model;
    QTableView* view;
    int tableRowCount;

    int userCount;
    int pageCount;

    QLabel* totalPage;
    QSpinBox* currnetPage;
};

#endif // USERMANAGER_WINDOW_H
