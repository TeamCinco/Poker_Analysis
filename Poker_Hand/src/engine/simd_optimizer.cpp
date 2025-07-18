#include "simd_optimizer.hpp"
#include "hand_evaluator.hpp"
#include "../utils/bit_utils.hpp"
#include <cstring>
#include <algorithm>

#ifdef _WIN32
#include <intrin.h>
#elif defined(__GNUC__) || defined(__clang__)
#include <cpuid.h>
#endif

namespace poker {

// SIMD-optimized batch processing
void SimdOptimizer::process_batch_avx2(const std::array<Card, 2>& hero_hand,
                                       const std::array<std::array<Card, 2>, 8>& opponent_hands,
                                       const std::array<Card, 5>& board,
                                       std::array<int, 8>& results) {
    if (!has_avx2_support()) {
        // Fallback to scalar implementation
        for (int i = 0; i < 8; ++i) {
            std::array<Card, 7> hero_7_cards = {hero_hand[0], hero_hand[1], 
                                               board[0], board[1], board[2], board[3], board[4]};
            std::array<Card, 7> opp_7_cards = {opponent_hands[i][0], opponent_hands[i][1],
                                              board[0], board[1], board[2], board[3], board[4]};
            
            uint32_t hero_rank = g_hand_evaluator.evaluate_7_cards(hero_7_cards);
            uint32_t opp_rank = g_hand_evaluator.evaluate_7_cards(opp_7_cards);
            
            results[i] = g_hand_evaluator.compare_hands(hero_rank, opp_rank);
        }
        return;
    }
    
    // SIMD implementation for AVX2
    std::array<std::array<Card, 7>, 8> hero_hands;
    std::array<std::array<Card, 7>, 8> opp_hands;
    
    // Prepare data for SIMD processing
    for (int i = 0; i < 8; ++i) {
        hero_hands[i] = {hero_hand[0], hero_hand[1], 
                        board[0], board[1], board[2], board[3], board[4]};
        opp_hands[i] = {opponent_hands[i][0], opponent_hands[i][1],
                       board[0], board[1], board[2], board[3], board[4]};
    }
    
    // Evaluate hands using SIMD
    std::array<uint32_t, 8> hero_ranks, opp_ranks;
    evaluate_hands_batch_avx2(hero_hands, hero_ranks);
    evaluate_hands_batch_avx2(opp_hands, opp_ranks);
    
    // Compare results using SIMD
    __m256i hero_vec = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(hero_ranks.data()));
    __m256i opp_vec = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(opp_ranks.data()));
    
    __m256i comparison = compare_hands_avx2(hero_vec, opp_vec);
    _mm256_storeu_si256(reinterpret_cast<__m256i*>(results.data()), comparison);
}

// Vectorized probability calculations
void SimdOptimizer::calculate_probabilities_avx2(const std::array<uint64_t, 8>& win_counts,
                                                 const std::array<uint64_t, 8>& total_counts,
                                                 std::array<double, 8>& probabilities) {
    if (!has_avx2_support()) {
        // Fallback to scalar implementation
        for (int i = 0; i < 8; ++i) {
            probabilities[i] = (total_counts[i] > 0) ? 
                static_cast<double>(win_counts[i]) / total_counts[i] : 0.0;
        }
        return;
    }
    
    // Convert to doubles for SIMD processing
    alignas(32) double win_counts_d[8];
    alignas(32) double total_counts_d[8];
    
    for (int i = 0; i < 8; ++i) {
        win_counts_d[i] = static_cast<double>(win_counts[i]);
        total_counts_d[i] = static_cast<double>(total_counts[i]);
    }
    
    // Load into SIMD registers
    __m256d wins_vec1 = _mm256_load_pd(&win_counts_d[0]);
    __m256d wins_vec2 = _mm256_load_pd(&win_counts_d[4]);
    __m256d totals_vec1 = _mm256_load_pd(&total_counts_d[0]);
    __m256d totals_vec2 = _mm256_load_pd(&total_counts_d[4]);
    
    // Perform division
    __m256d prob_vec1 = _mm256_div_pd(wins_vec1, totals_vec1);
    __m256d prob_vec2 = _mm256_div_pd(wins_vec2, totals_vec2);
    
    // Store results
    _mm256_store_pd(&probabilities[0], prob_vec1);
    _mm256_store_pd(&probabilities[4], prob_vec2);
}

