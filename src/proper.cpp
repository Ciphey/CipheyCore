#include <ciphey/proper.hpp>

#include <boost/filesystem.hpp>
#include <random>

#include <thread>
#include <msieve.h>

namespace ciphey {
  std::vector<std::string> factorise(std::string input) {
    thread_local std::uniform_int_distribution<uint32_t> dist;
    thread_local std::random_device csprng;
    thread_local std::seed_seq seed{csprng(), csprng(), csprng(), csprng(), csprng(), csprng(), csprng(), csprng()};
    thread_local std::default_random_engine rng(seed);

//    auto save_state = boost::filesystem::unique_path().c_str();
    uint32_t l1cache, l2cache;
    get_cache_sizes(&l1cache, &l2cache);

    std::unique_ptr<::msieve_obj, msieve_obj*(*)(::msieve_obj*)> obj(::msieve_obj_new(
                                                                  input.data(),
                                                                  msieve_flags::MSIEVE_DEFAULT_FLAGS,
                                                                  nullptr, //save_state,
                                                                  nullptr,
                                                                  nullptr,
                                                                  dist(rng),
                                                                  dist(rng),
                                                                  0,
                                                                  ::get_cpu_type(),
                                                                  l1cache,
                                                                  l2cache,
                                                                  std::thread::hardware_concurrency(),
                                                                  0,
                                                                  nullptr), ::msieve_obj_free);
    msieve_run(obj.get());

    std::vector<std::string> ret;

    for (auto* factor = obj->factors; factor; factor = factor->next) {
      ret.emplace_back(factor->number);
    }

    return ret;
  }
}
