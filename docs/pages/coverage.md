# Code Coverage {#coverage}

SDL++ includes comprehensive code coverage analysis to help maintain high code quality and ensure thorough testing of the codebase.

## Coverage Integration

Code coverage is seamlessly integrated into the documentation build system and generates professional HTML reports that are accessible directly from this documentation site.

### Features

- **Line Coverage**: Shows which lines of code are executed by tests
- **Function Coverage**: Tracks which functions are called during test execution  
- **Branch Coverage**: Analyzes conditional branch execution paths
- **Visual Reports**: Beautiful HTML reports with syntax highlighting
- **Integrated Navigation**: Coverage reports are styled to match this documentation

### Coverage Metrics

Our coverage analysis focuses on:

- **Core SDL Layer**: All wrapper classes and RAII implementations
- **Tools Layer**: Event routing, sprite system, and UI components
- **Application Framework**: Service registry and lifecycle management
- **Extensions**: TTF text rendering and other optional modules

External dependencies and build system files are automatically excluded from coverage analysis.

## Building with Coverage

Coverage analysis requires a special build configuration to instrument the code with coverage data collection.

### Prerequisites

Coverage analysis requires the following tools:
- `gcov` - Coverage data collection (part of GCC)
- `lcov` - Coverage data processing  
- `genhtml` - HTML report generation

On Ubuntu/Debian systems:
```bash
sudo apt install lcov
```

### Build Commands

**Configure with coverage:**
```bash
cmake --preset ninja-mc-coverage
```

**Build with coverage instrumentation:**  
```bash
cmake --build --preset ninja-mc-coverage
```

**Run tests to collect coverage data:**
```bash
ctest --preset ninja-mc-coverage
```

**Generate documentation with integrated coverage:**
```bash
cmake --build --preset ninja-mc-coverage --target docs
```

Alternatively, you can generate just the coverage report:
```bash
cmake --build --preset ninja-mc-coverage --target coverage
```

### Coverage Workflow

1. **Instrumentation**: Coverage flags are added to all project targets
2. **Test Execution**: Running tests generates coverage data files  
3. **Data Processing**: lcov processes the raw coverage data
4. **Filtering**: External dependencies and test files are excluded
5. **Report Generation**: Beautiful HTML reports are created
6. **Integration**: Reports are embedded in this documentation site

## Coverage Reports

When coverage is enabled, you can access detailed coverage reports at:

- **Coverage Summary**: Overview of overall project coverage
- **File-by-File**: Detailed coverage for each source file
- **Function Coverage**: Per-function coverage analysis
- **Branch Analysis**: Conditional branch coverage details

The coverage reports use the same styling as this documentation for a consistent experience.

## Development Workflow

For active development with coverage analysis:

```bash
# One-time setup
cmake --preset ninja-mc-coverage

# Development cycle
cmake --build --preset ninja-mc-coverage
ctest --preset ninja-mc-coverage  
cmake --build --preset ninja-mc-coverage --target coverage-html

# View coverage report
open build/ninja-mc/docs/output/html/coverage/index.html
```

## Coverage Targets

The build system provides several coverage-related targets:

- `coverage-clean`: Remove all coverage data
- `coverage-collect`: Gather coverage data from test runs
- `coverage-html`: Generate HTML coverage reports  
- `coverage`: Complete workflow (clean → test → report)

## Performance Impact

Coverage instrumentation adds debugging information and runtime hooks that impact performance:

- **Build Time**: Slightly increased due to additional instrumentation
- **Runtime**: Significant performance impact due to data collection
- **Binary Size**: Larger binaries due to debugging information

**Important**: Coverage builds are for analysis only. Never use coverage-instrumented binaries in production.

## Integration with CI/CD

The coverage system is designed to integrate seamlessly with continuous integration:

1. **GitHub Actions**: Coverage reports can be generated and deployed to GitHub Pages
2. **Pull Requests**: Coverage changes can be tracked across code reviews
3. **Quality Gates**: Minimum coverage thresholds can be enforced

## Coverage Best Practices

### Writing Testable Code

- **Small Functions**: Easier to achieve complete coverage
- **Clear Logic**: Reduce complex branching conditions  
- **Error Paths**: Test both success and failure scenarios
- **Edge Cases**: Cover boundary conditions and special values

### Coverage Goals

- **Core Layer**: Target 90%+ coverage for critical wrapper code
- **Tools Layer**: 85%+ coverage for game development utilities  
- **Application Framework**: 80%+ coverage for lifecycle management
- **New Features**: All new code should include comprehensive tests

### Coverage Analysis

Use coverage reports to:
- **Identify Gaps**: Find untested code paths
- **Refactor Opportunities**: Spot overly complex functions
- **Test Quality**: Ensure tests exercise realistic scenarios  
- **Regression Prevention**: Maintain coverage levels over time

## Technical Details

### Compiler Flags

Coverage instrumentation uses these GCC flags:
- `--coverage`: Enable coverage data generation
- `-fprofile-arcs`: Generate arc profiling information
- `-ftest-coverage`: Enable coverage analysis  
- `-O0`: Disable optimizations for accurate line mapping

### Data Processing

The coverage pipeline:
1. **gcov**: Generates `.gcda` and `.gcno` files during execution
2. **lcov**: Combines and processes coverage data into `.info` files
3. **genhtml**: Converts processed data into navigable HTML reports

### File Filtering

Coverage analysis automatically excludes:
- External dependencies (`/usr/*`, `*/_deps/*`)
- Test files (`*/test/*`, `*/tests/*`)
- Build system files (`*/build/*`, `*/external/*`)

Only project source files in `*/src/*` are included in coverage analysis.

The coverage system provides comprehensive code quality insights while maintaining the professional presentation standards of this documentation.