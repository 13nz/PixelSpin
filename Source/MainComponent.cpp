#include "MainComponent.h"

//==============================================================================
MainComponent::MainComponent()
{
    // set custom theme
    juce::LookAndFeel::setDefaultLookAndFeel(&custLnF);
    // set slider images
    custLnF.setSliderThumbImagesFromBasename("thumb");
    custLnF.setSliderThumbPixels(28);


    setSize (1280, 800);



    // Some platforms require permissions to open input channels so request that here
    if (juce::RuntimePermissions::isRequired (juce::RuntimePermissions::recordAudio)
        && ! juce::RuntimePermissions::isGranted (juce::RuntimePermissions::recordAudio))
    {
        juce::RuntimePermissions::request (juce::RuntimePermissions::recordAudio,
                                           [&] (bool granted) { setAudioChannels (granted ? 2 : 0, 2); });
    }
    else
    {
        // Specify the number of input and output channels that we want to open
        setAudioChannels (2, 2);
    }

    addAndMakeVisible(deckGUI1);
    addAndMakeVisible(deckGUI2);

    addAndMakeVisible(playlistComponent1);
    addAndMakeVisible(playlistComponent2);

    addAndMakeVisible(vinyl1);
    addAndMakeVisible(vinyl2);

    addAndMakeVisible(vinylSelectLeft);
    addAndMakeVisible(vinylSelectRight);

    vinylSelectLeft.addListener(this);
    vinylSelectRight.addListener(this);

    // default
    vinylSelectLeft.setTextWhenNothingSelected("Vinyl (L)");
    vinylSelectRight.setTextWhenNothingSelected("Vinyl (R)");

    // set each components playlist
    deckGUI1.setPlaylist(&playlistComponent1);
    deckGUI2.setPlaylist(&playlistComponent2);


    formatManager.registerBasicFormats();

    // load user library
    playlistComponent1.loadLibrary();
    playlistComponent2.loadLibrary();

    // scan vinyls
    scanVinylAssets();
}

MainComponent::~MainComponent()
{
    // save library
    playlistComponent1.saveLibrary();
    playlistComponent2.saveLibrary();
    // This shuts down the audio device and clears the audio source.
    shutdownAudio();
    // remove theme
    juce::LookAndFeel::setDefaultLookAndFeel(nullptr);
}

//==============================================================================
void MainComponent::prepareToPlay (int samplesPerBlockExpected, double sampleRate)
{
    player1.prepareToPlay(samplesPerBlockExpected, sampleRate);
    player2.prepareToPlay(samplesPerBlockExpected, sampleRate);

    // init mixer
    mixerSource.prepareToPlay(samplesPerBlockExpected, sampleRate);

    // set up mixer
    mixerSource.addInputSource(&player1, false);
    mixerSource.addInputSource(&player2, false);
}

void MainComponent::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) 
{
    mixerSource.getNextAudioBlock(bufferToFill);
}


void MainComponent::releaseResources()
{
    player1.releaseResources();
    player2.releaseResources();

    mixerSource.releaseResources();
}

//==============================================================================
void MainComponent::paint (juce::Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
}


void MainComponent::resized()
{
    auto r = getLocalBounds();
    const int h = getHeight();

    // --- TOP: vinyl + dropdowns
    auto top = r.removeFromTop(juce::roundToInt(h * 0.22f));
    auto leftTop = top.removeFromLeft(top.getWidth() / 2);
    auto rightTop = top;

    const int dropH = 32;
    vinylSelectLeft.setBounds(leftTop.removeFromTop(dropH).reduced(8, 4));
    vinylSelectRight.setBounds(rightTop.removeFromTop(dropH).reduced(8, 4));

    auto sideL = std::min(leftTop.getWidth(), leftTop.getHeight());
    auto sideR = std::min(rightTop.getWidth(), rightTop.getHeight());
    vinyl1.setBounds(juce::Rectangle<int>(0, 0, sideL, sideL).withCentre(leftTop.getCentre()));
    vinyl2.setBounds(juce::Rectangle<int>(0, 0, sideR, sideR).withCentre(rightTop.getCentre()));

    // --- MIDDLE: decks (upper half of remaining r)
    auto decksArea = r.removeFromTop(r.getHeight() / 2);
    auto leftDeck = decksArea.removeFromLeft(decksArea.getWidth() / 2);
    auto rightDeck = decksArea;
    deckGUI1.setBounds(leftDeck);
    deckGUI2.setBounds(rightDeck);

    // --- BOTTOM: playlists (what's left in r)
    auto leftList = r.removeFromLeft(r.getWidth() / 2);
    auto rightList = r;
    playlistComponent1.setBounds(leftList);
    playlistComponent2.setBounds(rightList);
}



