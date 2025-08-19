#include "MainComponent.h"

//==============================================================================
MainComponent::MainComponent()
{
    // set custom theme
    juce::LookAndFeel::setDefaultLookAndFeel(&custLnF);
    // set slider images
    custLnF.setSliderThumbImagesFromBasename("thumb");
    custLnF.setSliderThumbPixels(28);


    setSize(1280, 800);



    // Some platforms require permissions to open input channels so request that here
    if (juce::RuntimePermissions::isRequired(juce::RuntimePermissions::recordAudio)
        && !juce::RuntimePermissions::isGranted(juce::RuntimePermissions::recordAudio))
    {
        juce::RuntimePermissions::request(juce::RuntimePermissions::recordAudio,
            [&](bool granted) { setAudioChannels(granted ? 2 : 0, 2); });
    }
    else
    {
        // Specify the number of input and output channels that we want to open
        setAudioChannels(2, 2);
    }

    addAndMakeVisible(deckGUI1);
    addAndMakeVisible(deckGUI2);

    addAndMakeVisible(playlistComponent1);
    addAndMakeVisible(playlistComponent2);

    // set each components playlist
    deckGUI1.setPlaylist(&playlistComponent1);
    deckGUI2.setPlaylist(&playlistComponent2);


    formatManager.registerBasicFormats();

    // load user library
    playlistComponent1.loadLibrary();
    playlistComponent2.loadLibrary();

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

    // decks (2/3)
    auto decksArea = r.removeFromTop((r.getHeight() * 2) / 3);
    deckGUI1.setBounds(decksArea.removeFromLeft(decksArea.getWidth() / 2));
    deckGUI2.setBounds(decksArea);

    // playlists (1/3)
    playlistComponent1.setBounds(r.removeFromLeft(r.getWidth() / 2));
    playlistComponent2.setBounds(r);
}





