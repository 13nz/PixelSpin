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
                  waveformDisplay(formatManagerToUse, cacheToUse),
                  vinyl(_player, 33.333)
{

    //addAndMakeVisible(buttonRow);

    // hide text for pixel art buttons
    playButton.setButtonText({});
    stopButton.setButtonText({});
    loadButton.setButtonText({});
    clearButton.setButtonText({});

    bool okPlay = playButton.setImagesFromBaseName("play");   
    bool okStop = stopButton.setImagesFromBaseName("stop");
    bool okLoad = loadButton.setImagesFromBaseName("load");
    bool okClear = clearButton.setImagesFromBaseName("clear");

    addAndMakeVisible(playButton);
    addAndMakeVisible(stopButton);
    addAndMakeVisible(loadButton);
    addAndMakeVisible(clearButton);

  

    addAndMakeVisible(volSlider);
    addAndMakeVisible(speedSlider);
    addAndMakeVisible(posSlider);

    // Waveform component
    addAndMakeVisible(waveformDisplay);


    playButton.addListener(this);
    stopButton.addListener(this);
    loadButton.addListener(this);
    clearButton.addListener(this);

    volSlider.addListener(this);
    speedSlider.addListener(this);
    posSlider.addListener(this);

    // hide slider text boxes
    volSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    speedSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    posSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);

    volSlider.setRange(0.0, 1.0);
    speedSlider.setRange(0.0, 10.0);
    posSlider.setRange(0.0, 1.0);

    volSlider.setValue(0.5);

    // vinyl UI
    addAndMakeVisible(vinyl);
    addAndMakeVisible(vinylSelect);
    vinylSelect.setTextWhenNothingSelected("Vinyl");
    vinylSelect.addListener(this);
    scanVinylAssets();

    // listen for player load events
    if (player != nullptr)
    {
        player->addChangeListener(this);
    }

    // effect knobs
    addAndMakeVisible(reverbKnob);
    addAndMakeVisible(chorusKnob);
    addAndMakeVisible(compressionKnob);
    addAndMakeVisible(lofiKnob);

    // slider labels
    // volume 
    volLabel.setText("Vol", juce::dontSendNotification);
    volLabel.setJustificationType(juce::Justification::centredRight);
    addAndMakeVisible(volLabel);

    // speed 
    speedLabel.setText("Speed", juce::dontSendNotification);
    speedLabel.setJustificationType(juce::Justification::centredRight);
    addAndMakeVisible(speedLabel);

    // position 
    posLabel.setText("Pos", juce::dontSendNotification);
    posLabel.setJustificationType(juce::Justification::centredRight);
    addAndMakeVisible(posLabel);

    // knob labels
    // Reverb
    reverbLabel.setText("Reverb", juce::dontSendNotification);
    reverbLabel.setJustificationType(juce::Justification::centred);
    reverbLabel.setInterceptsMouseClicks(false, false);
    addAndMakeVisible(reverbLabel);

    // Chorus
    chorusLabel.setText("Chorus", juce::dontSendNotification);
    chorusLabel.setJustificationType(juce::Justification::centred);
    chorusLabel.setInterceptsMouseClicks(false, false);
    addAndMakeVisible(chorusLabel);

    // Compression
    compressionLabel.setText("Compression", juce::dontSendNotification);
    compressionLabel.setJustificationType(juce::Justification::centred);
    compressionLabel.setInterceptsMouseClicks(false, false);
    addAndMakeVisible(compressionLabel);

    // Lo‑Fi
    lofiLabel.setText("LoFi", juce::dontSendNotification);
    lofiLabel.setJustificationType(juce::Justification::centred);
    lofiLabel.setInterceptsMouseClicks(false, false);
    addAndMakeVisible(lofiLabel);

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
    using juce::roundToInt;
    auto r = getLocalBounds().reduced(8);

    // 1) Waveform at the top
    const int waveH = juce::jmax(48, roundToInt(getHeight() * 0.14f));
    waveformDisplay.setBounds(r.removeFromTop(waveH));

    // 2) Vinyl dropdown
    const int dropH = 24;
    vinylSelect.setBounds(r.removeFromTop(dropH).reduced(2));

    // 3) Main vinyl area
    auto vinylArea = r.removeFromTop(roundToInt(getHeight() * 0.55f)).reduced(6);

    // Reserve side strips for knobs
    const int knobSz = 80;        // knob size
    const int knobGap = 12;
    const int knobColW = knobSz + 2 * knobGap;

    auto leftCol = vinylArea.removeFromLeft(knobColW);
    auto rightCol = vinylArea.removeFromRight(knobColW);

    // Place vinyl in the remaining center
    const int side = std::min(vinylArea.getWidth(), vinylArea.getHeight());
    vinyl.setBounds(juce::Rectangle<int>(0, 0, side, side).withCentre(vinylArea.getCentre()));

    // knobs vertically next to vinyl
    auto kbLeft = juce::Rectangle<int>(0, 0, knobSz, knobSz).withCentre(leftCol.getCentre());
    reverbKnob.setBounds(kbLeft.translated(0, -knobSz - knobGap));
    reverbLabel.setBounds(reverbKnob.getBounds().withY(reverbKnob.getBottom()).withHeight(16));
    chorusKnob.setBounds(kbLeft.translated(0, knobSz + knobGap));
    chorusLabel.setBounds(chorusKnob.getBounds().withY(chorusKnob.getBottom()).withHeight(16));

    auto kbRight = juce::Rectangle<int>(0, 0, knobSz, knobSz).withCentre(rightCol.getCentre());
    compressionKnob.setBounds(kbRight.translated(0, -knobSz - knobGap));
    compressionLabel.setBounds(compressionKnob.getBounds().withY(compressionKnob.getBottom()).withHeight(16));
    lofiKnob.setBounds(kbRight.translated(0, knobSz + knobGap));
    lofiLabel.setBounds(lofiKnob.getBounds().withY(lofiKnob.getBottom()).withHeight(16));

    // sliders row
    // 4) Sliders row
    const int labelW = 50;    // width reserved for labels
    const int sliderH = 16;
    const int sliderGap = 6;

    auto slidersArea = r.removeFromTop(sliderH * 3 + sliderGap * 2).reduced(2);

    {
        auto row = slidersArea.removeFromTop(sliderH);
        volLabel.setBounds(row.removeFromLeft(labelW));
        volSlider.setBounds(row);
    }
    slidersArea.removeFromTop(sliderGap);

    {
        auto row = slidersArea.removeFromTop(sliderH);
        speedLabel.setBounds(row.removeFromLeft(labelW));
        speedSlider.setBounds(row);
    }
    slidersArea.removeFromTop(sliderGap);

    {
        auto row = slidersArea.removeFromTop(sliderH);
        posLabel.setBounds(row.removeFromLeft(labelW));
        posSlider.setBounds(row);
    }


    // gap below sliders
    r.removeFromTop(12); 

    // buttons row 
    const int btnRowH = 32;
    auto btnRow = r.removeFromTop(btnRowH);
    const int btnSz = btnRow.getHeight();

    playButton.setBounds(btnRow.removeFromLeft(btnSz));
    btnRow.removeFromLeft(knobGap);
    stopButton.setBounds(btnRow.removeFromLeft(btnSz));
    btnRow.removeFromLeft(knobGap);
    loadButton.setBounds(btnRow.removeFromLeft(btnSz));
    btnRow.removeFromLeft(knobGap);
    clearButton.setBounds(btnRow.removeFromLeft(btnSz));
}




