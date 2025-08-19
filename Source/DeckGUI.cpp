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

    // waveform
    auto waveformH = juce::jmax(40, juce::roundToInt(getHeight() * 0.15f));
    waveformDisplay.setBounds(r.removeFromTop(waveformH));

    // buttons & sliders
    const int btnRowH = 32;   
    const int sliderH = 26;   
    const int sliderGap = 6;

    auto btnRow = r.removeFromBottom(btnRowH).reduced(2);
    auto sliders = r.removeFromBottom(sliderH * 3 + sliderGap * 2).reduced(2);

    // sliders stacked
    auto one = juce::Rectangle<int>(sliders.getX(), sliders.getY(), sliders.getWidth(), sliderH);
    volSlider.setBounds(one);                    sliders = sliders.withTrimmedTop(sliderH + sliderGap);
    speedSlider.setBounds(one.withY(sliders.getY())); sliders = sliders.withTrimmedTop(sliderH + sliderGap);
    posSlider.setBounds(one.withY(sliders.getY()));

    // 3 vinyl in middle
    auto vinylArea = r.reduced(6);
    const int side = std::min(vinylArea.getWidth(), vinylArea.getHeight());
    vinyl.setBounds(juce::Rectangle<int>(0, 0, side, side).withCentre(vinylArea.getCentre()));

    // dropdown above vinyl spinner
    auto dropH = 28;
    auto drop = vinylArea.removeFromTop(dropH);
    vinylSelect.setBounds(drop.reduced(2));

    // button row
    const int gap = 6;
    const int btnSz = btnRow.getHeight();  // square buttons, crisp scaling
    auto row = btnRow;

    playButton.setBounds(row.removeFromLeft(btnSz));
    row.removeFromLeft(gap);
    stopButton.setBounds(row.removeFromLeft(btnSz));
    row.removeFromLeft(gap);
    loadButton.setBounds(row.removeFromLeft(btnSz));
    row.removeFromLeft(gap);
    clearButton.setBounds(row.removeFromLeft(btnSz));
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