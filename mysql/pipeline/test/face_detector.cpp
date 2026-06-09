#include <logger2.h>

#include "face_detector.h"
#include "opencv_frame.h"

std::shared_ptr<PipelineNode> CreateFaceDetector(const std::string& name) {
    std::shared_ptr<PipelineNode> face_detector = std::make_shared<FaceDetector>(name);
    return face_detector;
}

FaceDetector::FaceDetector(const std::string& name)
    : PipelineNode(name)
{
    m_valid_marker = true;
}

FaceDetector::~FaceDetector()
{
    m_valid_marker = false;
}

bool FaceDetector::LoadSubYaml(YAML::Node &config) {
    YAML::Emitter out;
    out << config;
    LOGD("FaceDetector YAML: \n%s", out.c_str());

    if (config.IsNull()) {
        return false;
    }

    model_ = config["model"].as<std::string>();
    config_file_ = config["config_file"].as<std::string>();

    return true;
}

void FaceDetector::InitializeThreadOnce() {
    // PipelineNode::registerThreadCleanup();

    m_net = cv::dnn::readNetFromCaffe(config_file_, model_);
    if (m_net.empty()) {
        LOGI("Failed to load DNN face detector model!");
        return;
    }
}

void FaceDetector::UninitializeThreadOnce() {
}

bool FaceDetector::processMsg(std::shared_ptr<PipelineMsg> &msg, std::shared_ptr<PipelineSink> &sink) {


    if (!m_valid_marker) {
        std::cerr << "ERROR: processMsg called on destroyed FaceDetector!\n";
        std::abort();
    }


    std::cout << " FaceDetector::processMsg --------" << std::endl;
    // LOGI("---FaceDetector: %s:%d ---->", __func__, __LINE__);

    // if ()

    uint32_t fourcc = msg->GetFourCC();
    if (fourcc == OPENCV_FRAME_FOURCC) {
        std::shared_ptr<OpenCVFrame> camera = std::dynamic_pointer_cast<OpenCVFrame>(msg);
        if (camera) {
            cv::Mat frame = camera->GetFrame();

            cv::Mat inputBlob = cv::dnn::blobFromImage(frame, 1.0, cv::Size(300, 300), cv::Scalar(104, 177, 123), false, false);
            m_net.setInput(inputBlob);
            cv::Mat detection = m_net.forward();

            if (detection.empty() || detection.dims != 4 || detection.size[3] != 7) {
                LOGE("Invalid detection output: dims=%d, size=[%d,%d,%d,%d]",
                     detection.dims,
                     detection.dims>0 ? detection.size[0] : -1,
                     detection.dims>1 ? detection.size[1] : -1,
                     detection.dims>2 ? detection.size[2] : -1,
                     detection.dims>3 ? detection.size[3] : -1);
                return false;
            }

            int numDetections = detection.size[2];
            float* data = (float*)detection.data; // 更安全的方式

            cv::Mat result = frame; // 避免修改原始帧（如果需要）

            for (int i = 0; i < numDetections; ++i) {
                float confidence = data[i * 7 + 2];
                if (confidence > 0.5) {
                    int x1 = static_cast<int>(data[i * 7 + 3] * frame.cols);
                    int y1 = static_cast<int>(data[i * 7 + 4] * frame.rows);
                    int x2 = static_cast<int>(data[i * 7 + 5] * frame.cols);
                    int y2 = static_cast<int>(data[i * 7 + 6] * frame.rows);

                    // 边界检查
                    x1 = std::max(0, std::min(x1, frame.cols - 1));
                    y1 = std::max(0, std::min(y1, frame.rows - 1));
                    x2 = std::max(0, std::min(x2, frame.cols - 1));
                    y2 = std::max(0, std::min(y2, frame.rows - 1));

                    cv::rectangle(result, cv::Point(x1, y1), cv::Point(x2, y2), cv::Scalar(0, 255, 0), 2);
                }
            }

            // std::shared_ptr<PipelineSource> source = GetSource("face_detector_source");
            // if (source) {
            //     source->PublishPipelineMsg(msg);
            // }
            PublishMsg(msg);
        }
    }

    return true;
}
