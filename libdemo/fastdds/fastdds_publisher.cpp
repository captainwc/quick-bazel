#include <chrono>
#include <iostream>
#include <thread>

#include "HelloWorld.hpp"
#include "HelloWorldPubSubTypes.hpp"
#include "fastdds/dds/domain/DomainParticipant.hpp"
#include "fastdds/dds/domain/DomainParticipantFactory.hpp"
#include "fastdds/dds/publisher/DataWriter.hpp"
#include "fastdds/dds/publisher/Publisher.hpp"
#include "fastdds/dds/publisher/qos/DataWriterQos.hpp"
#include "fastdds/dds/publisher/qos/PublisherQos.hpp"
#include "fastdds/dds/topic/Topic.hpp"
#include "fastdds/dds/topic/TypeSupport.hpp"
#include "fastdds/dds/topic/qos/TopicQos.hpp"

using namespace eprosima::fastdds::dds;

class HelloWorldPublisher {
public:
    HelloWorldPublisher()
        : participant_(nullptr),
          publisher_(nullptr),
          writer_(nullptr),
          topic_(nullptr),
          type_(new HelloWorldPubSubType()) {}

    ~HelloWorldPublisher() {
        if (participant_ != nullptr) {
            // 删除所有实体
            participant_->delete_contained_entities();
            DomainParticipantFactory::get_instance()->delete_participant(participant_);
        }
    }

    bool Init() {
        participant_ = DomainParticipantFactory::get_instance()->create_participant(0, PARTICIPANT_QOS_DEFAULT);
        if (participant_ == nullptr) {
            return false;
        }
        type_.register_type(participant_);
        publisher_ = participant_->create_publisher(PUBLISHER_QOS_DEFAULT);
        if (publisher_ == nullptr) {
            return false;
        }
        topic_ = participant_->create_topic(HELLO_TOPIC_NAME, type_.get_type_name(), TOPIC_QOS_DEFAULT);
        if (topic_ == nullptr) {
            return false;
        }
        writer_ = publisher_->create_datawriter(topic_, DATAWRITER_QOS_DEFAULT);
        if (writer_ == nullptr) {
            return false;
        }
        return true;
    }

    void Publish(uint32_t samples) {
        HelloWorld msg;
        for (uint32_t i = 0; i < samples; ++i) {
            msg.index(i);
            msg.message("Hello World");
            writer_->write(&msg);
            std::cout << "Sent message: '" << msg.message() << "' with index " << msg.index() << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }

private:
    static constexpr const char* HELLO_TOPIC_NAME = "HelloWorldTopic";

    DomainParticipant* participant_;
    Publisher*         publisher_;
    DataWriter*        writer_;
    Topic*             topic_;
    TypeSupport        type_;
    HelloWorld         msg_;
};

int main() {
    std::cout << "Starting publisher." << std::endl;
    HelloWorldPublisher publisher;

    if (publisher.Init()) {
        publisher.Publish(10);
    }

    std::cout << "Publisher finished." << std::endl;
    return 0;
}
