#pragma once

#include <JuceHeader.h>
#include "DJAudioPlayer.h"
#include "WaveformDisplay.h"


// avoid circular include errors
class PlaylistComponent;

//==============================================================================
/*
*/
class DeckGUI  : public juce::Component, 
                 public juce::Button::Listener, 
                 public juce::Slider::Listener,
                 public juce::FileDragAndDropTarget,
                 public juce::Timer,
                 public juce::ChangeListener
{
public:
    DeckGUI(DJAudioPlayer* player, 
            juce::AudioFormatManager& formatManagerToUse,
            juce::AudioThumbnailCache& cacheToUse
        );
    ~DeckGUI() override;

    void paint (juce::Graphics&) override;
    void resized() override;

    // implement Button::Listener
    void buttonClicked(juce::Button* button) override;

    // implement Slider::Listener
    void sliderValueChanged(juce::Slider* slider) override;

    bool isInterestedInFileDrag(const juce::StringArray& files) override;
    void filesDropped(const juce::StringArray& files, int x, int y) override;

    // timer
    void timerCallback() override;

    // change listener to subscribe to player
    void changeListenerCallback(juce::ChangeBroadcaster* source) override;

    // show file waveform
    void showWaveForm(juce::URL url);

    // allow MainComponent to hook this deck to playlist
    void setPlaylist(PlaylistComponent* p) { playlist = p; };


private:
    juce::TextButton playButton{ "PLAY" };
    juce::TextButton stopButton{ "STOP" };
    juce::TextButton loadButton{ "LOAD" };

    juce::Slider volSlider;
    juce::Slider speedSlider;
    juce::Slider posSlider;

    DJAudioPlayer* player;

    WaveformDisplay waveformDisplay;

    juce::FileChooser fChooser{ "Select a file..." };

    PlaylistComponent* playlist{ nullptr };


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DeckGUI)
};
