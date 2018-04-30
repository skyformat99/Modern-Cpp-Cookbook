#pragma once

#include <atomic>
#include <cassert>
#include <iostream>
#include <numeric>
#include <thread>

namespace recipe_8_08 {
  void test_atomic()
  {
    std::atomic<int> counter{ 0 };

    std::vector<std::thread> threads;
    for (int i = 0; i < 10; ++i) {
      threads.emplace_back([&counter]() {
        for (int i = 0; i < 10; ++i)
          ++counter;
      });
    }

    for (auto& t : threads)
      t.join();

    std::cout << counter << std::endl;
  }

  void test_atomic_flag()
  {
    std::atomic_flag lock = ATOMIC_FLAG_INIT;
    int counter = 0;

    std::vector<std::thread> threads;
    for (int i = 0; i < 10; ++i) {
      threads.emplace_back([&]() {
        while (lock.test_and_set(std::memory_order_acquire))
          ;
        ++counter;
        lock.clear(std::memory_order_release);
      });
    }

    for (auto& t : threads)
      t.join();

    std::cout << counter << std::endl;
  }

  std::vector<int> generate_random()
  {
    std::random_device rd;
    auto generator = std::mt19937{ rd() };
    auto dis = std::uniform_int_distribution<>{ 1, 99 };
    std::vector<int> numbers(100000, 0);
    std::generate(std::begin(numbers), std::end(numbers),
                  [&dis, &generator] { return dis(generator); });

    return numbers;
  }

  void test_fetch_arithmetic()
  {
    std::atomic<int> sum{ 0 };
    std::vector<int> numbers = generate_random();

    auto sum_expected = std::accumulate(std::begin(numbers), std::end(numbers), 0);

    size_t size = numbers.size();
    std::vector<std::thread> threads;
    for (int i = 0; i < 10; ++i) {
      threads.emplace_back(
        [&sum, &numbers](size_t const start, size_t const end) {
          for (size_t i = start; i < end; ++i) {
            std::atomic_fetch_add_explicit(&sum, numbers[i], std::memory_order_acquire);
          }
        },
        i * (size / 10), (i + 1) * (size / 10));
    }

    for (auto& t : threads)
      t.join();

    assert(sum == sum_expected);
    std::cout << sum << std::endl;
    std::cout << sum_expected << std::endl;
  }

  template <typename T,
            typename I = typename std::enable_if<std::is_integral<T>::value>::type>
  class atomic_counter {
    std::atomic<T> counter{ 0 };

  public:
    T increment()
    {
      return counter.fetch_add(1);
    }

    T decrement()
    {
      return counter.fetch_sub(1);
    }

    T get()
    {
      return counter.load();
    }
  };

  void test_counter()
  {
    atomic_counter<int> counter;

    std::vector<std::thread> threads;
    for (int i = 0; i < 10; ++i) {
      threads.emplace_back([&counter]() {
        for (int i = 0; i < 10; ++i)
          counter.increment();
      });
    }

    for (auto& t : threads)
      t.join();

    std::cout << counter.get() << std::endl;
  }

  void execute()
  {
    test_atomic();
    test_atomic_flag();
    test_fetch_arithmetic();
    test_counter();
  }
} // namespace recipe_8_08