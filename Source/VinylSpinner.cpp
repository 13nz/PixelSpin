/*
  ==============================================================================

    VinylSpinner.cpp
    Created: 16 Aug 2025 5:38:18pm
    Author:  Lena

  ==============================================================================
*/

#include <JuceHeader.h>
#include "VinylSpinner.h"
#include "DJAudioPlayer.h"

// draws and rotates a pixel art record image when the player is playing
VinylSpinner::VinylSpinner(DJAudioPlayer* p, double rpmIn) : player(p), rpm(rpmIn)
{
    startTimer(30); // for 30fps
    setInterceptsMouseClicks(false, false);
}

VinylSpinner::~VinylSpinner()
{
    // stop timer when destroyed
    stopTimer();
}

void VinylSpinner::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::transparentBlack); // clear with transparent bg
    if (!vinyl.isValid()) return; // don't draw if image not set

    const int compW = getWidth();
    const int compH = getHeight();
    const int imgW = vinyl.getWidth();
    const int imgH = vinyl.getHeight();

    // target square size
    const int target = std::min(compW, compH);

    // nearest integer scaling for pixel rendering
    int integerScale = std::max(1, target / std::max(imgW, imgH));  
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

// stores new image and redraws with new vinyl
void VinylSpinner::setImage(const juce::Image& img)
{
    vinyl = img;
    repaint();
}

void VinylSpinner::timerCallback()
{
    // only rotate when audio is playing
    if (player != nullptr && player->isPlaying())
    {
        const double fps = 30.0;
        // angle by rpm converted to radians
        currentAngle += juce::MathConstants<double>::twoPi * (rpm / 60.0) * (1.0 / fps);

        // constrain to 0-2pi
        if (currentAngle > juce::MathConstants<double>::twoPi)
        {
            currentAngle -= juce::MathConstants<double>::twoPi;
        }

        // redraw
        repaint();
    }
}
