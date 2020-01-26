// async example
// #include <stdlib.h>     // srand, rand
// #include <time.h>       // time() for seed
#include <iostream>     // std::cout
#include <thread>
#include <future>       // std::async, std::future
#include <chrono>
#include <string>

// cacheflush()
// #include <asm/cachectl.h>

using namespace std;
using ts = decltype(std::chrono::steady_clock::now());

long print_ts(string mod_name, ts before, ts after)
{
    std::cerr << "===========================" << std::endl;
    auto t = std::chrono::duration_cast<std::chrono::microseconds>(after - before).count();
    std::cerr << mod_name << ": ran for " << t << "us" << std::endl;
    std::cerr << "===========================" << std::endl;
    return t;
}

void my_mem_flush(const void *p, unsigned int allocation_size){
    const size_t cache_line = 64;
    const char *cp = (const char *)p;
    size_t i = 0;

    if (p == NULL || allocation_size <= 0)
	return;

    for (i = 0; i < allocation_size; i += cache_line) {
	asm volatile("clflush (%0)\n\t"
		     : 
		     : "r"(&cp[i])
		     : "memory");
    }

    asm volatile("sfence\n\t"
                 :
                 :
                 : "memory");
}

int main (int argc, char *argv[])
{
    char stack_start[] = "0";
    // TODO: How much should we flush??
    auto flush_size = 4096;
    auto prime_candidate = 313222313;

    // purposely slow primality lambda
    auto prime_lambda = [](auto x) {
			    std::cerr << "Wait ..." << x << std::endl;
			    for (int i = 2; i < x; ++i) if ((x % i) == 0) return false;
			    return true;
			};

    auto timed_lambda = [](auto lam, auto mod_name) -> long {
			    auto before = std::chrono::steady_clock::now();
			    lam();
			    auto after = std::chrono::steady_clock::now();
			    auto t1 = print_ts(string(mod_name), before, after);
			    return t1;
			};

    my_mem_flush(stack_start, flush_size);

    std::future<bool> def_fut = std::async(launch::deferred,
					   prime_lambda, prime_candidate);

    std::cerr << "Checking whether " << prime_candidate << " is prime" << std::endl;

    bool ret;
    long total = 0;
    for ([[maybe_unused]] auto i : {1, 2, 3, 4, 5}) {
	// policy for async() is unspecified
	std::future<bool> fut = std::async(prime_lambda, prime_candidate);
	auto f = 
	    [&ret, &fut]() -> bool {
		ret = fut.get();
		return ret;
	    };
	total += timed_lambda(f, "unspecified");
    }
    double avg = total / 5;
    std::cout << "unspecified took " << avg << "us on avg" << std::endl;

    if (argc == 2) {
	auto foo_cache_flush = [&stack_start, &flush_size]() {
				my_mem_flush(stack_start, flush_size);
			    };
	timed_lambda(foo_cache_flush, "cacheflush");

	auto foo_deferred_fut_get = [&ret, &def_fut]() {
					ret = def_fut.get();
				    };
	auto tt = timed_lambda(foo_deferred_fut_get, "deferred");
	std::cout << "deferred took " << tt << "us on avg" << std::endl;
    }

    if (ret) std::cerr << "It is prime!\n";
    else std::cerr << "It is not prime.\n";

    return 0;
}