// vinyl spinner helpers
void MainComponent::scanVinylAssets()
{
    vinylOptions.clear();
    vinylFiles.clear();

    const auto vinylsFolder = getVinylsFolder();
    DBG("Vinyls folder resolved to: " + vinylsFolder.getFullPathName());

    if (!vinylsFolder.isDirectory())
    {
        DBG("Vinyls folder NOT found. Expected OtoDecks/Assets/Vinyls somewhere above the exe.");
        vinylSelectLeft.clear();
        vinylSelectRight.clear();
        return;
    }

    juce::Array<juce::File> files;
    vinylsFolder.findChildFiles(files, juce::File::findFiles, false); // get all; we’ll filter

    // Filter by extension case-insensitively and sort
    juce::Array<juce::File> pngs;
    for (auto& f : files)
        if (f.getFileExtension().equalsIgnoreCase(".png"))
            pngs.add(f);

    pngs.sort(); // stable ordering

    if (pngs.isEmpty())
        DBG("No PNGs found in: " + vinylsFolder.getFullPathName());

    for (auto& f : pngs)
    {
        vinylFiles.add(f);
        vinylOptions.add(f.getFileNameWithoutExtension());
    }

    // Populate dropdowns
    vinylSelectLeft.clear(juce::dontSendNotification);
    vinylSelectRight.clear(juce::dontSendNotification);

    for (int i = 0; i < vinylOptions.size(); ++i)
    {
        const int itemId = i + 1; // JUCE requires itemId >= 1
        vinylSelectLeft.addItem(vinylOptions[i], itemId);
        vinylSelectRight.addItem(vinylOptions[i], itemId);
    }

    // Select first and apply to spinners
    if (!vinylFiles.isEmpty())
    {
        vinylSelectLeft.setSelectedId(1, juce::dontSendNotification);
        vinylSelectRight.setSelectedId(1, juce::dontSendNotification);
        setSpinnerImageFromIndex(true, 0);
        setSpinnerImageFromIndex(false, 0);
    }
}


juce::Image MainComponent::tryLoadImage(const juce::File& f)
{
    if (!f.existsAsFile()) return {};

    auto in = f.createInputStream();

    if (in == nullptr) return {};

    return juce::ImageFileFormat::loadFrom(*in);
}

void MainComponent::setSpinnerImageFromIndex(bool left, int index)
{
    if (index < 0 || index >= vinylFiles.size()) return;

    auto img = tryLoadImage(vinylFiles[index]);

    if (!img.isValid()) return;

    if (left) vinyl1.setImage(img);
    else vinyl2.setImage(img);
}

void MainComponent::comboBoxChanged(juce::ComboBox* box)
{
    if (box == &vinylSelectLeft)
    {
        int index = vinylSelectLeft.getSelectedId() - 1;
        setSpinnerImageFromIndex(true, index);
    }
    else if (box == &vinylSelectRight)
    {
        int index = vinylSelectRight.getSelectedId() - 1;
        setSpinnerImageFromIndex(false, index);
    }
}

juce::File MainComponent::getVinylsFolder()
{
    // Start at the executable directory 
    juce::File dir = juce::File::getSpecialLocation(juce::File::currentExecutableFile)
        .getParentDirectory();

    // walk up to 10 levels and look for Assets/Vinyls
    for (int i = 0; i < 10; ++i)
    {
        auto candidate = dir.getChildFile("Assets").getChildFile("Vinyls");
        if (candidate.isDirectory())
            return candidate;
        dir = dir.getParentDirectory();
    }

    // fallback if launching from project root
    dir = juce::File::getCurrentWorkingDirectory()
        .getChildFile("Assets").getChildFile("Vinyls");
    if (dir.isDirectory())
        return dir;

    return {}; // not found

}

