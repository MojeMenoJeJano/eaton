#include <iostream>
#include <thread>
#include <vector>
#include <cassert>

class Counter {
private:
    int count = 0;
    std::mutex mtx;
public:
    void incrementCount() {
        const std::lock_guard<std::mutex> lock(mtx);
        count++;
    }

    int getCount() {
        std::cout << "The function incrementCount was called: " << count << " times." << std::endl;
        const std::lock_guard<std::mutex> lock(mtx);
        return count;
    }
};

class MessageReceiver {
    std::shared_ptr<Counter> counter;
public:
    explicit MessageReceiver(std::shared_ptr<Counter> counter)
            : counter{counter} {};

    template<typename T>
    void getMassage(T message) {
        counter->incrementCount();
        //here the message would have been processed further
    }
};

//this function is called in thread
void deviceWhoSendsData(int iterationCount, std::shared_ptr<MessageReceiver> messageReceiver) {
    for (int i = 0; i < iterationCount; i++) {
        messageReceiver->getMassage("random string");
    }
}

int main() {

    auto counter = std::make_shared<Counter>();
    auto messageReceiver = std::make_shared<MessageReceiver>(counter);

    int numberOfThreads = 150;
    int numberOfIterations = 10000;
    int expectedResult = numberOfIterations * numberOfThreads;

    std::vector<std::thread> devices;

    for (int i = 0; i < numberOfThreads; i++) {
        devices.emplace_back(deviceWhoSendsData, numberOfIterations, messageReceiver); //make new thread
    }
    for (auto &i: devices) {
        i.join();
    }
    assert(counter->getCount() == expectedResult);

    return 0;
}
