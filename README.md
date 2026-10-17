# Glitchbox

A multi-effect VST3 plugin for Ableton Live 11 on Windows, in the spirit of ShaperBox and Gross Beat: a chain of effects where any knob can be driven by drawable LFOs, step sequencers or the incoming sound. The full concept lives in the project's concept doc.

Current state: **foundation build**. The plugin loads, passes audio through, and has Mix and Output knobs. Effects arrive in the next milestone.

## Installing on Windows

1. Open the repository's **Actions** tab, pick the latest green **Build** run, and download **Glitchbox-VST3-Windows** at the bottom of the page.
2. Unzip it. You get a folder called `Glitchbox.vst3`.
3. Copy that folder into `C:\Program Files\Common Files\VST3`.
4. In Live 11, open *Preferences > Plug-ins*, turn on **Use VST3 Plug-in System Folders**, and press **Rescan**.
5. Glitchbox appears under *Plug-ins > VST3 > Glitchbox*. Drop it on any audio track.

## Building from source

Requires CMake 3.22+ and Visual Studio 2022 (Windows) or GCC/Clang (Linux). JUCE 8 is downloaded automatically.

```
cmake -S . -B build
cmake --build build --config Release
```

Pass `-DJUCE_DIR=/path/to/JUCE` to use a local JUCE checkout instead.
