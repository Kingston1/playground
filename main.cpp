#include "staging/thread_collector.hpp"
#include "playground.hpp"

#include <future>

//#include <boost/thread/future.hpp>
//#include <boost/thread/executors/basic_thread_pool.hpp>

template<typename T>
using deleted_unique_ptr = std::unique_ptr<T, std::function<void(T*)>>;

int main(int argc, char *argv[])
{
    std::cout << argv[0] << " running" << std::endl;

    //deleted_unique_ptr<bullshit> bullptr(new bullshit, [](auto p) { delete p; });

    auto bullptr2 = std::make_shared<bullshit>();

    const auto runner = [bull = std::move(bullptr2)](const unsigned ms = 500) {
        bull->hello();
        std::this_thread::sleep_for(std::chrono::milliseconds(ms));
        throw std::exception();
    };

    thread_collector collector;
    //thread_collector collector(runner);
    collector.spawn(runner);
    collector.spawn(nullptr);

    /*auto thread_count = 10000;
    while (thread_count--)
        collector.spawn(runner);*/

    collector.join();

    //auto future = std::async(std::launch::async, runner);
    //std::thread wtf(runner);

    std::cout << argv[0] << " finished" << std::endl;
    return EXIT_SUCCESS;
}
