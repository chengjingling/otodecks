/*
  ==============================================================================

    DeckGUI.h
    Created: 31 Jan 2023 4:32:33pm
    Author:  cheng

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "DJAudioPlayer.h"
#include "WaveformDisplay.h"

class DeckGUI : public juce::Component,
                public juce::Button::Listener,
                public juce::Slider::Listener,
                public juce::Timer
{
public:
    DeckGUI(DJAudioPlayer* player,
            juce::AudioFormatManager & formatManagerToUse,
            juce::AudioThumbnailCache & cacheToUse);
    ~DeckGUI();

    void paint(juce::Graphics& g) override;
    void resized() override;

    void buttonClicked(juce::Button* button) override;
    void sliderValueChanged(juce::Slider* slider) override;

    void timerCallback() override;

    void loadTrack(juce::String filePath);

    juce::String formatTime(double time);

    void clearDeck();

    std::vector<double> getSliderValues();
    void setSliderValues(double position, double volume, double speed);

private:
    juce::Label trackTitle;
    juce::Label trackPosition;
    juce::Label trackLength;
    juce::Slider posSlider;
    juce::Image rewindImage{ juce::ImageFileFormat::loadFrom(BinaryData::rewindbutton_png, BinaryData::rewindbutton_pngSize) };
    juce::ImageButton rewindButton;
    juce::Image playImage{ juce::ImageFileFormat::loadFrom(BinaryData::playbutton_png, BinaryData::playbutton_pngSize) };
    juce::Image pauseImage{ juce::ImageFileFormat::loadFrom(BinaryData::pausebutton_png, BinaryData::pausebutton_pngSize) };
    juce::ImageButton playPauseButton;
    juce::Image stopImage{ juce::ImageFileFormat::loadFrom(BinaryData::stopbutton_png, BinaryData::stopbutton_pngSize) };
    juce::ImageButton stopButton;
    juce::Image forwardImage{ juce::ImageFileFormat::loadFrom(BinaryData::forwardbutton_png, BinaryData::forwardbutton_pngSize) };
    juce::ImageButton forwardButton;
    juce::Image loopImage{ juce::ImageFileFormat::loadFrom(BinaryData::loopbutton_png, BinaryData::loopbutton_pngSize) };
    juce::ImageButton loopButton;
    bool isLooping = false;
    juce::Slider volSlider;
    juce::Label volSliderLabel;
    juce::Slider speedSlider;
    juce::Label speedSliderLabel;

    DJAudioPlayer* player;
    WaveformDisplay waveformDisplay;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DeckGUI)
};