/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainComponent   : public AudioAppComponent, public ChangeListener
{
public:
    //==============================================================================
    MainComponent();
    ~MainComponent();

    //==============================================================================
    void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock (const AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;

    //==============================================================================
    void paint (Graphics& g) override;
    void resized() override;
    void changeListenerCallback (ChangeBroadcaster* source) override;

private:
   
    //==============================================================================
    // Your private member variables go here...
    enum TransportState
       {
           Stopped,
           Starting,
           Playing,
           Pausing,
           Paused,
           Stopping
       };
    
    void openButtonClicked();
    void encodeFlac(AudioFormatReader* reader);
    void decodeFlac();
    
    void changeState (TransportState newState);
    void playButtonClicked();
    void stopButtonClicked();
    void flacButtonClicked();

    TextButton openButton;
    TextButton playButton;
    TextButton stopButton;
    TextButton flacButton;

    AudioFormatManager formatManager;
    FlacAudioFormat flacFormat;
    MemoryOutputStream* mem;    //store flac audio file
    std::unique_ptr<AudioFormatReaderSource> readerSource;
    std::unique_ptr<AudioFormatReaderSource> flacSource;

    AudioTransportSource transportSource;
    AudioTransportSource transportSourceFlac;

    TransportState state;
    

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
