#include "MainComponent.h"
#include <iostream>
#include <fstream>

MainComponent::MainComponent()
{
    // set size of component
    setSize(1370, 835);

    if (juce::RuntimePermissions::isRequired(juce::RuntimePermissions::recordAudio)
        && juce::RuntimePermissions::isGranted(juce::RuntimePermissions::recordAudio) == false) // if microphone access is required and permission not granted
    {
        // request for permission and set 2 output channels
        juce::RuntimePermissions::request(juce::RuntimePermissions::recordAudio,
                                           [&] (bool granted) { setAudioChannels(granted ? 2 : 0, 2); });
    }

    else
    {
        // set 2 output channels
        setAudioChannels(0, 2);
    }

    // add and make visible decks and playlist
    addAndMakeVisible(deckGUI1);
    addAndMakeVisible(deckGUI2);
    addAndMakeVisible(playlistComponent);

    // register basic formats for the audio files
    formatManager.registerBasicFormats();

    // read previously loaded tracks from deck file
    readFromDeckFile();

    // set font
    getLookAndFeel().setDefaultSansSerifTypefaceName("Avenir LT Std");
}

MainComponent::~MainComponent()
{
    // shut down audio device and clear audio source
    shutdownAudio();
}

void MainComponent::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    player1.prepareToPlay(samplesPerBlockExpected, sampleRate);
    player2.prepareToPlay(samplesPerBlockExpected, sampleRate);
    mixerSource.prepareToPlay(samplesPerBlockExpected, sampleRate);
    mixerSource.addInputSource(&player1, false);
    mixerSource.addInputSource(&player2, false);
}

void MainComponent::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    mixerSource.getNextAudioBlock(bufferToFill);
}

void MainComponent::releaseResources()
{
    player1.releaseResources();
    player2.releaseResources();
    mixerSource.releaseResources();
}

void MainComponent::paint(juce::Graphics& g)
{
    
}

void MainComponent::resized()
{
    deckGUI1.setBounds(0, 0, getWidth() / 2, getHeight() / 2);
    deckGUI2.setBounds(getWidth() / 2, 0, getWidth() / 2, getHeight() / 2);
    playlistComponent.setBounds(0, getHeight() / 2, getWidth(), getHeight() / 2);
}

void MainComponent::readFromDeckFile()
{
    // open deck.txt which contains the deck numbers and file paths which the user had loaded previously
    std::ifstream deckFile("deck.txt");
    std::string line;
    std::vector<std::string> lines;

    while (std::getline(deckFile, line))
    {
        lines.push_back(line);
    }

    for (int i = 0; i < lines.size(); i += 5)
    {
        // load each track into the respective deck and set their respective slider values
        if (lines[i] == "1")
        {
            deckGUI1.loadTrack(lines[i + 1]);
            deckGUI1.setSliderValues(std::stod(lines[i + 2]), std::stod(lines[i + 3]), std::stod(lines[i + 4]));
        }

        else
        {
            deckGUI2.loadTrack(lines[i + 1]);
            deckGUI2.setSliderValues(std::stod(lines[i + 2]), std::stod(lines[i + 3]), std::stod(lines[i + 4]));
        }
    }

    // close the file
    deckFile.close();
}