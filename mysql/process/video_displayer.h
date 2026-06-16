#ifndef VIDEO_DISPLAYER_H
#define VIDEO_DISPLAYER_H

#include <QImage>
#include <QObject>

#include <pipeline_node.h>

class VideoDisplayer : public QObject, public PipelineNode {
    Q_OBJECT
public:
    explicit VideoDisplayer(const std::string& instance_name, QObject *parent = nullptr);

    virtual bool Process() {
        return false;
    }
    virtual bool ProcessMsg(MsgPtr msg);

    virtual std::string GetNodeName() {
        return VideoDisplayer::NodeName();
    }
    static std::string NodeName() {
        return "VideoDisplayer";
    }

signals:
    void Display(const QImage& img);
};

#endif // VIDEO_DISPLAYER_H
