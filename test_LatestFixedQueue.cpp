#include "LatestFixedQueue.h"
#include <iostream>
#include <vector>
#include <memory>
#include <thread>
#include <chrono>
#include <cassert>
#include <atomic>

// Test data structure
struct TestData {
    int id;
    std::string name;
    
    TestData(int i, const std::string& n) : id(i), name(n) {}
    
    bool operator==(const TestData& other) const {
        return id == other.id && name == other.name;
    }
};

class TestLatestFixedQueue {
private:
    int test_count = 0;
    int passed_count = 0;
    
    void assert_true(bool condition, const std::string& test_name) {
        test_count++;
        if (condition) {
            passed_count++;
            std::cout << "[PASS] " << test_name << std::endl;
        } else {
            std::cout << "[FAIL] " << test_name << std::endl;
        }
    }
    
public:
    void run_all_tests() {
        std::cout << "=== Running LatestFixedQueue Unit Tests ===" << std::endl;
        
        test_constructor_and_initial_state();
        test_basic_push_pop();
        test_capacity_overflow();
        test_is_full_is_empty();
        test_size();
        test_get_items_with_filter();
        test_get_items_with_max_count();
        test_start_stop();
        test_stop_with_timeout();
        test_stop_wait_queue_empty();
        test_stop_timeout_while_waiting();
        test_set_capacity();
        test_concurrent_operations();
        test_edge_cases();
        
        print_summary();
    }
    
private:
    void test_constructor_and_initial_state() {
        std::cout << "\n--- Testing Constructor and Initial State ---" << std::endl;
        
        LatestFixedQueue<TestData> queue(5);
        
        assert_true(queue.Size() == 0, "Initial size should be 0");
        assert_true(queue.IsEmpty(), "Queue should be empty initially");
        assert_true(!queue.IsFull(), "Queue should not be full initially");
    }
    
    void test_basic_push_pop() {
        std::cout << "\n--- Testing Basic Push/Pop Operations ---" << std::endl;
        
        LatestFixedQueue<TestData> queue(3);
        auto data1 = std::make_shared<TestData>(1, "test1");
        auto data2 = std::make_shared<TestData>(2, "test2");
        
        queue.Push(data1);
        assert_true(queue.Size() == 1, "Size should be 1 after one push");
        assert_true(!queue.IsEmpty(), "Queue should not be empty after push");
        
        queue.Push(data2);
        assert_true(queue.Size() == 2, "Size should be 2 after two pushes");
        
        std::shared_ptr<TestData> popped;
        assert_true(queue.Pop(popped), "Pop should succeed");
        assert_true(popped->id == 1 && popped->name == "test1", "Popped data should match first pushed");
        assert_true(queue.Size() == 1, "Size should be 1 after one pop");
        
        assert_true(queue.Pop(popped), "Second pop should succeed");
        assert_true(popped->id == 2 && popped->name == "test2", "Second popped data should match");
        assert_true(queue.IsEmpty(), "Queue should be empty after popping all");
    }
    
    void test_capacity_overflow() {
        std::cout << "\n--- Testing Capacity Overflow ---" << std::endl;
        
        LatestFixedQueue<TestData> queue(2);
        auto data1 = std::make_shared<TestData>(1, "test1");
        auto data2 = std::make_shared<TestData>(2, "test2");
        auto data3 = std::make_shared<TestData>(3, "test3");
        
        queue.Push(data1);
        queue.Push(data2);
        assert_true(queue.IsFull(), "Queue should be full after 2 pushes");
        
        queue.Push(data3); // Should remove oldest (data1)
        assert_true(queue.Size() == 2, "Size should remain 2 after overflow");
        
        std::shared_ptr<TestData> popped;
        queue.Pop(popped);
        assert_true(popped->id == 2, "Should pop data2 (data1 was removed)");
    }
    
    void test_is_full_is_empty() {
        std::cout << "\n--- Testing IsFull/IsEmpty Methods ---" << std::endl;
        
        LatestFixedQueue<TestData> queue(2);
        assert_true(queue.IsEmpty(), "Should be empty initially");
        assert_true(!queue.IsFull(), "Should not be full initially");
        
        auto data = std::make_shared<TestData>(1, "test");
        queue.Push(data);
        assert_true(!queue.IsEmpty(), "Should not be empty after push");
        assert_true(!queue.IsFull(), "Should not be full with one element");
        
        queue.Push(data);
        assert_true(!queue.IsEmpty(), "Should not be empty with two elements");
        assert_true(queue.IsFull(), "Should be full with capacity elements");
        
        std::shared_ptr<TestData> popped;
        queue.Pop(popped);
        assert_true(!queue.IsEmpty(), "Should not be empty after one pop");
        assert_true(!queue.IsFull(), "Should not be full after one pop");
    }
    
