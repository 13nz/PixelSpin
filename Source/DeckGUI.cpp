/*
  ==============================================================================

    DeckGUI.cpp
    Created: 30 Apr 2025 3:46:34pm
    Author:  Lena

  ==============================================================================
*/

#include <JuceHeader.h>
#include "DeckGUI.h"
#include "PlaylistComponent.h"
#include "Theme.h"


//==============================================================================
DeckGUI::DeckGUI(DJAudioPlayer* _player,
                juce::AudioFormatManager& formatManagerToUse,
                juce::AudioThumbnailCache& cacheToUse) 
                : player(_player),
                  waveformDisplay(formatManagerToUse, cacheToUse)
{
    // hide text for pixel art buttons
    playButton.setButtonText({});
    stopButton.setButtonText({});
    loadButton.setButtonText({});

    bool okPlay = playButton.setImagesFromBaseName("play");   // looks in Assets/Buttons
    bool okStop = stopButton.setImagesFromBaseName("stop");
    bool okLoad = loadButton.setImagesFromBaseName("load");

    addAndMakeVisible(playButton);
    addAndMakeVisible(stopButton);
    addAndMakeVisible(loadButton);

    addAndMakeVisible(volSlider);
    addAndMakeVisible(speedSlider);
    addAndMakeVisible(posSlider);

    // Waveform component
    addAndMakeVisible(waveformDisplay);

    playButton.addListener(this);
    stopButton.addListener(this);
    loadButton.addListener(this);

    volSlider.addListener(this);
    speedSlider.addListener(this);
    posSlider.addListener(this);

    volSlider.setRange(0.0, 1.0);
    speedSlider.setRange(0.0, 10.0);
    posSlider.setRange(0.0, 1.0);

    volSlider.setValue(0.5);

    // listen for player load events
    if (player != nullptr)
    {
        player->addChangeListener(this);
    }

    // 500 milliseconds: half a second
    startTimer(500);

}

DeckGUI::~DeckGUI()
{
    stopTimer();

    if (player != nullptr)
    {
        player->removeChangeListener(this);
    }
}

void DeckGUI::paint (juce::Graphics& g)
{
    g.fillAll(Theme::panelBg);
    g.setColour(Theme::panelOutline());
    g.drawRect(getLocalBounds(), 1);
}

void DeckGUI::resized()
{
    auto r = getLocalBounds().reduced(8);

    // top row for buttons
    auto top = r.removeFromTop(60);
    const int sz = top.getHeight();
    playButton.setBounds(top.removeFromLeft(sz));
    top.removeFromLeft(8);
    stopButton.setBounds(top.removeFromLeft(sz));
    top.removeFromLeft(8);
    loadButton.setBounds(top.removeFromLeft(sz));

    // now lay out sliders & waveform only
    const int rowH = r.getHeight() / 5; // adjust as you like
    volSlider.setBounds(r.removeFromTop(rowH));
    speedSlider.setBounds(r.removeFromTop(rowH));
    posSlider.setBounds(r.removeFromTop(rowH));
    waveformDisplay.setBounds(r); // whatever remains
}


void DeckGUI::buttonClicked(juce::Button* button)
{
    if (button == &playButton) { player->start(); }
    if (button == &stopButton) {  player->stop(); }
    if (button == &loadButton) 
    {
        auto flags = juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles | juce::FileBrowserComponent::canSelectMultipleItems;

        fChooser.launchAsync(flags, [this](const juce::FileChooser& fc)
            {
                auto files = fc.getResults();
                // add to decks playlist
                if (playlist != nullptr)
                {
                    playlist->addFiles(files);

                    // preview first selected
                    if (files.size() > 0)
                    {
                        juce::URL first{ files[0] };
                        player->loadURL(first);
                        waveformDisplay.loadURL(first);
                    }
                }
            });
    }
}

void DeckGUI::sliderValueChanged(juce::Slider* slider)
{
    if (slider == &volSlider) 
    {
        player->setGain(slider->getValue());
    }
    if (slider == &speedSlider) 
    {
        player->setSpeed(slider->getValue());
    }
    if (slider == &posSlider) 
    {
        player->setPositionRelative(slider->getValue());
    }
}

bool DeckGUI::isInterestedInFileDrag(const juce::StringArray& files)
{
    return true;
}

void DeckGUI::filesDropped(const juce::StringArray& files, int x, int y)
{
    for (juce::String filename : files)
    {
        std::cout << "DeckGUI::filesDropped " << filename << std::endl;
        juce::URL fileURL = juce::URL{ juce::File{filename} };
        player->loadURL(fileURL);
        return;
    }
}

void DeckGUI::timerCallback()
{
    waveformDisplay.setPositionRelative(player->getPositionRelative());
}

void DeckGUI::changeListenerCallback(juce::ChangeBroadcaster* source)
{
    if (source == player)
    {
        auto url = player->getCurrentURL();

        if (url.isEmpty())
        {
            return;
        }

        waveformDisplay.loadURL(url);
    }
}

void DeckGUI::showWaveForm(juce::URL url)
{
    waveformDisplay.loadURL(url);
}
