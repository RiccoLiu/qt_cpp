#include "userinfo_dlg.h"
#include "ui_userinfo_dlg.h"

UserInfoDlg::UserInfoDlg(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::UserInfoDlg)
{
    ui->setupUi(this);


    QRegularExpression regExp("^[0-9]*$"); // 只允许数字，可为空
    ui->user_id->setValidator(new QRegularExpressionValidator(regExp, this));

    setWindowTitle("用户信息");

    setWindowFlags(windowFlags() & ~Qt::WindowMinMaxButtonsHint);
    setFixedSize(size());

    ui->gender->addItems({"男", "女", "其他"});
    ui->gender->setCurrentIndex(0);

    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);

    connect(ui->photo, &QLineEdit::editingFinished, this, &UserInfoDlg::on_photo_finished);
}

UserInfoDlg::~UserInfoDlg()
{
    delete ui;
}

User UserInfoDlg::GetUserInfo() const {
    User user;
    user.id = ui->user_id->text().toInt();                          //
    user.name = ui->name->text().toStdString();                 // QLineEdit
    user.gender = ui->gender->currentIndex() == 0 ? Gender::Male : Gender::Female;
    user.age = ui->age->value();                                // QSpinBox
    user.image_path = ui->photo->text().toStdString();               // QLineEdit
    return user;
}

void UserInfoDlg::on_photo_finished() {
    QString img_path = ui->photo->text();
    if (img_path.isEmpty()) {
        ui->img_preview->clear();
        ui->img_preview->setText("无图片");
        return;
    }

    QPixmap pixmap(img_path);
    if (pixmap.isNull()) {
        ui->img_preview->setText("图片加载失败");
        return;
    }
    const int maxWidth = 200;
    const int maxHeight = 200;
    if (pixmap.width() > maxWidth || pixmap.height() > maxHeight) {
        pixmap = pixmap.scaled(maxWidth, maxHeight, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    }

    ui->img_preview->setPixmap(pixmap);
    ui->img_preview->setText(""); // 清除提示文字
}

void UserInfoDlg::keyPressEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return) {
        return;
    }
    QDialog::keyPressEvent(event);
}
