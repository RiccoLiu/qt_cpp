#ifndef MAINDIALOG_H
#define MAINDIALOG_H

#include <QDialog>

#include <QPushButton>
#include <QLabel>
#include <QLineEdit>

class MainDialog : public QDialog
{
    Q_OBJECT

public:
    MainDialog(QWidget *parent = nullptr);
    ~MainDialog();

private Q_SLOTS:
    void CalCircleArea();

private:
    QPushButton* apply_btn_;
    QLabel* disp_label_;
    QLabel* res_label_;
    QLineEdit* input_edit_;
};
#endif // MAINDIALOG_H
