/*
  ==============================================================================

    SpectrumBars.cpp
    Created: 27 Aug 2025 5:33:54pm
    Author:  Lena

  ==============================================================================
*/

#include <JuceHeader.h>
#include "SpectrumBars.h"

// https://juce.com/tutorials/tutorial_spectrum_analyser/ <-- documentation used

// spectrum visualizer component using fft to draw vertical bars
// takes audio input, performs fft, maps magnitudes to bars, and repaints at 60fps

SpectrumBars::SpectrumBars(int order, int numBars)
    : fftOrder(order),
    fftSize(1 << order),
    bars(juce::jlimit(8, 64, numBars)), // clamp bars 8-64
    fft(order),
    window(fftSize, juce::dsp::WindowingFunction<float>::hann)
{
    // larger than 1 frame
    monoFifo.setSize(1, fftSize * 2); // buffer for mono audio data

    fftInput.resize(fftSize, 0.0f); // fft input buffer
    fftBuffer.resize((size_t)fftSize * 2, 0.0f);
    mag.resize(fftSize / 2, 0.0f); // magnitude after fft
    barLevels.resize((size_t)bars, 0.0f);

    // repaint timer, refresh at 60fps
    startTimerHz(60);
}

void SpectrumBars::prepare(double sampleRate)
{
    fifoFill.store(0);
    std::fill(barLevels.begin(), barLevels.end(), 0.0f);
}

void SpectrumBars::setNumBars(int n)
{
    bars = juce::jlimit(8, 64, n);
    barLevels.assign((size_t)bars, 0.0f);
    repaint();
}

void SpectrumBars::setDecay(float perSecond)
{
    // how quickly bars fall
    decayPerSec = juce::jlimit(0.1f, 30.0f, perSecond);
}

void SpectrumBars::pushAudioBlock(const juce::AudioBuffer<float>& buffer, int start, int num)
{
    const int chs = buffer.getNumChannels();
    const float* const* chans = buffer.getArrayOfReadPointers();

    // feed audio into fifo as mono
    pushMono(chans, chs, num, start);
    runFFTIfReady();
}

void SpectrumBars::pushMono(const float* const* chans, int numChans, int num, int start)
{
    auto* fifo = monoFifo.getWritePointer(0);
    int fill = fifoFill.load(std::memory_order_relaxed);

    for (int i = 0; i < num; ++i)
    {
        // simple average to mono
        float s = 0.f;
        for (int c = 0; c < numChans; ++c)
        {
            s += chans[c][start + i];
        }

        // average channels
        s *= (numChans > 0 ? (1.0f / numChans) : 1.0f);

        fifo[fill % monoFifo.getNumSamples()] = s;
        ++fill;
    }
    fifoFill.store(fill, std::memory_order_release);
}

void SpectrumBars::runFFTIfReady()
{
    // if at least fftSize samples --> FFT on the latest window
    int fill = fifoFill.load(std::memory_order_acquire);
    // make sure enough samples
    if (fill < fftSize) return;

    const int fifoLen = monoFifo.getNumSamples();
    const int start = (fill - fftSize) % fifoLen;
    const auto* fifo = monoFifo.getReadPointer(0);

    // copy out a contiguous block (handle wrap)
    if (start + fftSize <= fifoLen)
    {
        std::memcpy(fftInput.data(), fifo + start, (size_t)fftSize * sizeof(float));
    }
    else
    {
        const int first = fifoLen - start;
        std::memcpy(fftInput.data(), fifo + start, first * sizeof(float));
        std::memcpy(fftInput.data() + first, fifo, (fftSize - first) * sizeof(float));
    }

    // window and forward FFT
    window.multiplyWithWindowingTable(fftInput.data(), fftSize);

    // copy to buffer
    std::fill(fftBuffer.begin(), fftBuffer.end(), 0.0f);
    for (int i = 0; i < fftSize; ++i) fftBuffer[(size_t)i * 2] = fftInput[(size_t)i];

    fft.performRealOnlyForwardTransform(fftBuffer.data());

    // magnitude 0 - N/2
    for (int i = 0; i < fftSize / 2; ++i)
    {
        const float re = fftBuffer[(size_t)i * 2];
        const float im = fftBuffer[(size_t)i * 2 + 1];
        mag[(size_t)i] = std::sqrt(re * re + im * im);
    }

    // map to bars
    const int nSpec = fftSize / 2;

    for (int b = 0; b < bars; ++b)
    {
        // index range for bar
        const float t0 = (float)b / (float)bars;
        const float t1 = (float)(b + 1) / (float)bars;
        const int i0 = (int)std::floor(std::pow(10.0f, t0) / 10.0f * nSpec);
        const int i1 = (int)std::floor(std::pow(10.0f, t1) / 10.0f * nSpec);
        const int a = juce::jlimit(1, nSpec - 1, i1 - i0);
        float sum = 0.0f;

        for (int i = 0; i < a; ++i)
        {
            sum += mag[(size_t)juce::jlimit(0, nSpec - 1, i0 + i)];
        }

        const float v = sum / (float)a;

        // convert to a normalized 0-1 
        const float nv = juce::jlimit(0.0f, 1.0f, std::log10(1.0f + v * 8.0f));

        // hold/decay
        barLevels[(size_t)b] = juce::jmax(barLevels[(size_t)b], nv);
    }
}

void SpectrumBars::timerCallback()
{
    // apply frame decay
    const float dt = 1.0f / 60.0f;
    const float d = decayPerSec * dt;
    for (auto& v : barLevels)
        v = juce::jmax(0.0f, v - d);

    repaint();
}

void SpectrumBars::paint(juce::Graphics& g)
{
    auto r = getLocalBounds().toFloat();
    g.fillAll(juce::Colours::transparentBlack);

    const float gap = 2.0f;
    const float bw = (r.getWidth() - gap * (bars - 1)) / (float)bars;
    const float h = r.getHeight();

    for (int b = 0; b < bars; ++b)
    {
        const float x = r.getX() + b * (bw + gap);
        const float v = juce::jlimit(0.0f, 1.0f, barLevels[(size_t)b]);
        const float bh = juce::jmax(1.0f, v * h);

        // base track colour, accent fill
        g.setColour(juce::Colours::black.withAlpha(0.35f));
        g.fillRoundedRectangle({ x, r.getBottom() - bh, bw, bh }, 2.0f);

        g.setColour(findColour(juce::Slider::thumbColourId).withAlpha(0.85f));
        g.fillRoundedRectangle({ x + 1.0f, r.getBottom() - bh + 1.0f, bw - 2.0f, bh - 2.0f }, 2.0f);
    }

}