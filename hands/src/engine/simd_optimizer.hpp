#pragma once

#include "../core/types.hpp"
#include <array>
#include <vector>
#include <immintrin.h>

namespace poker {

class SIMDOptimizer {
public:
    SIMDOptimizer();
    ~SIMDOptimizer() = default;

    // SIMD-optimized batch operations
    std::vector<double> batch_test_frequencies(
        const Scenario& scenario, 
        const std::vector<double>& frequencies,
        int simulations_per_frequency);

    // Vectorized equity calculations
    std::vector<double> batch_calculate_equity(
        const std::vector<std::array<Card, 2>>& hero_hands,
        const std::vector<std::array<Card, 2>>& opponent_hands,
        const std::vector<Card>& board);

    // Parallel random number generation
    void generate_random_batch(double* output, size_t count, unsigned seed);

    // Check CPU capabilities
    bool has_avx2() const { return has_avx2_; }
    bool has_avx512() const { return has_avx512_; }
    
private:
    bool has_avx2_;
    bool has_avx512_;
    
    // AVX2 implementations (256-bit, 4 doubles)
    void avx2_test_frequencies_batch(
        const Scenario& scenario,
        const double* frequencies,
        double* results,
        size_t count,
        int simulations);
    
    void avx2_calculate_equity_batch(
        const std::array<Card, 2>* hero_hands,
        const std::array<Card, 2>* opponent_hands,
        const Card* board,
        size_t board_size,
        double* results,
        size_t count);
    
    // AVX-512 implementations (512-bit, 8 doubles)  
    void avx512_test_frequencies_batch(
        const Scenario& scenario,
        const double* frequencies,
        double* results,
        size_t count,
        int simulations);
    
    void avx512_calculate_equity_batch(
        const std::array<Card, 2>* hero_hands,
        const std::array<Card, 2>* opponent_hands,
        const Card* board,
        size_t board_size,
        double* results,
        size_t count);
    
    // CPU feature detection
    void detect_cpu_features();
    
    // Vectorized hand strength calculation
    void vectorized_hand_strength(
        const std::array<Card, 2>* hands,
        double* strengths,
        size_t count);
    
    // Vectorized preflop equity estimation
    void vectorized_preflop_equity(
        const std::array<Card, 2>* hero_hands,
        int num_opponents,
        double* equities,
        size_t count);
    
    // Scalar fallback for single frequency testing
    double test_single_frequency_scalar(
        const Scenario& scenario, 
        double frequency, 
        int simulations);
};

// Utility functions for SIMD operations
namespace simd_utils {
    
    // Convert card ranks to vectorized format
    void cards_to_simd_ranks(__m256i& ranks_vec, const std::array<Card, 2>* hands, size_t count);
    void cards_to_simd_ranks_avx512(__m512i& ranks_vec, const std::array<Card, 2>* hands, size_t count);
    
    // Vectorized comparison operations
    __m256d compare_hand_strengths_avx2(__m256d hero_strengths, __m256d opp_strengths);
    __m512d compare_hand_strengths_avx512(__m512d hero_strengths, __m512d opp_strengths);
    
    // Fast vectorized RNG using xoroshiro128+
    class VectorizedRNG {
    public:
        VectorizedRNG(unsigned seed);
        
        // Generate 4 random doubles with AVX2
        __m256d next_avx2();
        
        // Generate 8 random doubles with AVX-512
        __m512d next_avx512();
        
    private:
        alignas(32) uint64_t state_[4];  // For AVX2
        alignas(64) uint64_t state_avx512_[8];  // For AVX-512
        
        void seed_states(unsigned seed);
    };
    
} // namespace simd_utils

} // namespace poker
