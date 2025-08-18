/*
  ==============================================================================

    VinylSpinner.cpp
    Created: 16 Aug 2025 5:38:18pm
    Author:  User

  ==============================================================================
*/

#include <JuceHeader.h>
#include "VinylSpinner.h"
#include "DJAudioPlayer.h"

//==============================================================================
VinylSpinner::VinylSpinner(DJAudioPlayer* p, double rpmIn) : player(p), rpm(rpmIn)
{
    startTimer(30);
    setInterceptsMouseClicks(false, false);
}

VinylSpinner::~VinylSpinner()
{
    stopTimer();
}

void VinylSpinner::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::transparentBlack);
    if (!vinyl.isValid()) return;

    const int compW = getWidth();
    const int compH = getHeight();
    const int imgW = vinyl.getWidth();
    const int imgH = vinyl.getHeight();

    // arget square (spinner component is square in your layout)
    const int target = std::min(compW, compH);

    // integer scaling for pixel rendering
    int integerScale = std::max(1, target / std::max(imgW, imgH));  // floor
    int drawW = imgW * integerScale;
    int drawH = imgH * integerScale;

    // center on whole pixels (avoid half-pixel blur)
    const int cx = compW / 2;
    const int cy = compH / 2;

    // build transform:
    // move image center to origin
    // scale by integer factor (no interpolation)
    // rotate around origin
    // translate to component center (integer coords)
    juce::AffineTransform t =
        juce::AffineTransform::translation(-imgW * 0.5f, -imgH * 0.5f)
        .scaled((float)integerScale, (float)integerScale)
        .rotated((float)currentAngle)
        .translated((float)cx, (float)cy);

    // nearest-neighbor for pixel art (no blur)
    g.setImageResamplingQuality(juce::Graphics::lowResamplingQuality);

    // false avoids extra filtering passes
    g.drawImageTransformed(vinyl, t, false);
}




void VinylSpinner::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..

}

void VinylSpinner::setImage(const juce::Image& img)
{
    vinyl = img;
    repaint();
}

void VinylSpinner::timerCallback()
{
    if (player != nullptr && player->isPlaying())
    {
        const double fps = 30.0;
        currentAngle += juce::MathConstants<double>::twoPi * (rpm / 60.0) * (1.0 / fps);

        if (currentAngle > juce::MathConstants<double>::twoPi)
        {
            currentAngle -= juce::MathConstants<double>::twoPi;
        }

        repaint();
    }
}
