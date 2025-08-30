# DJApp 🎛️🎶

A cross-platform **DJ mixing application** built with the [JUCE](https://juce.com/) framework in C++.  
It features dual decks, waveform displays, custom pixel-art UI elements, and built-in audio effects.
---

<img width="1598" height="927" alt="image" src="https://github.com/user-attachments/assets/d044dd3b-7a26-4ef9-9ec9-0224b0713a1c" />


---

## ✨ Features

- **Dual Decks**
  - Independent playback for two tracks.
  - Load audio files via file chooser, drag-and-drop, or playlist double-click.
  - Volume, speed, and position sliders for each deck.
  - Play, Stop, Load, and Clear buttons with crisp pixel-art icons.

- **Waveform Display**
  - Real-time waveform visualization using `AudioThumbnail`.
  - Seek playback by clicking on the waveform.
  - Playhead tracking synced to transport position.

- **Playlist Component**
  - Persistent track library stored as JSON in the user’s app data folder.
  - Track titles and lengths displayed in a table.
  - Double-click to load + play tracks on the linked deck.
  - “Clear all” option to reset playlists.

- **Vinyl Spinner**
  - Pixel-art vinyl disc that spins in real time when a deck is playing.
  - Custom vinyl art selectable from the `Assets/Vinyls/` folder.

- **Effects Deck**
  - Four audio effects powered by `juce_dsp`:
    - **Reverb**
    - **Chorus**
    - **Compression**
    - **Delay**
  - Controlled via custom **PixelKnob** widgets with discrete stepped positions.

- **Sample pads**
  - Five audio samples:
    - **Vinyl scratch**
    - **Vinyl glitch**
    - **Drum**
    - **Kick**
    - **Snare**
  - Controlled via custom **PixelKnob** widgets with discrete stepped positions.

- **Mixer**
  - Mix the two tracks with crossfade
  - Controlled via slider and buttons to snap to deck.

- **Custom UI**
  - hand drawn pixel-art themed controls (`PixelButton`, `PixelKnob`).
  - Custom Look-and-Feel for sliders and window title bar.
  - Responsive layout for decks and playlists.

---


## 🛠️ Building

### Prerequisites
- [JUCE](https://juce.com/) 7+
- C++17 compatible compiler (MSVC, Clang, or GCC)
- CMake or Projucer for project generation


---

## 🎹 Usage

1. **Load Tracks**
   - Use the **Load** button to load audio files onto a deck.

2. **Control Playback**
   - Adjust **Volume**, **Speed**, and **Position** with sliders.
   - Start/stop with pixel-art **Play/Stop** buttons.
   - Clear the deck with the **Clear** button.

3. **Effects**
   - Twist the knobs (Reverb, Chorus, Compression, Delay) with the mouse or scroll.
   - Knobs step between 0–6 for discrete effect levels.

4. **Samples**
   - Click the pads to play.
   - 5 different sound effects

5. **Vinyls**
   - Choose a vinyl art from the dropdown (loaded from `Assets/Vinyls/`).
   - Vinyl spins when the track plays.

6. **Playlists**
   - Tracks persist automatically between sessions.

7. **Mixer**
   - Adjust gains and crossfade

---

## 🚀 Future Improvements
- More samples
- More FX
- More customizable UI themes

---

## 📜 License
This project is for educational and personal use.  
Check JUCE licensing terms if you plan to distribute.

---

Made with ❤️ in C++ and JUCE.
