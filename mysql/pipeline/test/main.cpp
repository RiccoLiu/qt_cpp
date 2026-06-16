
#include <logger2.h>
#include <pipeline.h>
#include <yaml-cpp/yaml.h>
#include "node_factory.h"
#include "pipeline.h"

int main() {
    // 1. 创建节点制造工厂
    std::shared_ptr<NodeFactory> factory = std::make_shared<NodeFactory>();

    // 2. 创建pipeline
    std::shared_ptr<Pipeline> pipeline = std::make_shared<Pipeline>(factory);

    // 3. 加载 pipeline 配置文件
    pipeline->LoadYAML("pipeline_test.yaml");

    // 4. pipeline 启动
    pipeline->Start();

    while(1) {
        std::this_thread::sleep_for(std::chrono::duration(std::chrono::seconds(1)));
    }

    // 5. pipeline 停止
    pipeline->Stop();
    return 0;

}
