#include <chrono>
#include <iostream>
#include <string>
#include <thread>

#include "mqtt/async_client.h"

// MQTT 服务器地址和客户端 ID
const std::string SERVER_ADDRESS("tcp://localhost:1883");
const std::string CLIENT_ID("paho_cpp_example");
const std::string TOPIC("test/topic");
const int         QOS = 1;

// 回调类：处理连接状态、消息到达等事件
class MqttCallback : public virtual mqtt::callback {
public:
    void connection_lost(const std::string& cause) override {
        std::cout << "Connection lost: " << (cause.empty() ? "unknown reason" : cause) << std::endl;
    }

    void message_arrived(mqtt::const_message_ptr msg) override {
        std::cout << "Message received:\n"
                  << "  Topic: " << msg->get_topic() << "\n"
                  << "  Payload: " << msg->to_string() << std::endl;
    }

    void delivery_complete(mqtt::delivery_token_ptr token) override {
        std::cout << "Message delivery confirmed" << std::endl;
    }
};

int main() {
    // 创建异步客户端
    mqtt::async_client client(SERVER_ADDRESS, CLIENT_ID);

    // 设置回调
    MqttCallback cb;
    client.set_callback(cb);

    // 连接选项（保持会话、心跳间隔等）
    mqtt::connect_options connOpts;
    connOpts.set_clean_session(true);
    connOpts.set_keep_alive_interval(20);

    try {
        // 连接到服务器
        std::cout << "Connecting to MQTT server..." << std::endl;
        client.connect(connOpts)->wait();
        std::cout << "Connected!" << std::endl;

        // 订阅主题
        client.subscribe(TOPIC, QOS)->wait();
        std::cout << "Subscribed to topic: " << TOPIC << std::endl;

        // 发布消息
        std::string payload = "Hello from paho.mqtt.cpp!";
        auto        msg     = mqtt::make_message(TOPIC, payload);
        msg->set_qos(QOS);
        client.publish(msg)->wait();
        std::cout << "Message published: " << payload << std::endl;

        // 等待接收消息（模拟持续运行）
        std::this_thread::sleep_for(std::chrono::seconds(10));

        // 断开连接
        client.disconnect()->wait();
        std::cout << "Disconnected." << std::endl;
    } catch (const mqtt::exception& exc) {
        std::cerr << "Error: " << exc.what() << std::endl;
        return 1;
    }

    return 0;
}
