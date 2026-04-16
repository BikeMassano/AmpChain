cmake_minimum_required(VERSION 3.22)

project(NoiseGate VERSION 0.0.1)

add_subdirectory(JUCE)

juce_add_binary_data(Assets
    SOURCES
        Assets/images/body.png
	Assets/images/butOff.png
	Assets/images/butOn.png
	Assets/images/knob.png
	Assets/images/knob_shadow.png
	Assets/images/lampOff.png
	Assets/images/lampOn.png
)

juce_add_plugin(NoiseGate
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
    PRODUCT_NAME "NoiseGate"
)

target_sources(NoiseGate
    PRIVATE
        Source/PluginEditor.cpp
	Source/PluginEditor.h
        Source/PluginProcessor.cpp
	Source/PluginProcessor.h
	Source/ParamIDs.h
	Source/LookAndFeel/KnobLookAndFeel.h
	Source/Components/GateComponent.cpp
	Source/Components/GateComponent.h
	Source/Components/CompressorComponent.cpp
	Source/Components/CompressorComponent.h
)

target_compile_definitions(NoiseGate
    PUBLIC
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0)

target_link_libraries(NoiseGate
    PRIVATE
	Assets
        juce::juce_audio_processors
        juce::juce_gui_basics
        juce::juce_gui_extra
        juce::juce_dsp
        juce::juce_audio_utils
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
)

# target_include_directories(NoiseGate PRIVATE
#     ${CMAKE_BINARY_DIR}/juce_binarydata_Assets/JuceLibraryCode
# )
