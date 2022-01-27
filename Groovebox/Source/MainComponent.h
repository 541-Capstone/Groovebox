#pragma once

#include <JuceHeader.h>
//#include "Utilities.h"

//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainComponent  : public juce::AudioAppComponent, public juce::Timer
{
public:
    //==============================================================================
    MainComponent();
    ~MainComponent() override;

    //==============================================================================
    void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;

    //==============================================================================
    void paint (juce::Graphics& g) override;
    void resized() override;

    void timerCallback() override;

    enum class PlayState {
        Play,
        Stop,
        Record
    };

private:
    //==============================================================================
    // Your private member variables go here...
    tracktion_engine::Engine engine { ProjectInfo::projectName };

    /*te::Edit::Options editOptions{ engine,
        te::createEmptyEdit(engine),
        te::ProjectItemID::createNewID(0),
        te::Edit::EditRole::forEditing,
        nullptr,
        0 };*/



    tracktion_engine::Edit edit{ engine,
                                 tracktion_engine::createEmptyEdit(engine),
                                 tracktion_engine::Edit::EditRole::forEditing,
                   nullptr,
                   0 };

    juce::TextButton playBtn{ "Play" };
    juce::TextButton stopBtn{ "Stop" };
    juce::TextButton addTrackBtn{ "Add Track" };
    juce::Label statusLbl{ "statusLbl", "Status" };
    juce::Label trackCountLbl{ "trackCountLbl", "Track count" };

    PlayState playState = PlayState::Stop;

    void play();
    void stop();
    void record();
    void addTrack();


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