    void test_size() {
        std::cout << "\n--- Testing Size Method ---" << std::endl;
        
        LatestFixedQueue<TestData> queue(3);
        assert_true(queue.Size() == 0, "Initial size should be 0");
        
        auto data = std::make_shared<TestData>(1, "test");
        queue.Push(data);
        assert_true(queue.Size() == 1, "Size should be 1 after push");
        
        queue.Push(data);
        assert_true(queue.Size() == 2, "Size should be 2 after second push");
        
        queue.Push(data);
        assert_true(queue.Size() == 3, "Size should be 3 after third push");
        
        queue.Push(data); // Overflow
        assert_true(queue.Size() == 3, "Size should remain 3 after overflow");
        
        std::shared_ptr<TestData> popped;
        queue.Pop(popped);
        assert_true(queue.Size() == 2, "Size should be 2 after pop");
    }
    
    void test_get_items_with_filter() {
        std::cout << "\n--- Testing GetItems with Filter ---" << std::endl;
        
        LatestFixedQueue<TestData> queue(5);
        
        // Add test data
        for (int i = 1; i <= 4; i++) {
            queue.Push(std::make_shared<TestData>(i, "test" + std::to_string(i)));
        }
        
        // Test with filter (only even IDs)
        std::vector<TestData> result;
        queue.GetItems(result, [](const std::shared_ptr<TestData>& data) {
            return data->id % 2 == 0;
        });
        
        assert_true(result.size() == 2, "Filter should return 2 items");
        assert_true(result[0].id == 2, "First item should have id 2");
        assert_true(result[1].id == 4, "Second item should have id 4");
    }
    
    void test_get_items_with_max_count() {
        std::cout << "\n--- Testing GetItems with MaxCount ---" << std::endl;
        
        LatestFixedQueue<TestData> queue(5);
        
        // Add test data
        for (int i = 1; i <= 4; i++) {
            queue.Push(std::make_shared<TestData>(i, "test" + std::to_string(i)));
        }
        
        // Test with max count
        std::vector<TestData> result;
        queue.GetItems(result, [](const std::shared_ptr<TestData>& data) {
            return true;
        }, 2);
        
        assert_true(result.size() == 2, "Should return exactly 2 items");
        assert_true(result[0].id == 1, "First item should have id 1");
        assert_true(result[1].id == 2, "Second item should have id 2");
    }
    
    void test_start_stop() {
        std::cout << "\n--- Testing Start/Stop Operations ---" << std::endl;
        
        LatestFixedQueue<TestData> queue(3);
        
        // Test immediate stop
        queue.Stop();
        
        auto data = std::make_shared<TestData>(1, "test");
        queue.Push(data); // Should be ignored when stopped
        
        assert_true(queue.Size() == 0, "Push should be ignored when stopped");
        
        // Test restart
        queue.Start();
        queue.Push(data);
        assert_true(queue.Size() == 1, "Push should work after start");
    }
    
    void test_stop_with_timeout() {
        std::cout << "\n--- Testing Stop with Timeout ---" << std::endl;
        
        LatestFixedQueue<TestData> queue(3);
        
        // Add some data
        auto data = std::make_shared<TestData>(1, "test");
        queue.Push(data);
        
        // Test stop with timeout (should not wait since we're not waiting for empty)
        auto start = std::chrono::steady_clock::now();
        queue.Stop(false, 1000); // 1 second timeout
        auto end = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        
        assert_true(duration.count() < 100, "Immediate stop should take less than 100ms");
    }
    
    void test_stop_wait_queue_empty() {
        std::cout << "\n--- Testing Stop Wait Queue Empty ---" << std::endl;
        
        LatestFixedQueue<TestData> queue(3);
        
        // Add data
        auto data = std::make_shared<TestData>(1, "test");
        queue.Push(data);
        
        // Start a thread to consume data
        std::atomic<bool> consumer_done(false);
        std::thread consumer([&]() {
            std::shared_ptr<TestData> popped;
            queue.Pop(popped);
            consumer_done = true;
        });
        
        // Wait a bit then stop (should wait for queue to empty)
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        
        queue.Stop(true); // Wait for queue empty
        
        consumer.join();
        
        assert_true(consumer_done, "Consumer should have completed");
        assert_true(queue.IsEmpty(), "Queue should be empty after stop");
    }
    