void DeckGUI::buttonClicked(juce::Button* button)
{
    if (button == &playButton) { player->start(); }
    if (button == &stopButton) {  player->stop(); }
    if (button == &clearButton)
    {
        if (playlist) playlist->clearAll(); 
    }
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

// vinyl helpers
void DeckGUI::scanVinylAssets()
{
    vinylNames.clear();
    vinylFiles.clear();

    auto folder = getVinylsFolder();
    if (!folder.isDirectory()) { vinylSelect.clear(); return; }

    juce::Array<juce::File> all;
    folder.findChildFiles(all, juce::File::findFiles, false);
    all.sort();

    for (auto& f : all)
        if (f.getFileExtension().equalsIgnoreCase(".png"))
        {
            vinylFiles.add(f);
            vinylNames.add(f.getFileNameWithoutExtension());
        }

    vinylSelect.clear(juce::dontSendNotification);
    for (int i = 0; i < vinylNames.size(); ++i)
        vinylSelect.addItem(vinylNames[i], i + 1);

    if (!vinylFiles.isEmpty())
    {
        vinylSelect.setSelectedId(1, juce::dontSendNotification);
        setVinylFromIndex(0);
    }
}

juce::File DeckGUI::getVinylsFolder()
{
    // walk up from the executable looking for Assets/Vinyls (works in Debug/Release and when running from IDE)
    juce::File dir = juce::File::getSpecialLocation(juce::File::currentExecutableFile).getParentDirectory();
    for (int i = 0; i < 10; ++i)
    {
        auto candidate = dir.getChildFile("Assets").getChildFile("Vinyls");
        if (candidate.isDirectory())
            return candidate;
        dir = dir.getParentDirectory();
    }
    // fallback: project root
    dir = juce::File::getCurrentWorkingDirectory().getChildFile("Assets").getChildFile("Vinyls");
    if (dir.isDirectory()) return dir;
    return {};
}


juce::Image DeckGUI::tryLoadImage(const juce::File& f)
{
    if (!f.existsAsFile()) return {};
    if (auto in = f.createInputStream())
        return juce::ImageFileFormat::loadFrom(*in);
    return {};
}


void DeckGUI::setVinylFromIndex(int idx)
{
    if (idx < 0 || idx >= vinylFiles.size()) return;
    auto img = tryLoadImage(vinylFiles[idx]);
    if (img.isValid()) vinyl.setImage(img);
}


void DeckGUI::comboBoxChanged(juce::ComboBox* box)
{
    if (box == &vinylSelect)
    {
        const int idx = vinylSelect.getSelectedId() - 1;
        setVinylFromIndex(idx);
    }
}