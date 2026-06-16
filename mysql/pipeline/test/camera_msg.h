#ifndef PIPELINE2_TEST_CAMERA_MSG_H
#define PIPELINE2_TEST_CAMERA_MSG_H

#include <sharedata.h>

#include "pipeline_msg.h"

#define PIPELINE_CAMMSG PIPELINE_FOURCC('c', 'a', 'm', '\0')

#define IMAGE_FRAME_KEY         "img_frame"
#define IMAGE_PYRAMID_KEY       "img_pyramid"
#define GRAY_FRAME_KEY          "gray_frame"
#define GRAY_PYRAMID_KEY        "gray_pyramid"
#define FACE_DETECT_RES_KEY     "face_detect_res"

struct CamTimeStamp {
    uint64_t sec;
    uint64_t nsec;
};

inline bool operator<(const CamTimeStamp &v1, const CamTimeStamp &v2) {
    return (v1.sec < v2.sec) || ((v1.sec == v2.sec) && (v1.nsec < v2.nsec));
}

inline bool operator>(const CamTimeStamp &v1, const CamTimeStamp &v2) {
    return (v1.sec > v2.sec) || ((v1.sec == v2.sec) && (v1.nsec > v2.nsec));
}

class CameraMsg : public PipelineMsg, public ShareDataManager {
public:
    CameraMsg() {
        fourcc_ = PIPELINE_CAMMSG;
    }
    virtual ~CameraMsg() {}

    const std::string GetName() const {
        return name_;
    }
    const uint64_t GetFrameId() const {
        return frame_id_;
    }
    const CamTimeStamp GetTimeStamp() const {
        return timestamp_;
    }
    // !!! 重置Msg状态
    virtual void Reset() {
        ClearObject();
    }

private:
    std::string name_;
    int frame_id_;

    CamTimeStamp timestamp_;
};

#endif // PIPELINE2_TEST_CAMERA_MSG_H
