/*
  ==============================================================================

    Theme.h
    Created: 17 Aug 2025 4:23:45pm
    Author:  User

  ==============================================================================
*/

/*

#3c42c4 - indig0
#6e51c8 - dark purple
#a065cd - lighter purple
#ce79d2 - pinkish lighter purple
#d68fb8 - pink / peach
#dda2a3 - light orange / peach
#eac4ae - lighter ornge
#f4dfbe - pale yellow
#6e5da3 - lighter purple (bg test)
*/

#pragma once
#include <JuceHeader.h>

namespace Theme {
    // ---- Dark surfaces ----
    static const juce::Colour windowBg{ 0xFF6E51C8 }; 
   // static const juce::Colour panelBg{ 0xFFA065CD }; //   deck
    static const juce::Colour panelBg{ 0xFF6e5da3 }; //   deck
    static const juce::Colour panelBg2{ 0xFF3C42C4 }; //   playlist
    inline juce::Colour panelOutline() { return juce::Colours::black.withAlpha(0.25f); }

    // ---- Light controls / content ----
    static const juce::Colour controlFill{ 0xFFCE79D2 }; // vutton base
    static const juce::Colour controlFillOver{ 0xFFEAC4AE }; // hover
    static const juce::Colour controlFillDown{ 0xFFDEAFA1 }; // mid between #dda2a3/#eac4ae

    // ---- Accents / highlights ----
    static const juce::Colour accent{ 0xFFDDA2A3 };
    static const juce::Colour accentAlt{ 0xFFDDA2A3 };
    static const juce::Colour trackBase{ 0xFFDDA2A3 }; 
    static const juce::Colour trackStrong{ 0xFFCE79D2 }; 

    // ---- Text on dark vs on light ----
    static const juce::Colour textOnDarkMain{ juce::Colours::white.withAlpha(0.92f) };
    static const juce::Colour textOnDarkMuted{ juce::Colours::white.withAlpha(0.65f) };
    static const juce::Colour textOnLight{ juce::Colours::black.withAlpha(0.85f) };

}