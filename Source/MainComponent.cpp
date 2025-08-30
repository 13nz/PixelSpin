#include "MainComponent.h"

MainComponent::MainComponent()
{
    // set custom theme
    juce::LookAndFeel::setDefaultLookAndFeel(&custLnF);
    // set slider images
    custLnF.setSliderThumbImagesFromBasename("thumb");
    custLnF.setSliderThumbPixels(28);


    setSize(1600, 900);



    // permissions to open input channels request
    if (juce::RuntimePermissions::isRequired(juce::RuntimePermissions::recordAudio)
        && !juce::RuntimePermissions::isGranted(juce::RuntimePermissions::recordAudio))
    {
        juce::RuntimePermissions::request(juce::RuntimePermissions::recordAudio,
            [&](bool granted) { setAudioChannels(granted ? 2 : 0, 2); });
    }
    else
    {
        // number of input and output channels  to open
        setAudioChannels(2, 2);
    }

    addAndMakeVisible(deckGUI1);
    addAndMakeVisible(deckGUI2);

    addAndMakeVisible(playlistComponent1);
    addAndMakeVisible(playlistComponent2);

    // add mixing strip
    addAndMakeVisible(mixerStrip);

    // set each components playlist
    deckGUI1.setPlaylist(&playlistComponent1);
    deckGUI2.setPlaylist(&playlistComponent2);

    // connect to DeckGUI pads
    deckGUI1.onPadTriggered = [this](const juce::String& id) { triggerPad(id); };
    deckGUI2.onPadTriggered = [this](const juce::String& id) { triggerPad(id); };

    // crossfader
    mixerStrip.onCrossfadeChanged = [this](float x) { applyCrossfade(x); };
    mixerStrip.onSnapToDeck = [this](bool toB)
        {
            applyCrossfade(toB ? 1.0f : 0.0f);
        };

    // bars visualization
    addAndMakeVisible(playlistGapViz);


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
    // shuts down the audio device and clears the audio source.
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

    // add samples input source
    mixerSource.addInputSource(&sampleBank, false);
    sampleBank.prepareToPlay(samplesPerBlockExpected, sampleRate);

    // freq bars
    playlistGapViz.prepare(sampleRate);
}

void MainComponent::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) 
{
    mixerSource.getNextAudioBlock(bufferToFill);

    // freq bars]mixerSource.getNextAudioBlock(bufferToFill);
    playlistGapViz.pushAudioBlock(*bufferToFill.buffer, bufferToFill.startSample, bufferToFill.numSamples);
}


void MainComponent::releaseResources()
{
    player1.releaseResources();
    player2.releaseResources();

    // release samples resources
    sampleBank.releaseResources();

    mixerSource.releaseResources();
}

void MainComponent::paint (juce::Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
}


void MainComponent::resized()
{
    auto r = getLocalBounds();

    // decks & mixer
    auto decksArea = r.removeFromTop((r.getHeight() * 2) / 3);

    // area for mixer strip
    const int mixW = 140;
    auto left = decksArea.removeFromLeft((decksArea.getWidth() - mixW) / 2);
    auto center = decksArea.removeFromLeft(mixW);
    auto right = decksArea;

    deckGUI1.setBounds(left);
    mixerStrip.setBounds(center.reduced(8));
    deckGUI2.setBounds(right);

    // playlists
    auto playlistsArea = r;

    auto leftPl = playlistsArea.removeFromLeft((playlistsArea.getWidth() - mixW) / 2);
    auto gap = playlistsArea.removeFromLeft(mixW); // empty gap same as mixer width
    auto rightPl = playlistsArea;

    playlistComponent1.setBounds(leftPl);
    playlistComponent2.setBounds(rightPl);

    // bars
    playlistGapViz.setBounds(gap.reduced(6));
}

void MainComponent::applyCrossfade(float x)
{
    x = juce::jlimit(0.0f, 1.0f, x);

    // hard endpoints to guarantee isolation on snaps
    if (x <= 0.0f + 1e-4f)
    {
        player1.setGain(1.0f);
        player2.setGain(0.0f);
        return;
    }
    if (x >= 1.0f - 1e-4f)
    {
        player1.setGain(0.0f);
        player2.setGain(1.0f);
        return;
    }

    // equal power crossfade
    const float theta = x * juce::MathConstants<float>::halfPi;
    const float gA = std::cos(theta) * std::cos(theta);
    const float gB = std::sin(theta) * std::sin(theta);

    player1.setGain(gA);
    player2.setGain(gB);
}





