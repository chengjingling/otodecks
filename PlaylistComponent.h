/*
  ==============================================================================

    PlaylistComponent.h
    Created: 6 Feb 2023 2:35:01pm
    Author:  cheng

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include <vector>
#include "DeckGUI.h"

class PlaylistComponent : public juce::Component,
                          public juce::TableListBoxModel,
                          public juce::Button::Listener,
                          public juce::FileDragAndDropTarget,
                          public juce::TextEditor::Listener
{
public:
    PlaylistComponent(DeckGUI* deckGUI1, DeckGUI* deckGUI2);
    ~PlaylistComponent() override;

    void paint(juce::Graphics&) override;
    void resized() override;

    int getNumRows() override;

    void paintRowBackground(juce::Graphics&,
                            int rowNumber,
                            int width,
                            int height,
                            bool rowIsSelected) override;

    void paintCell(juce::Graphics&,
        int rowNumber,
        int columnId,
        int width,
        int height,
        bool rowIsSelected) override;

    juce::Component* refreshComponentForCell(int rowNumber,
                                             int columnId,
                                             bool rowIsSelected,
                                             juce::Component* existingComponentToUpdate) override;

    void buttonClicked(juce::Button* button) override;
    void textEditorTextChanged(juce::TextEditor& editor) override;
    void textEditorFocusLost(juce::TextEditor& editor) override;
    void sortOrderChanged(int newSortColumnId, bool isForwards) override;

    bool isInterestedInFileDrag(const juce::StringArray& files) override;
    void filesDropped(const juce::StringArray& files, int x, int y) override;

    void readFromDeckFile();
    void writeToDeckFile();

    void readFromPlaylistFile();
    void writeToPlaylistFile();

    void importTrack(juce::File file);
    void deleteTrack();

private:
    juce::AudioFormatManager formatManager;
    juce::TextButton loadAButton{ "LOAD DECK A" };
    juce::TextButton clearAButton{ "CLEAR DECK A" };
    juce::TextEditor searchBar;
    std::vector<int> searchedRows;
    juce::TextButton loadBButton{ "LOAD DECK B" };
    juce::TextButton clearBButton{ "CLEAR DECK B" };
    DeckGUI* deckGUI1;
    DeckGUI* deckGUI2;
    std::map<juce::String, juce::String> existingDecks;
    juce::TableListBox tableComponent;
    std::vector<juce::String> existingFiles;
    std::vector<juce::String> trackTitles;
    std::vector<juce::String> trackLengths;
    juce::TextButton importButton{ "IMPORT TRACKS" };
    std::vector<juce::String> importedTracks;
    juce::TextButton deleteButton{ "DELETE SELECTED" };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PlaylistComponent)
};