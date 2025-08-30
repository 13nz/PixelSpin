/*
  ==============================================================================

    SampleAudioSource.cpp
    Created: 27 Aug 2025 3:58:56pm
    Author:  Lena

  ==============================================================================
*/

#include <JuceHeader.h>
#include "SampleAudioSource.h"
// one-shot sample player that mixes into output buffer

// prepare: store sample rate if needed later
void SampleAudioSource::prepareToPlay(int, double sampleRate)
{
    fs = sampleRate;
}

// clear voices and loaded samples
void SampleAudioSource::releaseResources()
{
    const juce::ScopedLock sl(lock);
    voices.clear();
    samples.clear();
}

// mix each active voice into output and remove finished voices
void SampleAudioSource::getNextAudioBlock(const juce::AudioSourceChannelInfo& info)
{
    auto* out = info.buffer;
    if (!out) return;

    // clear
    out->clear(info.startSample, info.numSamples);

    const juce::ScopedLock sl(lock);
    if (voices.empty()) return;

    // get channels
    const int outCh = out->getNumChannels();

    // mix all active voices
    for (auto it = voices.begin(); it != voices.end(); )
    {
        Voice& v = *it;
        if (v.sample == nullptr || v.sample->buffer.getNumSamples() <= 0)
        {
            it = voices.erase(it); continue;
        }

        const auto& sbuf = v.sample->buffer;
        const int srcCh = sbuf.getNumChannels();
        const int remaining = sbuf.getNumSamples() - v.pos;

        if (remaining <= 0) { it = voices.erase(it); continue; }

        const int toCopy = std::min(remaining, info.numSamples);

        // add data to output channel
        for (int ch = 0; ch < outCh; ++ch)
        {
            const float* src = sbuf.getReadPointer(std::min(ch, srcCh - 1), v.pos);
            float* dst = out->getWritePointer(ch, info.startSample);
            juce::FloatVectorOperations::addWithMultiply(dst, src, v.gain, toCopy);
        }

        v.pos += toCopy;

        // remove when sample finishes
        if (v.pos >= sbuf.getNumSamples()) it = voices.erase(it);
        else ++it;
    }
}

// trigger a one - shot sample by id
void SampleAudioSource::trigger(const juce::String& id, float gain)
{
    if (!ensureSampleLoaded(id)) return;

    const juce::ScopedLock sl(lock);
    auto it = samples.find(id.toStdString());
    if (it == samples.end()) return;

    // cap simultaneous voices to avoid memory problems
    if (voices.size() > 16) voices.erase(voices.begin());

    voices.push_back(Voice{ &it->second, 0, gain });
}

// ensure a sample with this id is present in cache & try assets / samples
bool SampleAudioSource::ensureSampleLoaded(const juce::String& id)
{
    const std::string key = id.toStdString();
    if (samples.find(key) != samples.end()) return true;

    auto root = findSamplesFolder();     // Assets/Samples
    if (!root.isDirectory()) return false;

    // allow few file extensions (most will be wav or mp3)
    static const char* exts[] = { ".wav", ".mp3", ".flac"};
    juce::File f;

    // add files
    for (auto* e : exts)
    {
        auto cand = root.getChildFile(id + e);
        if (cand.existsAsFile()) { f = cand; break; }
    }
    if (!f.existsAsFile()) return false;

    auto reader = openReader(f);
    if (!reader) return false;

    SampleData data;

    data.buffer.setSize((int)reader->numChannels, (int)reader->lengthInSamples);
    reader->read(&data.buffer, 0, (int)reader->lengthInSamples, 0, true, true);

    samples.emplace(key, std::move(data));

    return true;
}

// try to find assets/samples folder near executable or current working directory
juce::File SampleAudioSource::findSamplesFolder()
{
    // walk up from exe: Assets/Samples
    auto dir = juce::File::getSpecialLocation(juce::File::currentExecutableFile).getParentDirectory();
    for (int i = 0; i < 10; ++i)
    {
        auto cand = dir.getChildFile("Assets").getChildFile("Samples");
        if (cand.isDirectory()) return cand;
        dir = dir.getParentDirectory();
    }
    // fallback: current dir
    auto cwd = juce::File::getCurrentWorkingDirectory().getChildFile("Assets").getChildFile("Samples");
    return cwd.isDirectory() ? cwd : juce::File{};
}

std::unique_ptr<juce::AudioFormatReader> SampleAudioSource::openReader(const juce::File& f)
{
    static juce::AudioFormatManager fm; static bool inited = false;
    if (!inited) { fm.registerBasicFormats(); inited = true; }
    return std::unique_ptr<juce::AudioFormatReader>(fm.createReaderFor(f));
}
