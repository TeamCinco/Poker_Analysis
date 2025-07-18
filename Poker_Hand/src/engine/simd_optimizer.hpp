#pragma once

#include "types.hpp"
#include <immintrin.h>
#include <array>
#include <random>
#include <vector>

namespace poker {

class SimdOptimizer {
public:
    // SIMD-optimized batch processing
    static void process_batch_avx2(const std::array<Card, 2>& hero_hand,
                                   const std::array<std::array<Card, 2>, 8>& opponent_hands,
                                   const std::array<Card, 5>& board,
                                   std::array<int, 8>& results);
    
    // Vectorized probability calculations
    static void calculate_probabilities_avx2(const std::array<uint64_t, 8>& win_counts,
                                             const std::array<uint64_t, 8>& total_counts,
                                             std::array<double, 8>& probabilities);
    
    // SIMD-optimized card shuffling
    static void shuffle_deck_avx2(std::array<Card, 52>& deck, std::mt19937_64& rng);
    
    // Parallel hand evaluation for 8 hands
    static void evaluate_hands_batch_avx2(const std::array<std::array<Card, 7>, 8>& hands,
                                          std::array<uint32_t, 8>& ranks);
    
    // CPU feature detection
    static bool has_avx2_support();
    static bool has_avx512_support();
    
private:
    // SIMD utility functions
    static __m256i pack_cards_avx2(const std::array<Card, 8>& cards);
    static void unpack_results_avx2(const __m256i& packed_results, std::array<uint32_t, 8>& results);
    
    // Vectorized comparison operations
    static __m256i compare_hands_avx2(const __m256i& hand_ranks1, const __m256i& hand_ranks2);
    
    // SIMD-optimized bit manipulation
    static __m256i popcount_avx2(const __m256i& values);
    static __m256i find_straights_avx2(const __m256i& rank_masks);
};

// High-performance memory pool for SIMD operations
template<typename T, size_t Alignment = 64>
class AlignedMemoryPool {
public:
    AlignedMemoryPool(size_t pool_size);
    ~AlignedMemoryPool();
    
    T* allocate(size_t count);
    void deallocate(T* ptr);
    void reset();
    
private:
    alignas(Alignment) std::vector<T> pool_;
    size_t current_offset_;
    size_t pool_size_;
};

// Cache-optimized simulator for maximum performance
class CacheOptimizedSimulator {
public:
    CacheOptimizedSimulator(int num_threads);
    
    // Process multiple simulations with cache-friendly access patterns
    void simulate_batch_optimized(const std::array<Card, 2>& hero_hand,
                                  int num_opponents,
                                  int batch_size,
                                  std::array<SimulationResult, 8>& results);
    
    // Tile-based processing for large datasets
    void process_tiles(const std::vector<std::string>& hands,
                       int simulations_per_hand,
                       std::vector<HandAnalysis>& results);
    
private:
    struct alignas(64) CacheLine {
        std::array<uint64_t, 8> counters;
    };
    
    std::vector<CacheLine> thread_local_counters_;
    AlignedMemoryPool<ThreadLocalData> memory_pool_;
    int num_threads_;
    
    // Cache-friendly data layout
    void optimize_data_layout();
    
    // Prefetch strategies
    void prefetch_next_batch(const void* data, size_t size);
};

} // namespace poker
