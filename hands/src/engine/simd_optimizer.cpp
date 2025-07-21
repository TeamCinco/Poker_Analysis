#include "simd_optimizer.hpp"
#include <random>
#include <cstring>
#include <algorithm>

#ifdef _MSC_VER
#include <intrin.h>
#else
#include <cpuid.h>
#endif

namespace poker {

SIMDOptimizer::SIMDOptimizer() {
    detect_cpu_features();
}

void SIMDOptimizer::detect_cpu_features() {
    int cpu_info[4];
    
#ifdef _MSC_VER
    // Use MSVC intrinsic
    __cpuidex(cpu_info, 7, 0);
    has_avx2_ = (cpu_info[1] & (1 << 5)) != 0;
    has_avx512_ = (cpu_info[1] & (1 << 16)) != 0;
#else
    // Use GCC intrinsic
    unsigned int eax, ebx, ecx, edx;
    if (__get_cpuid_count(7, 0, &eax, &ebx, &ecx, &edx)) {
        has_avx2_ = (ebx & (1 << 5)) != 0;
        has_avx512_ = (ebx & (1 << 16)) != 0;
    } else {
        has_avx2_ = false;
        has_avx512_ = false;
    }
#endif
}

double SIMDOptimizer::test_single_frequency_scalar(
    const Scenario& scenario, 
    double frequency, 
    int simulations) {
    
    double total_ev = 0.0;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<double> dis(0.0, 1.0);
    
    for (int sim = 0; sim < simulations; ++sim) {
        bool take_action = dis(gen) < frequency;
        double equity = 0.4; // Simplified equity
        
        if (take_action) {
            total_ev += equity + 0.1; // Action bonus
        } else {
            total_ev += equity;
        }
    }
    
    return total_ev / simulations;
}

std::vector<double> SIMDOptimizer::batch_test_frequencies(
    const Scenario& scenario, 
    const std::vector<double>& frequencies,
    int simulations_per_frequency) {
    
    std::vector<double> results(frequencies.size());
    
    if (has_avx512_ && frequencies.size() >= 8) {
        // Process 8 frequencies at once with AVX-512
        size_t batches = frequencies.size() / 8;
        for (size_t i = 0; i < batches; ++i) {
            avx512_test_frequencies_batch(
                scenario,
                &frequencies[i * 8],
                &results[i * 8],
                8,
                simulations_per_frequency
            );
        }
        
        // Handle remaining frequencies
        for (size_t i = batches * 8; i < frequencies.size(); ++i) {
            // Fall back to scalar processing for remainder
            results[i] = test_single_frequency_scalar(scenario, frequencies[i], simulations_per_frequency);
        }
        
    } else if (has_avx2_ && frequencies.size() >= 4) {
        // Process 4 frequencies at once with AVX2
        size_t batches = frequencies.size() / 4;
        for (size_t i = 0; i < batches; ++i) {
            avx2_test_frequencies_batch(
                scenario,
                &frequencies[i * 4],
                &results[i * 4],
                4,
                simulations_per_frequency
            );
        }
        
        // Handle remaining frequencies
        for (size_t i = batches * 4; i < frequencies.size(); ++i) {
            results[i] = test_single_frequency_scalar(scenario, frequencies[i], simulations_per_frequency);
        }
        
    } else {
        // Fall back to scalar processing
        for (size_t i = 0; i < frequencies.size(); ++i) {
            results[i] = test_single_frequency_scalar(scenario, frequencies[i], simulations_per_frequency);
        }
    }
    
    return results;
}

void SIMDOptimizer::avx2_test_frequencies_batch(
    const Scenario& scenario,
    const double* frequencies,
    double* results,
    size_t count,
    int simulations) {
    
    // Load 4 frequencies into AVX2 register
    __m256d freq_vec = _mm256_load_pd(frequencies);
    
    // Initialize accumulators
    __m256d total_ev = _mm256_setzero_pd();
    
    // Simplified batch simulation - in practice this would be much more complex
    for (int sim = 0; sim < simulations; ++sim) {
        // Generate random decisions based on frequencies
        __m256d random_vec = simd_utils::VectorizedRNG(sim).next_avx2();
        __m256d decision_mask = _mm256_cmp_pd(random_vec, freq_vec, _CMP_LT_OQ);
        
        // Simulate equity for decisions (simplified)
        __m256d equity_vec = _mm256_set1_pd(0.4); // Base equity
        __m256d action_bonus = _mm256_set1_pd(0.1); // Action bonus
        
        // Add bonus if action taken
        __m256d sim_ev = _mm256_blendv_pd(_mm256_setzero_pd(), action_bonus, decision_mask);
        sim_ev = _mm256_add_pd(sim_ev, equity_vec);
        
        total_ev = _mm256_add_pd(total_ev, sim_ev);
    }
    
    // Average and store results
    __m256d avg_ev = _mm256_div_pd(total_ev, _mm256_set1_pd(simulations));
    _mm256_store_pd(results, avg_ev);
}

void SIMDOptimizer::avx512_test_frequencies_batch(
    const Scenario& scenario,
    const double* frequencies,
    double* results,
    size_t count,
    int simulations) {
    
    // Load 8 frequencies into AVX-512 register
    __m512d freq_vec = _mm512_load_pd(frequencies);
    
    // Initialize accumulators
    __m512d total_ev = _mm512_setzero_pd();
    
    for (int sim = 0; sim < simulations; ++sim) {
        // Generate random decisions
        __m512d random_vec = simd_utils::VectorizedRNG(sim).next_avx512();
        __mmask8 decision_mask = _mm512_cmp_pd_mask(random_vec, freq_vec, _CMP_LT_OQ);
        
        // Simulate equity
        __m512d equity_vec = _mm512_set1_pd(0.4);
        __m512d action_bonus = _mm512_set1_pd(0.1);
        
        __m512d sim_ev = _mm512_mask_add_pd(equity_vec, decision_mask, equity_vec, action_bonus);
        total_ev = _mm512_add_pd(total_ev, sim_ev);
    }
    
    // Average and store results
    __m512d avg_ev = _mm512_div_pd(total_ev, _mm512_set1_pd(simulations));
    _mm512_store_pd(results, avg_ev);
}

void SIMDOptimizer::vectorized_preflop_equity(
    const std::array<Card, 2>* hero_hands,
    int num_opponents,
    double* equities,
    size_t count) {
    
    if (has_avx512_ && count >= 8) {
        size_t batches = count / 8;
        
        for (size_t batch = 0; batch < batches; ++batch) {
            // Convert 8 hands to rank sums
            __m512d rank_sums = _mm512_setzero_pd();
            for (int i = 0; i < 8; ++i) {
                const auto& hand = hero_hands[batch * 8 + i];
                double rank_sum = static_cast<double>(static_cast<int>(hand[0].rank) + static_cast<int>(hand[1].rank));
                ((double*)&rank_sums)[i] = rank_sum;
            }
            
            // Calculate base equity
            __m512d base_equity = _mm512_div_pd(rank_sums, _mm512_set1_pd(30.0));
            base_equity = _mm512_min_pd(base_equity, _mm512_set1_pd(0.9));
            
            // Adjust for number of opponents
            __m512d opponent_factor = _mm512_set1_pd(1.0 + num_opponents * 0.15);
            __m512d adjusted_equity = _mm512_div_pd(base_equity, opponent_factor);
            adjusted_equity = _mm512_max_pd(adjusted_equity, _mm512_set1_pd(0.1));
            
            _mm512_store_pd(&equities[batch * 8], adjusted_equity);
        }
        
        // Handle remainder
        for (size_t i = batches * 8; i < count; ++i) {
            const auto& hand = hero_hands[i];
            double rank_sum = static_cast<double>(static_cast<int>(hand[0].rank) + static_cast<int>(hand[1].rank));
            double base_equity = std::min(0.9, rank_sum / 30.0);
            equities[i] = std::max(0.1, base_equity / (1.0 + num_opponents * 0.15));
        }
        
    } else if (has_avx2_ && count >= 4) {
        size_t batches = count / 4;
        
        for (size_t batch = 0; batch < batches; ++batch) {
            __m256d rank_sums = _mm256_setzero_pd();
            for (int i = 0; i < 4; ++i) {
                const auto& hand = hero_hands[batch * 4 + i];
                double rank_sum = static_cast<double>(static_cast<int>(hand[0].rank) + static_cast<int>(hand[1].rank));
                ((double*)&rank_sums)[i] = rank_sum;
            }
            
            __m256d base_equity = _mm256_div_pd(rank_sums, _mm256_set1_pd(30.0));
            base_equity = _mm256_min_pd(base_equity, _mm256_set1_pd(0.9));
            
            __m256d opponent_factor = _mm256_set1_pd(1.0 + num_opponents * 0.15);
            __m256d adjusted_equity = _mm256_div_pd(base_equity, opponent_factor);
            adjusted_equity = _mm256_max_pd(adjusted_equity, _mm256_set1_pd(0.1));
            
            _mm256_store_pd(&equities[batch * 4], adjusted_equity);
        }
        
        // Handle remainder
        for (size_t i = batches * 4; i < count; ++i) {
            const auto& hand = hero_hands[i];
            double rank_sum = static_cast<double>(static_cast<int>(hand[0].rank) + static_cast<int>(hand[1].rank));
            double base_equity = std::min(0.9, rank_sum / 30.0);
            equities[i] = std::max(0.1, base_equity / (1.0 + num_opponents * 0.15));
        }
        
    } else {
        // Scalar fallback
        for (size_t i = 0; i < count; ++i) {
            const auto& hand = hero_hands[i];
            double rank_sum = static_cast<double>(static_cast<int>(hand[0].rank) + static_cast<int>(hand[1].rank));
            double base_equity = std::min(0.9, rank_sum / 30.0);
            equities[i] = std::max(0.1, base_equity / (1.0 + num_opponents * 0.15));
        }
    }
}

// SIMD utilities implementation
namespace simd_utils {

VectorizedRNG::VectorizedRNG(unsigned seed) {
    seed_states(seed);
}

void VectorizedRNG::seed_states(unsigned seed) {
    std::mt19937 seeder(seed);
    
    // Seed AVX2 states
    for (int i = 0; i < 4; ++i) {
        state_[i] = seeder();
        state_[i] = (state_[i] << 32) | seeder();
    }
    
    // Seed AVX-512 states
    for (int i = 0; i < 8; ++i) {
        state_avx512_[i] = seeder();
        state_avx512_[i] = (state_avx512_[i] << 32) | seeder();
    }
}

__m256d VectorizedRNG::next_avx2() {
    // Simplified xoroshiro128+ implementation for 4 parallel streams
    __m256i s0 = _mm256_load_si256((__m256i*)state_);
    __m256i s1 = _mm256_load_si256((__m256i*)(state_ + 2));
    
    __m256i result = _mm256_add_epi64(s0, s1);
    
    s1 = _mm256_xor_si256(s1, s0);
    s0 = _mm256_or_si256(_mm256_slli_epi64(s0, 24), _mm256_srli_epi64(s0, 40));
    s0 = _mm256_xor_si256(s0, s1);
    s0 = _mm256_xor_si256(s0, _mm256_slli_epi64(s1, 16));
    s1 = _mm256_or_si256(_mm256_slli_epi64(s1, 37), _mm256_srli_epi64(s1, 27));
    
    _mm256_store_si256((__m256i*)state_, s0);
    _mm256_store_si256((__m256i*)(state_ + 2), s1);
    
    // Convert to double [0,1)
    __m256i mantissa = _mm256_srli_epi64(result, 12);
    mantissa = _mm256_or_si256(mantissa, _mm256_set1_epi64x(0x3FF0000000000000ULL));
    __m256d double_result = _mm256_castsi256_pd(mantissa);
    return _mm256_sub_pd(double_result, _mm256_set1_pd(1.0));
}

__m512d VectorizedRNG::next_avx512() {
    // Similar implementation for AVX-512 with 8 parallel streams
    __m512i s0 = _mm512_load_si512((__m512i*)state_avx512_);
    __m512i s1 = _mm512_load_si512((__m512i*)(state_avx512_ + 4));
    
    __m512i result = _mm512_add_epi64(s0, s1);
    
    s1 = _mm512_xor_si512(s1, s0);
    s0 = _mm512_or_si512(_mm512_slli_epi64(s0, 24), _mm512_srli_epi64(s0, 40));
    s0 = _mm512_xor_si512(s0, s1);
    s0 = _mm512_xor_si512(s0, _mm512_slli_epi64(s1, 16));
    s1 = _mm512_or_si512(_mm512_slli_epi64(s1, 37), _mm512_srli_epi64(s1, 27));
    
    _mm512_store_si512((__m512i*)state_avx512_, s0);
    _mm512_store_si512((__m512i*)(state_avx512_ + 4), s1);
    
    // Convert to double [0,1)
    __m512i mantissa = _mm512_srli_epi64(result, 12);
    mantissa = _mm512_or_si512(mantissa, _mm512_set1_epi64(0x3FF0000000000000ULL));
    __m512d double_result = _mm512_castsi512_pd(mantissa);
    return _mm512_sub_pd(double_result, _mm512_set1_pd(1.0));
}

} // namespace simd_utils

} // namespace poker