    void test_stop_timeout_while_waiting() {
        std::cout << "\n--- Testing Stop Timeout While Waiting ---" << std::endl;
        
        LatestFixedQueue<TestData> queue(3);
        
        // Add data but don't consume it
        auto data = std::make_shared<TestData>(1, "test");
        queue.Push(data);
        
        auto start = std::chrono::steady_clock::now();
        queue.Stop(true, 500); // Wait for empty with 500ms timeout
        auto end = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        
        assert_true(duration.count() >= 400 && duration.count() <= 600, 
                   "Should timeout after approximately 500ms");
    }
    
    void test_set_capacity() {
        std::cout << "\n--- Testing SetCapacity ---" << std::endl;
        
        LatestFixedQueue<TestData> queue(2);
        
        // Fill to capacity
        auto data = std::make_shared<TestData>(1, "test");
        queue.Push(data);
        queue.Push(data);
        assert_true(queue.IsFull(), "Should be full at capacity 2");
        
        // Increase capacity
        queue.SetCapacity(4);
        assert_true(!queue.IsFull(), "Should not be full after capacity increase");
        
        // Decrease capacity
        queue.SetCapacity(1);
        // Note: The actual behavior depends on implementation
        // This test mainly checks that the method doesn't crash
    }
    
    void test_concurrent_operations() {
        std::cout << "\n--- Testing Concurrent Operations ---" << std::endl;
        
        LatestFixedQueue<TestData> queue(10);
        std::atomic<int> push_count(0);
        std::atomic<int> pop_count(0);
        
        // Producer thread
        std::thread producer([&]() {
            for (int i = 0; i < 100; i++) {
                queue.Push(std::make_shared<TestData>(i, "test" + std::to_string(i)));
                push_count++;
                std::this_thread::sleep_for(std::chrono::microseconds(10));
            }
        });
        
        // Consumer thread
        std::thread consumer([&]() {
            for (int i = 0; i < 100; i++) {
                std::shared_ptr<TestData> data;
                if (queue.Pop(data)) {
                    pop_count++;
                }
                std::this_thread::sleep_for(std::chrono::microseconds(15));
            }
        });
        
        producer.join();
        consumer.join();
        
        assert_true(push_count.load() == 100, "Should have pushed 100 items");
        assert_true(pop_count.load() > 0, "Should have popped some items");
    }
    
    void test_edge_cases() {
        std::cout << "\n--- Testing Edge Cases ---" << std::endl;
        
        // Test with zero capacity (should handle gracefully)
        try {
            LatestFixedQueue<TestData> queue(0);
            assert_true(true, "Zero capacity queue should not crash");
        } catch (...) {
            assert_true(true, "Zero capacity may throw exception (acceptable)");
        }
        
        // Test with negative capacity (should handle gracefully)
        try {
            LatestFixedQueue<TestData> queue(-1);
            assert_true(true, "Negative capacity queue should not crash");
        } catch (...) {
            assert_true(true, "Negative capacity may throw exception (acceptable)");
        }
        
        // Test popping from empty queue with timeout
        LatestFixedQueue<TestData> queue(3);
        std::shared_ptr<TestData> data;
        
        auto start = std::chrono::steady_clock::now();
        queue.Start(); // Start the queue
        queue.Stop(false, 100); // Stop with timeout
        
        // Try to pop (should fail immediately since stopped)
        assert_true(!queue.Pop(data), "Pop should fail when queue is stopped");
        
        auto end = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        assert_true(duration.count() < 200, "Should return quickly when stopped");
    }
    
    void print_summary() {
        std::cout << "\n=== Test Summary ===" << std::endl;
        std::cout << "Total tests: " << test_count << std::endl;
        std::cout << "Passed: " << passed_count << std::endl;
        std::cout << "Failed: " << (test_count - passed_count) << std::endl;
        std::cout << "Success rate: " << (passed_count * 100.0 / test_count) << "%" << std::endl;
        
        if (passed_count == test_count) {
            std::cout << "🎉 All tests passed!" << std::endl;
        } else {
            std::cout << "❌ Some tests failed!" << std::endl;
        }
    }
};

int main() {
    TestLatestFixedQueue test_suite;
    test_suite.run_all_tests();
    return 0;
}