# Download script for DejaVu Sans Mono font
file(DOWNLOAD 
    ${URL}
    ${OUTPUT}
    SHOW_PROGRESS
    EXPECTED_HASH SHA256=${EXPECTED_HASH}
)