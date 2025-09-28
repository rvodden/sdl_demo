# Shader compilation using CMake
# Automatically compiles GLSL to SPIRV and generates C headers

find_program(GLSLANG_VALIDATOR glslang REQUIRED)

# Function to compile a GLSL shader to SPIRV and generate C header
function(compile_shader_to_header SHADER_FILE OUTPUT_VAR_NAME)
    get_filename_component(SHADER_NAME ${SHADER_FILE} NAME_WE)
    get_filename_component(SHADER_EXT ${SHADER_FILE} EXT)

    # Remove the dot from extension (.vert -> vert)
    string(SUBSTRING ${SHADER_EXT} 1 -1 SHADER_STAGE)

    set(SPIRV_FILE "${CMAKE_CURRENT_BINARY_DIR}/${SHADER_NAME}.${SHADER_STAGE}.spv")
    set(HEADER_FILE "${CMAKE_CURRENT_BINARY_DIR}/${SHADER_NAME}_${SHADER_STAGE}.h")

    # Compile GLSL to SPIRV
    add_custom_command(
        OUTPUT ${SPIRV_FILE}
        COMMAND ${GLSLANG_VALIDATOR} -V ${CMAKE_CURRENT_SOURCE_DIR}/${SHADER_FILE} -o ${SPIRV_FILE}
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/${SHADER_FILE}
        COMMENT "Compiling ${SHADER_FILE} to SPIRV"
        VERBATIM
    )

    # Generate C header from SPIRV
    add_custom_command(
        OUTPUT ${HEADER_FILE}
        COMMAND ${GLSLANG_VALIDATOR} -V ${CMAKE_CURRENT_SOURCE_DIR}/${SHADER_FILE} --vn ${OUTPUT_VAR_NAME} -o ${HEADER_FILE}
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/${SHADER_FILE}
        COMMENT "Generating C header ${HEADER_FILE} with variable ${OUTPUT_VAR_NAME}"
        VERBATIM
    )

    # Make the header file available in parent scope
    set(SHADER_HEADER_${OUTPUT_VAR_NAME} ${HEADER_FILE} PARENT_SCOPE)
    set(SHADER_SPIRV_${OUTPUT_VAR_NAME} ${SPIRV_FILE} PARENT_SCOPE)
endfunction()

# Compile our shaders
compile_shader_to_header(triangle.vert triangleVertexSpirv)
compile_shader_to_header(triangle.frag triangleFragmentSpirv)

# Bloom effect shaders
compile_shader_to_header(fullscreen.vert fullscreenVertexSpirv)
compile_shader_to_header(scene.vert sceneVertexSpirv)
compile_shader_to_header(scene.frag sceneFragmentSpirv)
compile_shader_to_header(bright_pass.frag brightPassFragmentSpirv)
compile_shader_to_header(blur.frag blurFragmentSpirv)
compile_shader_to_header(composite.frag compositeFragmentSpirv)
compile_shader_to_header(copy.frag copyFragmentSpirv)

# Create a custom target that depends on all generated headers
add_custom_target(compile_shaders ALL
    DEPENDS
        ${SHADER_HEADER_triangleVertexSpirv}
        ${SHADER_HEADER_triangleFragmentSpirv}
        ${SHADER_SPIRV_triangleVertexSpirv}
        ${SHADER_SPIRV_triangleFragmentSpirv}
        ${SHADER_HEADER_fullscreenVertexSpirv}
        ${SHADER_HEADER_sceneVertexSpirv}
        ${SHADER_HEADER_sceneFragmentSpirv}
        ${SHADER_HEADER_brightPassFragmentSpirv}
        ${SHADER_HEADER_blurFragmentSpirv}
        ${SHADER_HEADER_compositeFragmentSpirv}
        ${SHADER_HEADER_copyFragmentSpirv}
        ${SHADER_SPIRV_fullscreenVertexSpirv}
        ${SHADER_SPIRV_sceneVertexSpirv}
        ${SHADER_SPIRV_sceneFragmentSpirv}
        ${SHADER_SPIRV_brightPassFragmentSpirv}
        ${SHADER_SPIRV_blurFragmentSpirv}
        ${SHADER_SPIRV_compositeFragmentSpirv}
        ${SHADER_SPIRV_copyFragmentSpirv}
    COMMENT "Compiling all shaders"
)

# Create an interface library to provide the shader headers
add_library(shader_headers INTERFACE)

# Add the binary directory to the include path so generated headers can be found
target_include_directories(shader_headers INTERFACE ${CMAKE_CURRENT_BINARY_DIR})

# Make sure shaders are compiled before anyone tries to use the headers
add_dependencies(shader_headers compile_shaders)

# Optionally validate SPIRV if spirv-val is available
find_program(SPIRV_VAL spirv-val)
if(SPIRV_VAL)
    add_custom_target(validate_shaders
        COMMAND ${SPIRV_VAL} ${SHADER_SPIRV_triangleVertexSpirv}
        COMMAND ${SPIRV_VAL} ${SHADER_SPIRV_triangleFragmentSpirv}
        COMMAND ${SPIRV_VAL} ${SHADER_SPIRV_fullscreenVertexSpirv}
        COMMAND ${SPIRV_VAL} ${SHADER_SPIRV_sceneVertexSpirv}
        COMMAND ${SPIRV_VAL} ${SHADER_SPIRV_sceneFragmentSpirv}
        COMMAND ${SPIRV_VAL} ${SHADER_SPIRV_brightPassFragmentSpirv}
        COMMAND ${SPIRV_VAL} ${SHADER_SPIRV_blurFragmentSpirv}
        COMMAND ${SPIRV_VAL} ${SHADER_SPIRV_compositeFragmentSpirv}
        COMMAND ${SPIRV_VAL} ${SHADER_SPIRV_copyFragmentSpirv}
        DEPENDS compile_shaders
        COMMENT "Validating compiled SPIRV"
    )
    add_dependencies(shader_headers validate_shaders)
endif()

# Export variables for parent CMakeLists.txt
set(TRIANGLE_VERTEX_HEADER ${SHADER_HEADER_triangleVertexSpirv} PARENT_SCOPE)
set(TRIANGLE_FRAGMENT_HEADER ${SHADER_HEADER_triangleFragmentSpirv} PARENT_SCOPE)