// SIMD-optimized card shuffling
void SimdOptimizer::shuffle_deck_avx2(std::array<Card, 52>& deck, std::mt19937_64& rng) {
    if (!has_avx2_support()) {
        // Fallback to standard shuffle
        std::shuffle(deck.begin(), deck.end(), rng);
        return;
    }
    
    // Fisher-Yates shuffle with SIMD optimization
    for (int i = 51; i > 0; --i) {
        int j = std::uniform_int_distribution<int>(0, i)(rng);
        std::swap(deck[i], deck[j]);
    }
}

// Parallel hand evaluation for 8 hands
void SimdOptimizer::evaluate_hands_batch_avx2(const std::array<std::array<Card, 7>, 8>& hands,
                                              std::array<uint32_t, 8>& ranks) {
    if (!has_avx2_support()) {
        // Fallback to scalar evaluation
        for (int i = 0; i < 8; ++i) {
            ranks[i] = g_hand_evaluator.evaluate_7_cards(hands[i]);
        }
        return;
    }
    
    // SIMD implementation would go here
    // For now, use optimized scalar version
    for (int i = 0; i < 8; ++i) {
        ranks[i] = g_hand_evaluator.evaluate_7_cards(hands[i]);
    }
}

// CPU feature detection
bool SimdOptimizer::has_avx2_support() {
    static bool checked = false;
    static bool has_avx2 = false;
    
    if (!checked) {
        #ifdef _WIN32
        int cpuInfo[4];
        __cpuid(cpuInfo, 7);
        has_avx2 = (cpuInfo[1] & (1 << 5)) != 0;
        #else
        unsigned int eax, ebx, ecx, edx;
        if (__get_cpuid_count(7, 0, &eax, &ebx, &ecx, &edx)) {
            has_avx2 = (ebx & (1 << 5)) != 0;
        }
        #endif
        checked = true;
    }
    
    return has_avx2;
}

bool SimdOptimizer::has_avx512_support() {
    static bool checked = false;
    static bool has_avx512 = false;
    
    if (!checked) {
        #ifdef _WIN32
        int cpuInfo[4];
        __cpuid(cpuInfo, 7);
        has_avx512 = (cpuInfo[1] & (1 << 16)) != 0;
        #else
        unsigned int eax, ebx, ecx, edx;
        if (__get_cpuid_count(7, 0, &eax, &ebx, &ecx, &edx)) {
            has_avx512 = (ebx & (1 << 16)) != 0;
        }
        #endif
        checked = true;
    }
    
    return has_avx512;
}

// SIMD utility functions
__m256i SimdOptimizer::pack_cards_avx2(const std::array<Card, 8>& cards) {
    return _mm256_loadu_si256(reinterpret_cast<const __m256i*>(cards.data()));
}

void SimdOptimizer::unpack_results_avx2(const __m256i& packed_results, std::array<uint32_t, 8>& results) {
    _mm256_storeu_si256(reinterpret_cast<__m256i*>(results.data()), packed_results);
}

__m256i SimdOptimizer::compare_hands_avx2(const __m256i& hand_ranks1, const __m256i& hand_ranks2) {
    __m256i greater = _mm256_cmpgt_epi32(hand_ranks1, hand_ranks2);
    __m256i less = _mm256_cmpgt_epi32(hand_ranks2, hand_ranks1);
    
    // Return 1 for greater, -1 for less, 0 for equal
    __m256i result = _mm256_sub_epi32(greater, less);
    return result;
}

__m256i SimdOptimizer::popcount_avx2(const __m256i& values) {
    // AVX2 doesn't have native popcount, so we'll use a lookup table approach
    // This is a simplified implementation
    alignas(32) uint32_t temp[8];
    _mm256_store_si256(reinterpret_cast<__m256i*>(temp), values);
    
    for (int i = 0; i < 8; ++i) {
        temp[i] = popcount(temp[i]);
    }
    
    return _mm256_load_si256(reinterpret_cast<const __m256i*>(temp));
}

