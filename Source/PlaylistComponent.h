#pragma once

#include <JuceHeader.h>
#include <vector>
#include "PixelButton.h"

// avoid circular include
class DJAudioPlayer;
class DeckGUI;

class PlaylistComponent  : public juce::Component, 
                           public juce::TableListBoxModel,
                           public juce::Button::Listener
{
public:
    PlaylistComponent(DJAudioPlayer& targetPlayer, DeckGUI& targetDeckGUI, juce::AudioFormatManager& fmt, juce::String playlistId);
    ~PlaylistComponent() override;

    void paint (juce::Graphics&) override;
    void resized() override;

    // juce::TableListBoxModel
    int getNumRows() override;
    void paintRowBackground(
        juce::Graphics&,
        int rowNumber,
        int width,
        int height,
        bool rowIsSelected
    ) override;

    void paintCell(
        juce::Graphics&,
        int rowNumber,
        int columnId,
        int width,
        int height,
        bool rowIsSelected
    ) override;

    juce::Component* refreshComponentForCell(
        int rowNumber,
        int columnId,
        bool isRowSelected,
        juce::Component *existingComponentToUpdate
    );

    void buttonClicked(juce::Button* button) override;

    // allow external components to add files
    void addFiles(const juce::Array<juce::File>& files);
    // double click to play songs
    void cellDoubleClicked(int rowNumber, int columnId, const juce::MouseEvent&) override;

    // save and load library
    void saveLibrary() const;
    void loadLibrary();

    // clear playlist
    void clearAll();


private:
    juce::TableListBox tableComponent;

    // track object definistion
    struct Track { 
        juce::File file; 
        juce::String title; 
        double lengthSeconds{ 0.0 }; 
    };

    // stores loaded tracks
    std::vector<Track> tracks;

    void addTrackFromFile(const juce::File& file);
    void playRow(int row);

    juce::String formatSeconds(double totalSeconds) const;
    PixelButton clearButton;

    DJAudioPlayer& player;
    DeckGUI& deckGUI; // for waveform only
    juce::AudioFormatManager& formatManager;

    juce::String playlistId;

    // library helper
    juce::File getLibraryFile() const;


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PlaylistComponent)
};
