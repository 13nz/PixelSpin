/*
  ==============================================================================

    EffectsDeck.cpp
    Created: 19 Aug 2025 3:31:42pm
    Author:  User

  ==============================================================================
*/

#include <JuceHeader.h>
#include "EffectsDeck.h"

//==============================================================================
// https://docs.juce.com/master/namespacedsp.html

void EffectsDeck::prepare(double sampleRate, int maxBlockSize, int numChannels)
{
    juce::dsp::ProcessSpec spec{ sampleRate,
                                  (juce::uint32)maxBlockSize,
                                  (juce::uint32)numChannels };
    reverb.prepare(spec);

    // reverb
    params.roomSize = 0.35f;
    params.damping = 0.25f;
    params.width = 1.00f;
    params.wetLevel = 0.0f;  // start fully dry
    params.dryLevel = 1.0f;
    params.freezeMode = 0.0f;

    reverb.setParameters(params);

    // chorus
    chorus.prepare(spec);
    setChorusAmount(0.0f);

    // delay 
    const int maxDelaySamples = (int)std::ceil(sampleRate * 2.0);
    delay = std::make_unique<DL>(maxDelaySamples);

    // prepare with mono (1 channel)
    juce::dsp::ProcessSpec monoSpec{ sampleRate, (juce::uint32)maxBlockSize, 1u };                
    delay->prepare(monoSpec);

    delay->reset();
    setDelayAmount(0.0f);

    // compression
    compressor.prepare(spec);
    setCompressionAmount(0.0f);
}

void EffectsDeck::reset()
{
    reverb.reset();
    chorus.reset();
    compressor.reset();

    if (delay) delay->reset();
}

void EffectsDeck::process(juce::AudioBuffer<float>& buffer)
{


    juce::dsp::AudioBlock<float> block(buffer);
    juce::dsp::ProcessContextReplacing<float> ctx(block);

    // chorus before reverb
    if (chorusMix > 0.0f)
        chorus.process(ctx);

    // delay
    if (delay && delayMix > 0.0f)
    {
        const int nCh = buffer.getNumChannels();
        const int nSmps = buffer.getNumSamples();

        const float timeSamples = (float)(delayTimeMs * 0.001 * fs);
        delay->setDelay(timeSamples);

        for (int i = 0; i < nSmps; ++i)
        {
            const float inL = (nCh > 0) ? buffer.getSample(0, i) : 0.0f;
            const float inR = (nCh > 1) ? buffer.getSample(1, i) : 0.0f;

            // mono 
            const float monoIn = 0.5f * (inL + inR);

            // read delayed sample & write with feedback
            const float dl = delay->popSample(0);
            delay->pushSample(0, monoIn + dl * delayFeedback);

            // mix delayed signal back to both channels
            const float outL = inL * (1.0f - delayMix) + dl * delayMix;
            const float outR = inR * (1.0f - delayMix) + dl * delayMix;

            if (nCh > 0) buffer.setSample(0, i, outL);
            if (nCh > 1) buffer.setSample(1, i, outR);
        }
    }

    // reverb
    if (params.wetLevel > 0.0f)
    {
        reverb.setParameters(params);
        reverb.process(ctx);
    }

    // compression
    compressor.process(ctx);
}


// sets reverb amount
void EffectsDeck::setReverbAmount(float wet01)
{
    wet01 = juce::jlimit(0.0f, 1.0f, wet01);

    // reverb parameters
    params.roomSize = 0.60f + 0.40f * wet01;   
    params.damping = 0.10f + 0.30f * wet01;   
    params.wetLevel = 0.50f + 0.50f * wet01;   
    params.dryLevel = 1.0f;

}

// setc chorus amount
void EffectsDeck::setChorusAmount(float amt01)
{
    amt01 = juce::jlimit(0.0f, 1.0f, amt01);
    chorusMix = amt01;

    const float rateHz = 0.15f + 0.95f * amt01;   // 0.15 .. 1.10 Hz (slow)
    const float depth = 0.12f + 0.38f * amt01;   // 0.12 .. 0.50 (moderate)
    const float centreDelay = 8.0f + 10.0f * amt01;   // 8 .. 18 ms (chorus, not flange)
    const float feedback = 0.00f + 0.08f * amt01;   // 0 .. 0.08 (tiny)
    const float mix = 0.08f + 0.35f * amt01;   // 8% .. 43% (audible)

    chorus.setRate(rateHz);
    chorus.setDepth(depth);
    chorus.setCentreDelay(centreDelay);
    chorus.setFeedback(feedback);
    chorus.setMix(mix);
}


// sets compression
void EffectsDeck::setCompressionAmount(float amt01)
{
    amt01 = juce::jlimit(0.0f, 1.0f, amt01);

    // map knob amount to compressor params
    const float threshold = -6.0f - 24.0f * amt01; 
    const float ratio = 1.0f + 9.0f * amt01; 

    compressor.setThreshold(threshold);
    compressor.setRatio(ratio);
    compressor.setAttack(5.0f);
    compressor.setRelease(50.0f);
}

// stes delay amount
void EffectsDeck::setDelayAmount(float amt01)
{
    amt01 = juce::jlimit(0.0f, 1.0f, amt01);

    delayTimeMs = 90.0f + 360.0f * amt01;
    delayMix = 0.00f + 0.55f * amt01;
    delayFeedback = 0.00f + 0.45f * amt01;
}