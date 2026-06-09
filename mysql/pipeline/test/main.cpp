
#include <logger2.h>
#include <pipeline.h>
#include <yaml-cpp/yaml.h>
#include "video_decoder.h"
#include "opencv_display.h"
#include "face_detector.h"


int main() {
    // 1.注册节点制造器
    PipelineNodeFactory::GetInstance().RegisterCreator("VideoDecoder", OpenCVDecoderCreator);
    PipelineNodeFactory::GetInstance().RegisterCreator("VideoDisplay", OpenCVDisplayCreator);
    PipelineNodeFactory::GetInstance().RegisterCreator("FaceDetector", CreateFaceDetector);

    // 2. 创建 pipeline
    // std::string sYamlPath = "pipeline_test.yaml";
    std::string sPerceptionPipelineName = "PerceptionPipeline";
    std::shared_ptr<Pipeline> pipeline = std::make_shared<Pipeline>(sPerceptionPipelineName);
    if (pipeline) {
        // 3.连接 pipeline
        std::string sYamlPath = "pipeline_decoder_face-detect_display.yaml";
        YAML::Node config = YAML::LoadFile(sYamlPath);
        pipeline->LoadYAML(config);
    }

    // 4. 启动 pipeline
    pipeline->Start();

    while(1) {
        std::this_thread::sleep_for(std::chrono::duration(std::chrono::seconds(1)));
    }
    return 0;

}
