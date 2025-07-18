#include "types.hpp"
#include <iostream>

namespace poker {

// CPU feature detection functionality
// This file serves as a placeholder for future CPU feature detection extensions

void detect_cpu_features() {
    std::cout << "CPU Feature Detection:" << std::endl;
    
#ifdef __AVX2__
    std::cout << "  AVX2: Supported" << std::endl;
#else
    std::cout << "  AVX2: Not supported" << std::endl;
#endif

#ifdef __AVX512F__
    std::cout << "  AVX-512: Supported" << std::endl;
#else
    std::cout << "  AVX-512: Not supported" << std::endl;
#endif

#ifdef __SSE4_2__
    std::cout << "  SSE4.2: Supported" << std::endl;
#else
    std::cout << "  SSE4.2: Not supported" << std::endl;
#endif

    std::cout << std::endl;
}

} // namespace poker
