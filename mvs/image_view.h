#ifndef IMAGE_VIEW_H
#define IMAGE_VIEW_H

#include <QWidget>
#include <QLabel>

#include "image_control.h"
#include "controls/imagelabel.h"

class ImageView : public QWidget
{
    Q_OBJECT
public:
    explicit ImageView(QWidget *parent = nullptr);

    int RegisteControl(ImageControl* control);

    void SetImageViewCount(int count);

    int GetImgViewCount() const {
        return img_view_count_;
    }
signals:

private:
    int img_view_count_ {0};
    QVector<ImageLabel*> displayer_;

    ImageControl* img_ctl_;
};

#endif // IMAGE_VIEW_H
