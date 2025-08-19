#pragma once

#include <JuceHeader.h>
#include "DJAudioPlayer.h"
#include "DeckGUI.h"
#include "PlaylistComponent.h"
#include "VinylSpinner.h"
#include "CustomLookAndFeel.h"


//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
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



private:
    //==============================================================================
    // Your private member variables go here...

    juce::AudioFormatManager formatManager;
    juce::AudioThumbnailCache thumbCache{ 100 };


    DJAudioPlayer player1{ formatManager };
    DJAudioPlayer player2{ formatManager };

    DeckGUI deckGUI1{ &player1, formatManager, thumbCache };
    DeckGUI deckGUI2{ &player2, formatManager, thumbCache };

    juce::MixerAudioSource mixerSource;

    PlaylistComponent playlistComponent1{ player1, deckGUI1, formatManager, "1"};
    PlaylistComponent playlistComponent2{ player2, deckGUI2, formatManager, "2"};

    //// vinyl spinners
    //VinylSpinner vinyl1{ &player1, 33.333 };
    //VinylSpinner vinyl2{ &player2, 33.333 };

    //// vinyl selection
    //juce::ComboBox vinylSelectLeft;
    //juce::ComboBox vinylSelectRight;

    //juce::StringArray vinylOptions;
    //juce::Array<juce::File> vinylFiles;

    //void scanVinylAssets();
    //void setSpinnerImageFromIndex(bool left, int index);
    //static juce::Image tryLoadImage(const juce::File& f);
    //static juce::File getVinylsFolder();





    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
