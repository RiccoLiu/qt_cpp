#include "userinfo_dlg.h"
#include "ui_userinfo_dlg.h"

UserInfoDlg::UserInfoDlg(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::UserInfoDlg)
{
    ui->setupUi(this);

    setWindowTitle("用户信息");

    ui->gender->addItems({"男", "女", "其他"});
    ui->gender->setCurrentIndex(0);

    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
}

UserInfoDlg::~UserInfoDlg()
{
    delete ui;
}

UserInfo UserInfoDlg::GetUserInfo() const {
    UserInfo user;
    user.name = ui->name->text().toStdString();                 // QLineEdit
    user.gender = ui->gender->currentText().toStdString();      // QComboBox
    user.age = ui->age->value();                                // QSpinBox
    user.address = ui->address->toPlainText().toStdString();    // QTextEdit
    user.image = ui->photo->text().toStdString();               // QLineEdit
    return user;
}