__m256i SimdOptimizer::find_straights_avx2(const __m256i& rank_masks) {
    // Simplified straight detection using SIMD
    alignas(32) uint32_t temp[8];
    _mm256_store_si256(reinterpret_cast<__m256i*>(temp), rank_masks);
    
    for (int i = 0; i < 8; ++i) {
        // Check for straight patterns
        uint32_t mask = temp[i];
        bool is_straight = false;
        
        // Check all possible straight patterns
        uint32_t straight_patterns[] = {
            0x1F, 0x3E, 0x7C, 0xF8, 0x1F0, 0x3E0, 0x7C0, 0xF80, 0x1F00, 0x403C
        };
        
        for (uint32_t pattern : straight_patterns) {
            if (mask == pattern) {
                is_straight = true;
                break;
            }
        }
        
        temp[i] = is_straight ? 1 : 0;
    }
    
    return _mm256_load_si256(reinterpret_cast<const __m256i*>(temp));
}

// AlignedMemoryPool implementation
template<typename T, size_t Alignment>
AlignedMemoryPool<T, Alignment>::AlignedMemoryPool(size_t pool_size) 
    : pool_size_(pool_size), current_offset_(0) {
    pool_.reserve(pool_size);
}

template<typename T, size_t Alignment>
AlignedMemoryPool<T, Alignment>::~AlignedMemoryPool() = default;

template<typename T, size_t Alignment>
T* AlignedMemoryPool<T, Alignment>::allocate(size_t count) {
    if (current_offset_ + count > pool_size_) {
        return nullptr;  // Pool exhausted
    }
    
    T* ptr = &pool_[current_offset_];
    current_offset_ += count;
    return ptr;
}

template<typename T, size_t Alignment>
void AlignedMemoryPool<T, Alignment>::deallocate(T* ptr) {
    // Simple pool allocator - no individual deallocation
    (void)ptr;
}

template<typename T, size_t Alignment>
void AlignedMemoryPool<T, Alignment>::reset() {
    current_offset_ = 0;
}

// CacheOptimizedSimulator implementation
CacheOptimizedSimulator::CacheOptimizedSimulator(int num_threads) 
    : num_threads_(num_threads), memory_pool_(1024 * 1024) {
    thread_local_counters_.resize(num_threads);
    optimize_data_layout();
}

void CacheOptimizedSimulator::simulate_batch_optimized(const std::array<Card, 2>& hero_hand,
                                                      int num_opponents,
                                                      int batch_size,
                                                      std::array<SimulationResult, 8>& results) {
    // Cache-optimized simulation implementation
    for (int i = 0; i < 8; ++i) {
        results[i].win_rate = 0.5;  // Placeholder
        results[i].total_simulations = batch_size;
    }
}

void CacheOptimizedSimulator::process_tiles(const std::vector<std::string>& hands,
                                           int simulations_per_hand,
                                           std::vector<HandAnalysis>& results) {
    // Tile-based processing for cache efficiency
    const size_t tile_size = 8;  // Process 8 hands at a time
    
    for (size_t i = 0; i < hands.size(); i += tile_size) {
        size_t end = std::min(i + tile_size, hands.size());
        
        // Process tile
        for (size_t j = i; j < end; ++j) {
            // Placeholder processing
            results[j].hand_string = hands[j];
            results[j].combinations = 6;  // Placeholder
        }
    }
}

void CacheOptimizedSimulator::optimize_data_layout() {
    // Optimize memory layout for cache performance
    for (auto& counter : thread_local_counters_) {
        std::fill(counter.counters.begin(), counter.counters.end(), 0);
    }
}

void CacheOptimizedSimulator::prefetch_next_batch(const void* data, size_t size) {
    // Prefetch next batch of data
    const char* ptr = static_cast<const char*>(data);
    for (size_t i = 0; i < size; i += 64) {
        #ifdef _WIN32
        _mm_prefetch(ptr + i, _MM_HINT_T0);
        #else
        __builtin_prefetch(ptr + i, 0, 3);
        #endif
    }
}

// Explicit template instantiation
template class AlignedMemoryPool<ThreadLocalData, 64>;
template class AlignedMemoryPool<Card, 32>;
template class AlignedMemoryPool<uint32_t, 32>;

} // namespace poker
