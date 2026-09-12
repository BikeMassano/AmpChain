cmake_minimum_required(VERSION 3.22)
project(AmpChain VERSION 0.0.1)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(NAM_SAMPLE_FLOAT ON CACHE BOOL "" FORCE)
set(DSP_SAMPLE_FLOAT ON CACHE BOOL "" FORCE)
add_subdirectory(modules/JUCE)
add_subdirectory(modules/RTNeural)

file(GLOB ASSET_FILES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/models/*.json
    ${CMAKE_CURRENT_SOURCE_DIR}/Assets/images/*.png
)
juce_add_binary_data(Assets SOURCES ${ASSET_FILES})

juce_add_plugin(AmpChain
    COMPANY_NAME "Yurii"
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    PLUGIN_MANUFACTURER_CODE Yr91
    PLUGIN_CODE NgG9
    FORMATS VST3 Standalone
    PRODUCT_NAME "AmpChain"
)

file(GLOB APP CONFIGURE_DEPENDS Source/App/*.cpp Source/App/*.h)
file(GLOB DSP CONFIGURE_DEPENDS Source/DSP/*.h Source/DSP/*.cpp)
file(GLOB UI CONFIGURE_DEPENDS
    Source/UI/LookAndFeel/*.h
    Source/UI/Components/*.cpp
    Source/UI/Components/*.h
    Source/UI/Components/Pages/*.cpp
    Source/UI/Components/Pages/*.h
)

target_sources(AmpChain
    PRIVATE
        ${APP}
        ${DSP}
        ${UI}
        ${CMAKE_SOURCE_DIR}/modules/NeuralAmpModelerCore/NAM/wavenet/model.cpp
        ${CMAKE_SOURCE_DIR}/modules/NeuralAmpModelerCore/NAM/wavenet/slimmable.cpp
        ${CMAKE_SOURCE_DIR}/modules/NeuralAmpModelerCore/NAM/wavenet/a2_fast.cpp
        ${CMAKE_SOURCE_DIR}/modules/NeuralAmpModelerCore/NAM/activations.cpp
        ${CMAKE_SOURCE_DIR}/modules/NeuralAmpModelerCore/NAM/dsp.cpp
        ${CMAKE_SOURCE_DIR}/modules/NeuralAmpModelerCore/NAM/get_dsp.cpp
        ${CMAKE_SOURCE_DIR}/modules/NeuralAmpModelerCore/NAM/convnet.cpp
        ${CMAKE_SOURCE_DIR}/modules/NeuralAmpModelerCore/NAM/lstm.cpp
        ${CMAKE_SOURCE_DIR}/modules/NeuralAmpModelerCore/NAM/conv1d.cpp
        ${CMAKE_SOURCE_DIR}/modules/NeuralAmpModelerCore/NAM/container.cpp
	${CMAKE_SOURCE_DIR}/modules/NeuralAmpModelerCore/NAM/ring_buffer.cpp
)

target_compile_definitions(AmpChain
    PUBLIC
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0
        JUCE_ASIO=1
        NAM_SAMPLE_FLOAT
        DSP_SAMPLE_FLOAT
        NAM_ENABLE_A2_FAST
)

target_link_libraries(AmpChain
    PRIVATE
        RTNeural
        Assets
        juce::juce_audio_processors
        juce::juce_gui_basics
        juce::juce_gui_extra
        juce::juce_dsp
        juce::juce_audio_utils
        juce::juce_audio_devices
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
)

target_include_directories(AmpChain SYSTEM PRIVATE
    ${CMAKE_SOURCE_DIR}/ASIOSDK
    ${CMAKE_SOURCE_DIR}/ASIOSDK/common
    ${CMAKE_SOURCE_DIR}/modules
    ${CMAKE_SOURCE_DIR}/modules/NeuralAmpModelerCore/Dependencies/eigen
    ${CMAKE_SOURCE_DIR}/modules/NeuralAmpModelerCore
    ${CMAKE_SOURCE_DIR}/modules/NeuralAmpModelerCore/Dependencies
    ${CMAKE_SOURCE_DIR}/modules/NeuralAmpModelerCore/Dependencies/nlohmann
)