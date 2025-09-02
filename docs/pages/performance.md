# Performance & Benchmarks {#performance}

## Overview

SDL++ is designed with **zero-abstraction** principles, providing a modern C++ interface over SDL3 with minimal performance overhead. This page documents our comprehensive benchmarking approach and results.

## Key Performance Claims

- **Sub-nanosecond to low-nanosecond overhead** for most operations
- **Direct SDL3 API calls** with minimal wrapper logic  
- **No heap allocations** in performance-critical paths
- **RAII cleanup** with virtually no runtime cost

## Benchmark Suite

Our automated benchmark suite compares SDL++ wrapper performance against pure SDL3 C API calls across multiple categories:

### Window Operations
- Window creation and destruction
- Size queries and title management
- Property getters and setters

### Renderer Operations  
- Renderer creation and setup
- Drawing color management
- Clear and present operations
- Scaling and output size queries

### Texture Operations
- Texture creation from pixels and files
- Texture rendering (full and partial)
- Blend mode configuration
- Size and property queries

### Surface Operations
- Surface creation and pixel access
- Fill operations and blitting
- Memory management

### Composite Operations
- Complete render loops
- Multi-texture scenarios
- Real-world usage patterns

## Live Benchmark Results

🚀 **[View Latest Benchmark Results](https://vodden.com/sdl_demo/benchmarks/)**

The benchmark results page shows:
- **Performance comparisons** between SDL++ and pure SDL3
- **Overhead analysis** with percentage differences  
- **Cross-platform results** (Ubuntu and Windows)
- **Historical trends** and performance regression detection

## Benchmark Infrastructure

### Automated Testing
- **GitHub Actions** run benchmarks on every main branch push
- **Multi-platform testing** on Ubuntu Linux and Windows
- **Release mode optimization** for accurate performance measurement
- **Statistical analysis** with multiple repetitions

### Benchmark Tools
- **Google Benchmark** framework for precise measurements
- **JSON output** for automated analysis
- **HTML report generation** with interactive charts
- **Integration** with GitHub Pages for public access

## Performance Validation Results

Based on comprehensive benchmarking, our analysis shows:

### ✅ Validated Claims
- **Window operations**: <5% overhead for most operations
- **Renderer drawing**: <15% overhead for all operations  
- **Texture operations**: <5% overhead for creation and rendering
- **Surface operations**: Varies by complexity, generally <20%

### 🎯 Key Findings
- **Most operations** show overhead measured in nanoseconds, not microseconds
- **Real-world impact** is negligible compared to GPU operations
- **Type safety and RAII** benefits far outweigh minimal performance cost
- **Some operations** (like `drawLine`) are actually faster due to compiler optimizations

### ⚡ Performance Recommendations
1. **Use SDL++ confidently** - overhead is truly minimal
2. **Focus optimization elsewhere** - GPU operations and algorithms matter 1000x more
3. **Profile your specific use case** if performance is critical
4. **Consider batching** for operations called millions of times per frame

## Benchmark Methodology

### Measurement Approach
- **Release mode builds** with full optimization (-O3)
- **Warm-up iterations** to stabilize CPU performance
- **Multiple repetitions** with statistical aggregation
- **Noise filtering** and outlier detection

### Test Environment
- **Standardized hardware** via GitHub Actions runners
- **Consistent OS environments** (Ubuntu 22.04, Windows Server 2022)  
- **Clean baseline** with minimal background processes
- **Reproducible builds** with fixed compiler versions

### Data Analysis
- **Mean and median** timing comparisons
- **Standard deviation** and confidence intervals
- **Overhead percentages** with clear categorization
- **Regression detection** across builds

## Contributing to Benchmarks

### Adding New Benchmarks
1. Create benchmark files in `src/sdl/benchmark/`
2. Follow the existing pattern with SDL3 vs SDL++ comparisons
3. Add appropriate CMake integration
4. Test locally before submitting PR

### Benchmark Guidelines
- **Focus on realistic usage** patterns
- **Measure actual overhead**, not artificial scenarios
- **Include both micro and macro** benchmarks
- **Document expected results** and variations

### Performance Testing
```bash
# Enable benchmarks during configuration
cmake --preset ninja-mc -DSDLXX_ENABLE_BENCHMARKS=ON

# Build benchmark suite
cmake --build --preset ninja-mc-release --target sdl_abstraction_benchmarks

# Run benchmarks locally
./build/ninja-mc/bin/benchmarks/Release/sdl_abstraction_benchmarks
```

## Performance Philosophy

SDL++ prioritizes:
1. **Developer productivity** through type safety and RAII
2. **Code maintainability** with clear C++ interfaces  
3. **Performance transparency** with documented overhead
4. **Real-world usability** over synthetic benchmarks

The minimal overhead demonstrated by our benchmarks validates that modern C++ abstractions can provide significant developer benefits without sacrificing performance.

---

*Benchmark results are updated automatically on every main branch commit. Visit the [live results page](https://vodden.com/sdl_demo/benchmarks/) for the latest performance data.*