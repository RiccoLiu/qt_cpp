#ifndef SEARCHBAR_H
#define SEARCHBAR_H

#include <QWidget>

#include <QLineEdit>
#include <QPushButton>
#include <QHBoxLayout>

// 组合控件:
class SearchBar : public QWidget
{
    Q_OBJECT
public:
    explicit SearchBar(QString place_holder_text, QWidget *parent = nullptr)
        : QWidget{parent}
        , line_edit_(new QLineEdit(this))
        , search_btn_(new QPushButton(tr("Search"), this))
    {
        line_edit_->setPlaceholderText(place_holder_text);                          // 设置占位提示文本
        line_edit_->setClearButtonEnabled(true);                                    // 启用原生一键清除按钮，输入框后有个x, 点击可以清空编辑框的文本
        line_edit_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);  // 水平方向可以扩张，垂直方向保持首选大小
        search_btn_->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);     // 水平方向固定大小，垂直方向保持首选大小

        connect(search_btn_, &QPushButton::clicked, this, &SearchBar::onSearchTriggered);       // 搜索按钮触发搜索
        connect(line_edit_, &QLineEdit::returnPressed, this, &SearchBar::onSearchTriggered);    // 编辑框回车触发搜索

        QHBoxLayout* layout = new QHBoxLayout(this);
        layout->setContentsMargins(0, 0, 0, 0); // 设置空间和窗体之间的间距为 0
        layout->setSpacing(4);                  // 设置空间之间的距离为 0

        layout->addWidget(line_edit_);
        layout->addWidget(search_btn_);
        setLayout(layout);
    }

    QString text() const {
        return line_edit_->text();
    }
    void setText(const QString &text) {
        line_edit_->setText(text);
    }
    void clear() {
        line_edit_->clear();
    }

signals:
    void searchRequested(const QString &text);

private slots:
    void onSearchTriggered() {
        QString text = line_edit_->text().trimmed();
        if (!text.isEmpty()) {
            emit searchRequested(text);
        }
    }

private:
    QLineEdit* line_edit_;
    QPushButton* search_btn_;
};

#endif // SEARCHBAR_H
