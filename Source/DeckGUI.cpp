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

// UI for each deck
// waveform, vinyls, knobs, buttons, pads
// connects to DJAudioPlayer
DeckGUI::DeckGUI(DJAudioPlayer* _player,
                juce::AudioFormatManager& formatManagerToUse,
                juce::AudioThumbnailCache& cacheToUse) 
                : player(_player),
                  waveformDisplay(formatManagerToUse, cacheToUse),
                  vinyl(_player, 33.333)
{


    // hide text for pixel art buttons
    playButton.setButtonText({});
    stopButton.setButtonText({});
    loadButton.setButtonText({});
    clearButton.setButtonText({});

    saveButton.setButtonText({});

    bool okPlay = playButton.setImagesFromBaseName("play");   
    bool okStop = stopButton.setImagesFromBaseName("stop");
    bool okLoad = loadButton.setImagesFromBaseName("load");
    bool okClear = clearButton.setImagesFromBaseName("clear");

    addAndMakeVisible(playButton);
    addAndMakeVisible(stopButton);
    addAndMakeVisible(loadButton);
    addAndMakeVisible(clearButton);


    // vertical sliders
    volSlider.setSliderStyle(juce::Slider::LinearBarVertical);
    volSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);

    speedSlider.setSliderStyle(juce::Slider::LinearBarVertical);
    speedSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);


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
    speedSlider.setRange(0.0, 2.0);
    posSlider.setRange(0.0, 1.0);

    // default values
    volSlider.setValue(0.5);
    speedSlider.setValue(1.0);

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
    addAndMakeVisible(delayKnob);

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

    // Delay
    delayLabel.setText("Delay", juce::dontSendNotification);
    delayLabel.setJustificationType(juce::Justification::centred);
    delayLabel.setInterceptsMouseClicks(false, false);
    addAndMakeVisible(delayLabel);

    // knob functionality
    // reverb
    reverbKnob.onValueChange = [this](int step)
        {
            if (player != nullptr)
                player->setReverbAmount(step / 6.0f); // 7 positions for knobs
        };

    // chorus
    chorusKnob.onValueChange = [this](int step)
        {
            if (player != nullptr)
                player->setChorusAmount(step / 6.0f); // 0..6 → 0..1
        };

    // compression
    compressionKnob.onValueChange = [this](int step)
        {
            if (player != nullptr)
                player->setCompressionAmount(step / 6.0f); // 0..6 → 0..1
        };

    // delay
    delayKnob.onValueChange = [this](int step)
        {
            if (player != nullptr)
                player->setDelayAmount(step / 6.0f); // 0..6 → 0..1
        };


    // pads
    for (PixelPad* p : { &scratchPad, &vinylGlitchPad, &kickPad, &snarePad, &drumPad })
    {
        p->setButtonText({}); 
        p->setImagesFromBaseName("pad");
        addAndMakeVisible(*p);
        p->addListener(this);
    }

    // pad labels
    scratchLabel.setText("Scratch", juce::dontSendNotification);
    scratchLabel.setJustificationType(juce::Justification::centredRight);
    addAndMakeVisible(scratchLabel);

    vinylGlitchLabel.setText("Vinyl Glitch", juce::dontSendNotification);
    vinylGlitchLabel.setJustificationType(juce::Justification::centredRight);
    addAndMakeVisible(vinylGlitchLabel);

    kickLabel.setText("Kick", juce::dontSendNotification);
    kickLabel.setJustificationType(juce::Justification::centredRight);
    addAndMakeVisible(kickLabel);

    snareLabel.setText("Snare", juce::dontSendNotification);
    snareLabel.setJustificationType(juce::Justification::centredRight);
    addAndMakeVisible(snareLabel);

    drumLabel.setText("Drum", juce::dontSendNotification);
    drumLabel.setJustificationType(juce::Justification::centredRight);
    addAndMakeVisible(drumLabel);



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

    // Buttons
    const int knobGap = 12;
    const int btnRowH = 36;                      
    auto btnRow = r.removeFromBottom(btnRowH);
    const int btnSz = btnRowH;

    // Waveform on top
    const int waveH = juce::jmax(48, juce::roundToInt(getHeight() * 0.14f));
    waveformDisplay.setBounds(r.removeFromTop(waveH));

    // position slider
    const int labelW = 50;
    const int posRowH = 20;
    {
        auto row = r.removeFromTop(posRowH);
        posLabel.setBounds(row.removeFromLeft(labelW));
        posSlider.setBounds(row);
    }

    // vinyl dropdown
    const int dropH = 24;
    vinylSelect.setBounds(r.removeFromTop(dropH).reduced(2));

    // vinyl area with knobs
    auto vinylArea = r.removeFromTop(juce::roundToInt(getHeight() * 0.55f)).reduced(6);

    const int knobSz = 80;
    const int knobColW = knobSz + 2 * knobGap;

    auto leftCol = vinylArea.removeFromLeft(knobColW);
    auto rightCol = vinylArea.removeFromRight(knobColW);

    // Vinyl centered
    const int side = std::min(vinylArea.getWidth(), vinylArea.getHeight());
    vinyl.setBounds(juce::Rectangle<int>(0, 0, side, side).withCentre(vinylArea.getCentre()));

    // Knobs & labels (left)
    auto kbLeft = juce::Rectangle<int>(0, 0, knobSz, knobSz).withCentre(leftCol.getCentre());
    reverbKnob.setBounds(kbLeft.translated(0, -knobSz - knobGap));
    reverbLabel.setBounds(reverbKnob.getBounds().withY(reverbKnob.getBottom()).withHeight(16));
    chorusKnob.setBounds(kbLeft.translated(0, knobSz + knobGap));
    chorusLabel.setBounds(chorusKnob.getBounds().withY(chorusKnob.getBottom()).withHeight(16));

    // Knobs & labels (right)
    auto kbRight = juce::Rectangle<int>(0, 0, knobSz, knobSz).withCentre(rightCol.getCentre());
    compressionKnob.setBounds(kbRight.translated(0, -knobSz - knobGap));
    compressionLabel.setBounds(compressionKnob.getBounds().withY(compressionKnob.getBottom()).withHeight(16));
    delayKnob.setBounds(kbRight.translated(0, knobSz + knobGap));
    delayLabel.setBounds(delayKnob.getBounds().withY(delayKnob.getBottom()).withHeight(16));

    //vertical sliders for vol & speed
    const int padLabelH = 16;                 
    const int sliderBandH = juce::jmax(120, juce::roundToInt(getHeight() * 0.20f));
    auto sliderBand = r.removeFromTop(sliderBandH).reduced(2);

    // Left/right vertical strips for volume/speed
    const int vStripW = 36;

    // Left strip: VOL
    {
        auto strip = sliderBand.removeFromLeft(vStripW);
        volLabel.setBounds(strip.removeFromTop(16));
        volSlider.setBounds(strip); // fills remaining height
    }

    // Right strip: SPEED
    {
        auto strip = sliderBand.removeFromRight(vStripW);
        speedLabel.setBounds(strip.removeFromTop(16));
        speedSlider.setBounds(strip);
    }

    // center area for pads
    {
        auto center = sliderBand.reduced(4);

        const int maxPad = center.getHeight() - padLabelH - 4;       
        const int padSz = juce::jlimit(64, 128, maxPad); 

        const int gap = 6; 
        const int nPads = 5;
        const int totalW = nPads * padSz + (nPads - 1) * gap;

        // block for pad and label below
        const int blockH = padSz + padLabelH;
        auto block = juce::Rectangle<int>(0, 0, totalW, blockH).withCentre(center.getCentre());

        // split into pad row and label row
        auto padsRow = block.removeFromTop(padSz);
        auto labelsRow = block; 

        // pads
        auto p1 = padsRow.removeFromLeft(padSz);
        padsRow.removeFromLeft(gap);
        auto p2 = padsRow.removeFromLeft(padSz);
        padsRow.removeFromLeft(gap);
        auto p3 = padsRow.removeFromLeft(padSz);
        padsRow.removeFromLeft(gap);
        auto p4 = padsRow.removeFromLeft(padSz);
        padsRow.removeFromLeft(gap);
        auto p5 = padsRow.removeFromLeft(padSz);

        scratchPad.setBounds(p1);
        vinylGlitchPad.setBounds(p2);
        snarePad.setBounds(p3);
        kickPad.setBounds(p4);
        drumPad.setBounds(p5);

        // labels aligned with pads
        scratchLabel.setBounds(p1.getX(), labelsRow.getY(), p1.getWidth(), padLabelH);
        scratchLabel.setJustificationType(juce::Justification::centred);

        vinylGlitchLabel.setBounds(p2.getX(), labelsRow.getY(), p2.getWidth(), padLabelH);
        vinylGlitchLabel.setJustificationType(juce::Justification::centred);

        snareLabel.setBounds(p3.getX(), labelsRow.getY(), p3.getWidth(), padLabelH);
        snareLabel.setJustificationType(juce::Justification::centred);

        kickLabel.setBounds(p4.getX(), labelsRow.getY(), p4.getWidth(), padLabelH);
        kickLabel.setJustificationType(juce::Justification::centred);

        drumLabel.setBounds(p5.getX(), labelsRow.getY(), p5.getWidth(), padLabelH);
        drumLabel.setJustificationType(juce::Justification::centred);


    }

    // gap
    r.removeFromTop(8);

    // buttons
    playButton.setBounds(btnRow.removeFromLeft(btnSz));
    btnRow.removeFromLeft(knobGap);
    stopButton.setBounds(btnRow.removeFromLeft(btnSz));
    btnRow.removeFromLeft(knobGap);
    loadButton.setBounds(btnRow.removeFromLeft(btnSz));
    btnRow.removeFromLeft(knobGap);
    clearButton.setBounds(btnRow.removeFromLeft(btnSz));
}


