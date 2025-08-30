#pragma once

#include <JuceHeader.h>
#include "DJAudioPlayer.h"
#include "DeckGUI.h"
#include "PlaylistComponent.h"
#include "VinylSpinner.h"
#include "CustomLookAndFeel.h"
#include "SampleAudioSource.h"
#include "SpectrumBars.h"


class MainComponent  : public juce::AudioAppComponent
{
public:
    //==============================================================================
    MainComponent();
    ~MainComponent() override;

    //==============================================================================
    void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;

    //==============================================================================
    void paint (juce::Graphics& g) override;
    void resized() override;


    // theme
    CustomLookAndFeel custLnF;

    // for pads
    void triggerPad(const juce::String& id) { sampleBank.trigger(id, 1.0f); }


private:
    juce::AudioFormatManager formatManager;
    juce::AudioThumbnailCache thumbCache{ 100 };


    DJAudioPlayer player1{ formatManager };
    DJAudioPlayer player2{ formatManager };

    DeckGUI deckGUI1{ &player1, formatManager, thumbCache };
    DeckGUI deckGUI2{ &player2, formatManager, thumbCache };

    juce::MixerAudioSource mixerSource;

    PlaylistComponent playlistComponent1{ player1, deckGUI1, formatManager, "1"};
    PlaylistComponent playlistComponent2{ player2, deckGUI2, formatManager, "2"};

    SampleAudioSource sampleBank;

    // mixing
    MixerStrip mixerStrip;         

    // crossfade helper
    void applyCrossfade(float x);  

    // bars
    SpectrumBars playlistGapViz{ 10 /*1024*/, 16 /*bars*/ };


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
