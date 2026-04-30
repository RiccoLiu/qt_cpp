#ifndef USERINFO_DLG_H
#define USERINFO_DLG_H

#include <QDialog>
#include "database.h"

namespace Ui {
class UserInfoDlg;
}

class UserInfoDlg : public QDialog
{
    Q_OBJECT

public:
    explicit UserInfoDlg(QWidget *parent = nullptr);
    ~UserInfoDlg();

    UserInfo GetUserInfo() const;

private:
    Ui::UserInfoDlg *ui;
};

#endif // USERINFO_DLG_H
