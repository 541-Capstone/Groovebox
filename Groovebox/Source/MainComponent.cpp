#include "MainComponent.h"

//==============================================================================
MainComponent::MainComponent()
{

    addAndMakeVisible(playBtn);
    addAndMakeVisible(stopBtn);
    addAndMakeVisible(addTrackBtn);
    addAndMakeVisible(leftBtn);
    addAndMakeVisible(rightBtn);

    addAndMakeVisible(statusLbl);
    addAndMakeVisible(trackCountLbl);

    startTimer(250);


    playBtn.onClick = [this]() { play(); };
    stopBtn.onClick = [this]() { stop(); };
    addTrackBtn.onClick = [this]() {addTrack(); };
    rightBtn.onClick = [this]() {nextTrack(); };
    leftBtn.onClick = [this]() {prevTrack(); };

    numTracks = edit.getTrackList().size();
    numAudioTracks = 1;

    // Make sure you set the size of the component after
    // you add any child components.
    setSize (600, 300);

    // Some platforms require permissions to open input channels so request that here
    if (juce::RuntimePermissions::isRequired (juce::RuntimePermissions::recordAudio)
        && ! juce::RuntimePermissions::isGranted (juce::RuntimePermissions::recordAudio))
    {
        juce::RuntimePermissions::request (juce::RuntimePermissions::recordAudio,
                                           [&] (bool granted) { setAudioChannels (granted ? 2 : 0, 2); });
    }
    else
    {
        // Specify the number of input and output channels that we want to open
        setAudioChannels (2, 2);
    }

    audioTrackList.push_back(edit.getTrackList().objects[edit.getTrackList().objects.size() - 1]);
    currentTrack = 0;
}

MainComponent::~MainComponent()
{
    // This shuts down the audio device and clears the audio source.
    shutdownAudio();
}

void MainComponent::timerCallback() {
    repaint();
}

//==============================================================================
void MainComponent::prepareToPlay (int samplesPerBlockExpected, double sampleRate)
{
    // This function will be called when the audio device is started, or when
    // its settings (i.e. sample rate, block size, etc) are changed.

    // You can use this function to initialise any resources you might need,
    // but be careful - it will be called on the audio thread, not the GUI thread.

    // For more details, see the help for AudioProcessor::prepareToPlay()
}

void MainComponent::getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill)
{
    // Your audio-processing code goes here!

    // For more details, see the help for AudioProcessor::getNextAudioBlock()

    // Right now we are not producing any data, in which case we need to clear the buffer
    // (to prevent the output of random noise)
    bufferToFill.clearActiveBufferRegion();

    if (edit.getTransport().isPlaying()) {
        //DBG("Time: " << edit.getCurrentPlaybackContext()->getPosition());
        DBG("Time: " << edit.getTransport().getCurrentPosition());
    }

}

void MainComponent::releaseResources()
{
    // This will be called when the audio device stops, or when it is being
    // restarted due to a setting change.

    // For more details, see the help for AudioProcessor::releaseResources()
}

//==============================================================================
void MainComponent::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    // You can add your drawing code here!
    statusLbl.setText((juce::String)edit.getTransport().getCurrentPosition(), juce::NotificationType::dontSendNotification);
    trackCountLbl.setText((juce::String)edit.getTrackList().size() + ";" + (juce::String)numTracks, juce::NotificationType::dontSendNotification);
    juce::Array<tracktion_engine::Track*> trackList = edit.getTrackList().objects;
    juce::String trackNames = "";//(int)trackList.size() + "\n";
    for (int i = 0; i < trackList.size(); i++) {
        if (trackList[i] == audioTrackList[currentTrack]) {
            trackNames += "* ";
        }
        trackNames += trackList[i]->getName();
        trackNames += '\n';
    }
    trackCountLbl.setText(trackNames, juce::NotificationType::dontSendNotification);
}

void MainComponent::resized()
{
    // This is called when the MainContentComponent is resized.
    // If you add any child components, this is where you should
    // update their positions.
    juce::Rectangle<int> bounds = getLocalBounds();
    juce::Rectangle<int> buttonRect = getLocalBounds();
    juce::Rectangle<int> textRect = buttonRect.removeFromBottom(buttonRect.getHeight() - 150);

    juce::FlexBox buttonBox{ juce::FlexBox::Direction::row,
                             juce::FlexBox::Wrap::noWrap,
                             juce::FlexBox::AlignContent::stretch,
                             juce::FlexBox::AlignItems::center,
                             juce::FlexBox::JustifyContent::center };

    juce::FlexBox textBox{ juce::FlexBox::Direction::column,
                           juce::FlexBox::Wrap::noWrap,
                           juce::FlexBox::AlignContent::flexStart,
                           juce::FlexBox::AlignItems::flexStart,
                           juce::FlexBox::JustifyContent::flexEnd
    };

    buttonBox.items.add(juce::FlexItem(100, 100, playBtn));
    buttonBox.items.add(juce::FlexItem(100, 100, stopBtn));
    buttonBox.items.add(juce::FlexItem(100, 100, addTrackBtn));
    buttonBox.items.add(juce::FlexItem(100, 100, leftBtn));
    buttonBox.items.add(juce::FlexItem(100, 100, rightBtn));

    textBox.items.add(juce::FlexItem(150, 10, statusLbl));
    textBox.items.add(juce::FlexItem(textRect.getWidth(), textRect.getHeight(), trackCountLbl));

    buttonBox.performLayout(buttonRect);
    textBox.performLayout(textRect);
}

void MainComponent::play() {
    if (playState == PlayState::Stop) {
        playState = PlayState::Play;
        DBG("Play\n");
        edit.getTransport().play(false);
    }

    auto track = tracktion_engine::getAllTracks(edit);
}

void MainComponent::stop() {
    if (playState == PlayState::Play) {
        playState = PlayState::Stop;
    }
    else if (playState == PlayState::Record) {
        playState = PlayState::Stop;
    }
    DBG("Stop\n");

    edit.getTransport().stop(false,false);
    edit.getTransport().setCurrentPosition(0.0);

}

void MainComponent::addTrack() {
    edit.ensureNumberOfAudioTracks(numAudioTracks + 1);
    numTracks++;
    numAudioTracks++;
    audioTrackList.push_back(edit.getTrackList().objects[edit.getTrackList().objects.size() - 1]);
    DBG("Added track " + (juce::String)numAudioTracks + "\nNumber of tracks in edit: " + (juce::String) edit.getTrackList().size() + "\n");
}

void MainComponent::nextTrack() {
    if (currentTrack < audioTrackList.size() - 1) {
        currentTrack++;
        DBG("Paged up. Current track:" + (juce::String)currentTrack + "\n");
    }
}

void MainComponent::prevTrack() {
    if (currentTrack > 0) {
        currentTrack--;
        DBG("Paged up. Current track:" + (juce::String)currentTrack + "\n");
    }
}