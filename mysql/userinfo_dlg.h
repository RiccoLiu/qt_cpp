#ifndef USERINFO_DLG_H
#define USERINFO_DLG_H

#include <QDialog>
#include <QKeyEvent>
#include "database2.h"

namespace Ui {
class UserInfoDlg;
}

class UserInfoDlg : public QDialog
{
    Q_OBJECT

public:
    explicit UserInfoDlg(QWidget *parent = nullptr);
    ~UserInfoDlg();

    User GetUserInfo() const;

protected:
    void keyPressEvent(QKeyEvent *event) override;

private slots:
    void on_photo_finished();

private:
    Ui::UserInfoDlg *ui;
};

#endif // USERINFO_DLG_H
