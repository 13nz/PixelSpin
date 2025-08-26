#pragma once

#include <JuceHeader.h>
#include "DJAudioPlayer.h"
#include "WaveformDisplay.h"
#include "PixelButton.h"
#include "VinylSpinner.h"
#include "PixelKnob.h"


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
                 public juce::ChangeListener,
                 public juce::ComboBox::Listener
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

    // let MainComponent wire playlist actions to this deck:
    std::function<void()> onClearRequested;


    // combo change (vinyl)
    void comboBoxChanged(juce::ComboBox* box) override;


private:
    juce::Slider volSlider;
    juce::Slider speedSlider;
    juce::Slider posSlider;

    DJAudioPlayer* player;

    WaveformDisplay waveformDisplay;

    juce::FileChooser fChooser{ "Select a file..." };

    PlaylistComponent* playlist{ nullptr };

    // row component for buttons
    juce::Component buttonRow;

    // pixel buttons
    PixelButton playButton;
    PixelButton stopButton;
    PixelButton loadButton;
    PixelButton clearButton;

    // vinyl per deck
    VinylSpinner vinyl;                
    juce::ComboBox vinylSelect;        
    juce::StringArray vinylNames;
    juce::Array<juce::File> vinylFiles;

    void scanVinylAssets();
    static juce::File getVinylsFolder();
    static juce::Image tryLoadImage(const juce::File& f);
    void setVinylFromIndex(int idx);

    // effects knobs
    PixelKnob reverbKnob{ "knob" };
    PixelKnob chorusKnob{ "knob" };
    PixelKnob compressionKnob{ "knob" };
    PixelKnob delayKnob{ "knob" };

    // slider labels
    juce::Label volLabel, speedLabel, posLabel;

    // knob labels
    juce::Label reverbLabel, chorusLabel, compressionLabel, delayLabel;


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DeckGUI)
};
