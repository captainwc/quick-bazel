#include <condition_variable>
#include <csignal>
#include <fastdds/dds/domain/DomainParticipant.hpp>
#include <fastdds/dds/domain/DomainParticipantFactory.hpp>
#include <fastdds/dds/subscriber/DataReader.hpp>
#include <fastdds/dds/subscriber/DataReaderListener.hpp>
#include <fastdds/dds/subscriber/SampleInfo.hpp>
#include <fastdds/dds/subscriber/Subscriber.hpp>
#include <fastdds/dds/subscriber/qos/DataReaderQos.hpp>
#include <fastdds/dds/topic/TypeSupport.hpp>
#include <iostream>

#include "HelloWorld.hpp"
#include "HelloWorldPubSubTypes.hpp"

using namespace eprosima::fastdds::dds;

std::condition_variable cv;
std::mutex              mtx;
bool                    stop_program = false;

void signal_handler(int signum) {
    std::cout << "Interrupt signal (" << signum << ") received." << std::endl;
    {
        std::unique_lock<std::mutex> lock(mtx);
        stop_program = true;
    }
    cv.notify_one();
}

class HelloWorldSubscriber {
private:
    class SubListener : public DataReaderListener {
    public:
        void on_data_available(DataReader* reader) override {
            SampleInfo info;
            HelloWorld msg;
            if (reader->take_next_sample(&msg, &info) == RETCODE_OK) {
                if (info.valid_data) {
                    std::cout << "Received message: '" << msg.message() << "' with index " << msg.index() << std::endl;
                }
            }
        }
    };

public:
    HelloWorldSubscriber()
        : participant_(nullptr),
          subscriber_(nullptr),
          reader_(nullptr),
          topic_(nullptr),
          type_(new HelloWorldPubSubType()) {}

    ~HelloWorldSubscriber() {
        if (participant_ != nullptr) {
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

        subscriber_ = participant_->create_subscriber(SUBSCRIBER_QOS_DEFAULT);
        if (subscriber_ == nullptr) {
            return false;
        }

        topic_ = participant_->create_topic(HELLO_TOPIC_NAME, type_.get_type_name(), TOPIC_QOS_DEFAULT);
        if (topic_ == nullptr) {
            return false;
        }

        reader_ = subscriber_->create_datareader(topic_, DATAREADER_QOS_DEFAULT, &listener_);
        if (reader_ == nullptr) {
            return false;
        }

        return true;
    }

    void Subscribe() {
        std::cout << "Subscriber running. Waiting for data... Press Ctrl+C to exit." << std::endl;
        std::unique_lock<std::mutex> lock(mtx);
        cv.wait(lock, [] { return stop_program; });
    }

private:
    static constexpr const char* HELLO_TOPIC_NAME = "HelloWorldTopic";

    DomainParticipant* participant_;
    Subscriber*        subscriber_;
    DataReader*        reader_;
    Topic*             topic_;
    TypeSupport        type_;
    SubListener        listener_;
};

int main() {
    std::cout << "Starting subscriber." << std::endl;

    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);

    HelloWorldSubscriber subscriber;
    if (subscriber.Init()) {
        subscriber.Subscribe();
    }

    std::cout << "Subscriber finished." << std::endl;
    return 0;
}
