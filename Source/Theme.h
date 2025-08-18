/*
  ==============================================================================

    Theme.h
    Created: 17 Aug 2025 4:23:45pm
    Author:  User

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

namespace Theme {
    // ---- Dark surfaces ----
    static const juce::Colour windowBg{ 0xFF6E51C8 }; // #3c42c4  (deep indigo)
    static const juce::Colour panelBg{ 0xFFA065CD }; // #6e51c8  (deck/list backgrounds)
    inline juce::Colour panelOutline() { return juce::Colours::black.withAlpha(0.25f); }

    // ---- Light controls / content ----
    static const juce::Colour controlFill{ 0xFFCE79D2 }; // #f4dfbe (button base)
    static const juce::Colour controlFillOver{ 0xFFEAC4AE }; // #eac4ae (hover)
    static const juce::Colour controlFillDown{ 0xFFDEAFA1 }; // mid between #dda2a3/#eac4ae

    // ---- Accents / highlights ----
    static const juce::Colour accent{ 0xFFCE79D2 }; // #ce79d2 (primary accent)
    static const juce::Colour accentAlt{ 0xFFD68FB8 }; // #d68fb8 (secondary)
    static const juce::Colour trackBase{ 0xFFDDA2A3 }; // #dda2a3 (slider rails, grid)
    static const juce::Colour trackStrong{ 0xFFCE79D2 }; // #a065cd (active fills)

    // ---- Text on dark vs on light ----
    static const juce::Colour textOnDarkMain{ juce::Colours::white.withAlpha(0.92f) };
    static const juce::Colour textOnDarkMuted{ juce::Colours::white.withAlpha(0.65f) };
    static const juce::Colour textOnLight{ juce::Colours::black.withAlpha(0.85f) };

}