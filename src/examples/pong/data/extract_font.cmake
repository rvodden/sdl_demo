# Extract script for DejaVu Sans Mono font
file(ARCHIVE_EXTRACT
    INPUT ${INPUT}
    PATTERNS DejaVuSansMono.ttf
    DESTINATION ${DESTINATION}
)