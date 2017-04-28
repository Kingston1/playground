#include "staging/thread_collector.hpp"
#include "playground.hpp"

#include <vector>
#include <future>
#include <tuple>

//#include <boost/thread/thread.hpp>
//#include <boost/thread/future.hpp>
//#include <boost/thread/executors/basic_thread_pool.hpp>

template<typename T>
using deleted_unique_ptr = std::unique_ptr<T, std::function<void(T*)>>;

int main(int argc, char *argv[])
{
    std::cout << argv[0] << " started";

    //deleted_unique_ptr<bullshit> bullptr(new bullshit, [](auto p) { delete p; });

    auto bullptr2 = std::make_shared<bullshit>();

    const auto runner = [bullshit = std::move(bullptr2)](const unsigned ms = 500) {
        bullshit->hello();
        std::this_thread::sleep_for(std::chrono::milliseconds(ms));
    };

    thread_collector collector;
    collector.spawn(runner);
    collector.spawn(nullptr);

    auto thread_count = 100000;
    while (thread_count--)
    {
        collector.spawn(runner);
    }

    //std::this_thread::sleep_for(std::chrono::milliseconds(4000));

    collector.join();

    auto future = std::async(std::launch::async, runner);

    std::thread wtf(runner);

    //thread_collector collector2(runner);
    //collector2.spawn(runner);
    //runner(40);

    /*let first_tuple = std::make_tuple(2.3f, 6, "yello");
    //const auto { tuple_1, tuple_2, tuple_3 } = first_tuple;

    using r_tuple = std::tuple<float, unsigned, std::string>;
    std::vector<r_tuple> stupler{
        first_tuple,
        std::make_tuple(1.3f, 0, "hello"),
        std::make_tuple(-1.3f, 55, "mello"),
        std::make_tuple(-144.3f, 56, "cello")
    };

    std::cout << __FUNCTION__ << std::endl;

    for (const auto &t : stupler)
    {
        std::cout << std::get<0>(t) << std::endl;
        std::cout << std::get<1>(t) << std::endl;
        std::cout << std::get<2>(t) << std::endl;
    }*/

    /*for (const auto [&a, &b, &c] : stupler)
    {
        std::cout << a << std::endl;
        std::cout << b << std::endl;
        std::cout << c << std::endl;
    }*/

    //auto[a, b] = std::make_pair(1, 2.0);

    return EXIT_SUCCESS;
}
