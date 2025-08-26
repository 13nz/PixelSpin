/*
  ==============================================================================

    DJAudioPlayer.cpp
    Created: 30 Apr 2025 3:07:25pm
    Author:  Lena

  ==============================================================================
*/

#include "DJAudioPlayer.h"

DJAudioPlayer::DJAudioPlayer(juce::AudioFormatManager& _formatManager) : formatManager(_formatManager)
{

}

DJAudioPlayer::~DJAudioPlayer() 
{

}

void DJAudioPlayer::prepareToPlay(int samplesPerBlockExpected, double sampleRate) 
{ 
    transportSource.prepareToPlay(samplesPerBlockExpected, sampleRate);
    resampleSource.prepareToPlay(samplesPerBlockExpected, sampleRate);

    // effects
    effects.prepare(sampleRate, samplesPerBlockExpected, 2);
}

void DJAudioPlayer::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) 
{
    resampleSource.getNextAudioBlock(bufferToFill);

    // effects
    effects.process(*bufferToFill.buffer);
}

void DJAudioPlayer::releaseResources() {
    transportSource.releaseResources();
    resampleSource.releaseResources();

    // release effects
    effects.reset();
}

void DJAudioPlayer::loadURL(juce::URL audioURL) 
{
    // convert audioURL to input stream and create reader
    auto* reader = formatManager.createReaderFor(audioURL.createInputStream(false));

    //if file reader OK
    if (reader != nullptr) 
    {
        // create audio format reader source
        std::unique_ptr<juce::AudioFormatReaderSource> newSource(new juce::AudioFormatReaderSource(reader, true));

        // pass into transport source
        transportSource.setSource(newSource.get(), 0, nullptr, reader->sampleRate);
        readerSource.reset(newSource.release());

        currentURL = audioURL;

        // notify listeners of new URL
        sendChangeMessage();
    }
}

void DJAudioPlayer::setGain(double gain) 
{
    if (gain >= 0 && gain <= 1.0) 
    {
        transportSource.setGain(gain);
    }
}

void DJAudioPlayer::setSpeed(double ratio) 
{
    resampleSource.setResamplingRatio(ratio);
}

void DJAudioPlayer::setPosition(double posInSecs) 
{
    transportSource.setPosition(posInSecs);
}

void DJAudioPlayer::setPositionRelative(double pos) 
{
    if (pos >= 0 && pos <= 1) {
        double posInSecs = transportSource.getLengthInSeconds() * pos;
        setPosition(posInSecs);
    }
}

void DJAudioPlayer::start() 
{
    transportSource.start();
}

void DJAudioPlayer::stop() 
{
    transportSource.stop();
}

double DJAudioPlayer::getPositionRelative()
{
    const double len = transportSource.getLengthInSeconds();
    if (len <= 0.0) return 0.0;

    return transportSource.getCurrentPosition() / len;
}

bool DJAudioPlayer::isPlaying() const
{
    return transportSource.isPlaying();
}
    