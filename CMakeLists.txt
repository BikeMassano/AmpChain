cmake_minimum_required(VERSION 3.22)

project(AmpChain VERSION 0.0.1)

add_subdirectory(JUCE)

file(GLOB ASSET_FILES CONFIGURE_DEPENDS
	${CMAKE_CURRENT_SOURCE_DIR}/models/*.json
	${CMAKE_CURRENT_SOURCE_DIR}/Assets/images/*.png
)

juce_add_binary_data(Assets
    SOURCES
        ${ASSET_FILES}
)

juce_add_plugin(AmpChain
    # VERSION ...
    # ICON_BIG ...
    # ICON_SMALL ...
    COMPANY_NAME "Yurii"
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    # EDITOR_WANTS_KEYBOARD_FOCUS TRUE/FALSE
    # COPY_PLUGIN_AFTER_BUILD TRUE/FALSE
    PLUGIN_MANUFACTURER_CODE Yr91
    PLUGIN_CODE NgG9
                                                
    FORMATS VST3 Standalone
    PRODUCT_NAME "AmpChain"
)

file(GLOB APP CONFIGURE_DEPENDS
    	Source/App/*.cpp
    	Source/App/*.h
)

file(GLOB DSP CONFIGURE_DEPENDS 
	Source/DSP/*.h
	Source/DSP/*.cpp
)
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
)

target_compile_definitions(AmpChain
    PUBLIC
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0
	JUCE_ASIO=1
)

target_link_libraries(AmpChain
    PRIVATE
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

target_include_directories(AmpChain PRIVATE
	${CMAKE_SOURCE_DIR}/ASIOSDK
	${CMAKE_SOURCE_DIR}/ASIOSDK/common

	${CMAKE_SOURCE_DIR}/Source/DSP/NAM
    	${CMAKE_SOURCE_DIR}/Source/DSP/NAM/Dependencies/eigen
    	${CMAKE_SOURCE_DIR}/Source/DSP/NAM/Dependencies/nlohmann
)
