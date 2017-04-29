#include "staging/thread_collector.hpp"
#include "playground.hpp"

//template<typename T>
//using deleted_unique_ptr = std::unique_ptr<T, std::function<void(T*)>>;

int main(int argc, char *argv[])
{
    std::cout << argv[0] << " running" << std::endl;

    //deleted_unique_ptr<bullshit> bullptr(new bullshit, [](auto p) { delete p; });

    auto bullptr2 = std::make_shared<bullshit>();

    const auto runner = [bull = std::move(bullptr2)](const unsigned ms = 500) {
        bull->hello();
        std::this_thread::sleep_for(std::chrono::milliseconds(ms));
        //throw std::exception();
    };

    staging::thread_collector collector_default;
    staging::thread_collector collector(runner);
    collector.spawn(runner);
    collector.spawn(runner, 40);

    collector_default = std::move(collector);
    collector_default.swap(collector);

    /*auto thread_count = 10000;
    while (thread_count--)
        collector.spawn(runner);*/

    collector_default.join();
    collector.join();

    //std::thread wtf(runner);

    std::cout << argv[0] << " finished" << std::endl;
    return EXIT_SUCCESS;
}
