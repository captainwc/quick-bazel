#include <chrono>
#include <fstream>
#include <iostream>
#include <optional>
#include <string>

#include "libdemo/protobuf/address_book.pb.h"
#include "skutils/logger.h"
#include "skutils/random.h"

void fillInformationForEachPerson(tutorial::addressbook::Person* person) {
    person->mutable_header()->set_timestamp(
        std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch())
            .count());
    person->set_id(RANDTOOL.getRandomInt(1000));
    person->set_email(RANDTOOL.getRandomEmail());
    person->set_name(RANDTOOL.getRandomName());
    //* mutable_xxx() means that you can modify it.
    (*person->mutable_name()).append(" ");

    int phoneNumPerPerson = 3;
    while (phoneNumPerPerson--) {
        //* repeated attr has method add_xxx()
        tutorial::addressbook::Person::PhoneNumber* phone = person->add_phone();
        phone->set_number(RANDTOOL.getRandomPhoneNumber());
        auto type = RANDTOOL.getRandomInt(0, 2);
        if (type == 0) {
            phone->set_type(tutorial::addressbook::Person::MOBILE);
        } else if (type == 1) {
            phone->set_type(tutorial::addressbook::Person::HOME);
        } else {
            phone->set_type(tutorial::addressbook::Person::WORK);
        }
    }
}

bool saveToFile(const tutorial::addressbook::AddressBook& bookers, std::string filename) {
    std::fstream output(filename, std::ios::out | std::ios::trunc | std::ios::binary);
    if (!bookers.SerializePartialToOstream(&output)) {
        SK_ERROR("Failed to write address book.");
        return false;
    }
    return true;
}

auto parseFromFile(std::string filename) -> std::optional<tutorial::addressbook::AddressBook> {
    std::fstream input(filename, std::ios::in | std::ios::binary);
    if (!input) {
        SK_ERROR("File \"{}\" Unexists.", filename);
        return std::nullopt;
    }
    tutorial::addressbook::AddressBook ret;
    if (!ret.ParseFromIstream(&input)) {
        SK_ERROR("Parse information from {} FAILED.", filename);
        return std::nullopt;
    }
    return std::optional(ret);
}

int main() {
    GOOGLE_PROTOBUF_VERIFY_VERSION;

    const char* binFileName = "address_book.pb.bin";

    tutorial::addressbook::AddressBook address_book;
    for (int i = 0; i < 3; i++) {
        fillInformationForEachPerson(address_book.add_person());
    }

    saveToFile(address_book, binFileName);

    auto reParse = parseFromFile(binFileName);
    DUMP(reParse.value_or(tutorial::addressbook::AddressBook{}).DebugString());

    google::protobuf::ShutdownProtobufLibrary();

    return 0;
}