// buttons event handling
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
    // pads
    else if (button == &scratchPad) { if (onPadTriggered) onPadTriggered("scratch"); }
    else if (button == &vinylGlitchPad) { if (onPadTriggered) onPadTriggered("glitch"); }
    else if (button == &snarePad) { if (onPadTriggered) onPadTriggered("snare"); }
    else if (button == &kickPad) { if (onPadTriggered) onPadTriggered("kick"); }
    else if (button == &drumPad) { if (onPadTriggered) onPadTriggered("drum"); }
}

// slider handler
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


// file handling
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
    // clear existing
    vinylNames.clear();
    vinylFiles.clear();

    // find folder
    auto folder = getVinylsFolder();
    if (!folder.isDirectory()) { vinylSelect.clear(); return; }

    juce::Array<juce::File> all;
    folder.findChildFiles(all, juce::File::findFiles, false);
    all.sort();

    // loop & add each file
    for (auto& f : all)
    {
        if (f.getFileExtension().equalsIgnoreCase(".png"))
        {
            vinylFiles.add(f);
            vinylNames.add(f.getFileNameWithoutExtension());
        }
    }

    vinylSelect.clear(juce::dontSendNotification);
    for (int i = 0; i < vinylNames.size(); ++i)
        vinylSelect.addItem(vinylNames[i], i + 1);

    // default file
    if (!vinylFiles.isEmpty())
    {
        vinylSelect.setSelectedId(1, juce::dontSendNotification);
        setVinylFromIndex(0);
    }
}

// finds folder
juce::File DeckGUI::getVinylsFolder()
{
    // walk up from the executable looking for Assets/Vinyls
    juce::File dir = juce::File::getSpecialLocation(juce::File::currentExecutableFile).getParentDirectory();
    for (int i = 0; i < 10; ++i)
    {
        auto candidate = dir.getChildFile("Assets").getChildFile("Vinyls");
        if (candidate.isDirectory())
            return candidate;
        dir = dir.getParentDirectory();
    }
    // fallback to project root
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