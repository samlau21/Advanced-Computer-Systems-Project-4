#include <iostream>
#include <chrono>
#include <unordered_map>
#include <vector>
#include <thread>
#include <mutex>
#include <arm_neon.h> // For NEON instructions

std::mutex mtx;

// Dictionary Encoding
class DictionaryEncoder {
public:
    void encode(const std::vector<std::string>& column) {
        std::lock_guard<std::mutex> lock(mtx);
        for (const auto& item : column) {
            if (dictionary.find(item) == dictionary.end()) {
                dictionary[item] = dictionary.size();
            }
            encodedColumn.push_back(dictionary[item]);
        }
    }

    bool query(const std::string& item, int& index) {
        std::lock_guard<std::mutex> lock(mtx);
        if (dictionary.find(item) != dictionary.end()) {
            index = dictionary[item];
            return true;
        }
        return false;
    }

    std::vector<int> prefixScan(const std::string& prefix) {
        std::lock_guard<std::mutex> lock(mtx);
        std::vector<int> results;
        for (const auto& pair : dictionary) {
            if (pair.first.find(prefix) == 0) {
                results.push_back(pair.second);
            }
        }
        return results;
    }

private:
    std::unordered_map<std::string, int> dictionary;
    std::vector<int> encodedColumn;
};

// Key-Value Store
class KeyValueStore {
public:
    void put(const std::string& key, const std::string& value) {
        std::lock_guard<std::mutex> lock(mtx);
        store[key] = value;
    }

    bool get(const std::string& key, std::string& value) {
        std::lock_guard<std::mutex> lock(mtx);
        if (store.find(key) != store.end()) {
            value = store[key];
            return true;
        }
        return false;
    }

    void del(const std::string& key) {
        std::lock_guard<std::mutex> lock(mtx);
        store.erase(key);
    }

private:
    std::unordered_map<std::string, std::string> store;
};

// Vanilla Column Scan
void vanillaColumnScan(const std::vector<std::string>& column) {
    for (const auto& item : column) {
        std::cout << item << std::endl;
    }
}

// Multi-threading example
void multiThreading(DictionaryEncoder& encoder, const std::vector<std::string>& column) {
    std::vector<std::thread> threads;
    int numThreads = 8; // thread count
    int chunkSize = column.size() / numThreads;

    for (int i = 0; i < numThreads; ++i) {
        int start = i * chunkSize;
        int end = (i == numThreads - 1) ? column.size() : start + chunkSize;

        threads.emplace_back([&encoder, &column, start, end]() {
            std::vector<std::string> chunk(column.begin() + start, column.begin() + end);
            encoder.encode(chunk);
        });
    }

    for (auto& thread : threads) {
        thread.join();
    }
}

// NEON Utilization example
void neonQuery(const std::vector<int>& data, int value) {
    int32x4_t val = vdupq_n_s32(value);
    for (size_t i = 0; i < data.size(); i += 4) {
        int32x4_t chunk = vld1q_s32(&data[i]);
        uint32x4_t cmp = vceqq_s32(chunk, val);
        uint64_t mask = vgetq_lane_u64(vreinterpretq_u64_u32(cmp), 0);
        if (mask) {
            std::cout << "Value found at index: " << i << std::endl;
        }
    }
}

void neonPrefixScan(const std::vector<std::string>& keys, const std::string& prefix, std::vector<int>& results) {
    size_t prefix_len = prefix.size();
    uint8x16_t prefix_vec = vld1q_u8(reinterpret_cast<const uint8_t*>(prefix.c_str())); // Load prefix into a NEON register

    for (size_t i = 0; i < keys.size(); i++) {
        const std::string& key = keys[i];
        if (key.size() < prefix_len) {
            continue; // Skip if the key is shorter than the prefix
        }

        uint8x16_t key_vec = vld1q_u8(reinterpret_cast<const uint8_t*>(key.c_str())); // Load key into a NEON register
        uint8x16_t cmp_vec = vceqq_u8(key_vec, prefix_vec); // Compare the two
        uint64_t cmp_mask = vgetq_lane_u64(vreinterpretq_u64_u8(cmp_vec), 0); // Extract comparison results

        // Check if all bytes match the prefix
        if ((cmp_mask & ((1 << prefix_len) - 1)) == ((1 << prefix_len) - 1)) {
            results.push_back(i); // Add matching index to results
        }
    }
}

void testQueryPerformanceSIMD(DictionaryEncoder& encoder, const std::vector<std::string>& column, const std::string& query) {
    // Single-item query: Baseline (non-SIMD)
    int index;
    auto start = std::chrono::high_resolution_clock::now();
    bool found = encoder.query(query, index);
    auto end = std::chrono::high_resolution_clock::now();
    std::cout << "Single-item Query (Baseline) Time: " 
              << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count()
              << " us" << std::endl;

    // Single-item query: SIMD
    std::vector<int> encoded_data(column.size());
    encoder.encode(column);  // Populate encoded data for SIMD
    start = std::chrono::high_resolution_clock::now();
    neonQuery(encoded_data, index); // Call SIMD function
    end = std::chrono::high_resolution_clock::now();
    std::cout << "Single-item Query (SIMD) Time: " 
              << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count()
              << " us" << std::endl;

    // Prefix scan: Baseline
    start = std::chrono::high_resolution_clock::now();
    std::vector<int> results = encoder.prefixScan(query);
    end = std::chrono::high_resolution_clock::now();
    std::cout << "Prefix Scan (Baseline) Time: " 
              << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count()
              << " us" << std::endl;

    // Prefix scan: SIMD
    start = std::chrono::high_resolution_clock::now();
    std::vector<int> simd_results;
    neonPrefixScan(column, query, simd_results); // Call SIMD prefix scan
    end = std::chrono::high_resolution_clock::now();
    std::cout << "Prefix Scan (SIMD) Time: " 
              << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count()
              << " us" << std::endl;

    // Output results for validation
    std::cout << "Baseline Prefix Scan Results: ";
    for (int idx : results) {
        std::cout << idx << " ";
    }
    std::cout << std::endl;

    std::cout << "SIMD Prefix Scan Results: ";
    for (int idx : simd_results) {
        std::cout << idx << " ";
    }
    std::cout << std::endl;
}

int main() 
{
    DictionaryEncoder encoder;
    KeyValueStore kvStore;

    std::vector<std::string> column(1000, "test");
    column.push_back("test1");
    
    encoder.encode(column);
    testQueryPerformanceSIMD(encoder, column, "test1");

    //code below used for various performance testing
    /*
    int index;
    if (encoder.query("banana", index)) {
        std::cout << "Banana found at index: " << index << std::endl;
    }

    auto start = std::chrono::high_resolution_clock::now();
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;
    std::cout << "Elapsed time: " << elapsed.count() << " seconds" << std::endl;
    
    vanillaColumnScan(column);
    
    multiThreading(encoder, column);

    */
    return 0;
}
