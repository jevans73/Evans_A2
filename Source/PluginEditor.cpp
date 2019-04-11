/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
Evans_a2AudioProcessorEditor::Evans_a2AudioProcessorEditor (Evans_a2AudioProcessor& p)
    : AudioProcessorEditor (&p), processor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setOpaque (true);
    //startTimerHz(30);
    setSize (400, 300);
    
    
    //=======================================================================================
    
    mixSlider.setSliderStyle(Slider::RotaryVerticalDrag);
    
    mixSlider.setColour(Slider::ColourIds::rotarySliderFillColourId, Colours::yellow);
    mixSlider.setTextBoxStyle(Slider::TextEntryBoxPosition::TextBoxBelow, false, 80, 12);
    mixSlider.setColour(Slider::ColourIds::rotarySliderFillColourId, Colours::lawngreen);
    mixSlider.setRange(0, 100.0f);
    
    mixSlider.setTextValueSuffix("%");
    mixSlider.addListener(this);
    // setResizable(true, true);
    
    mixLabel.setText("Mix", dontSendNotification);
    mixLabel.attachToComponent(&mixSlider, false);
    
    addAndMakeVisible(mixSlider);
    addAndMakeVisible(mixLabel);
    
    //===================================================================================
    
    delaySlider.setSliderStyle(Slider::LinearBar);
    
    delaySlider.setColour(Slider::ColourIds::rotarySliderFillColourId, Colours::yellow);
    delaySlider.setTextBoxStyle(Slider::TextEntryBoxPosition::TextBoxBelow, false, 100, 12);
    delaySlider.setColour(Slider::ColourIds::rotarySliderFillColourId, Colours::lawngreen);
    delaySlider.setRange(0, 100.0f);
    
    delaySlider.setTextValueSuffix("%");
    delaySlider.addListener(this);
    // setResizable(true, true);
    
    delayLabel.setText("Delay", dontSendNotification);
    delayLabel.attachToComponent(&delaySlider, false);
    
    addAndMakeVisible(delaySlider);
    addAndMakeVisible(delayLabel);
    
    //======================================================================================
    
    //freqSlider.setTextBoxStyle(Slider::TextEntryBoxPosition::TextBoxBelow, false, 100, 12);

    //freqSlider.setColour(Slider::ColourIds::trackColourId, Colours::lawngreen);
    //freqSlider.setSliderStyle(Slider::SliderStyle::LinearHorizontal);
    //freqSlider.setRange(20.0f, 2000.0f);
    //freqSlider.setTextValueSuffix("Hz");
    //freqSlider.addListener(this);
    
    //freqLabel.setText("Freq", dontSendNotification);
    //freqLabel.attachToComponent(&freqSlider, false);
    
    //addAndMakeVisible(freqSlider);
    //addAndMakeVisible(freqLabel);

}

Evans_a2AudioProcessorEditor::~Evans_a2AudioProcessorEditor()
{
}

//==============================================================================
void Evans_a2AudioProcessorEditor::paint (Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (Colours::midnightblue);

    g.setColour (Colours::gainsboro);
    g.setFont (30.0f);
    g.drawFittedText ("Evans_A2", getLocalBounds(), Justification::centred, 1);
    
    g.setColour(Colours::hotpink);
    g.setFont(Font ("Arial", 20.0f, Font::italic));
    //g.drawFittedText("help", 10, 10, getWidth(), 50, Justification::left, 1);
    
    g.setColour(Colours::wheat);
    //g.drawRect(150, 10, 250, 100, 5);
    //g.fillRect(150, 10, 250, 100);
    
    //g.drawLine(10.0f, 5.0f, getWidth(), getHeight());
    
    //g.fillEllipse(getWidth()/2, getHeight()/2, 60, 60);
}

void Evans_a2AudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    
    // make sure this makes sense with the style of slider you create
    mixSlider.setBounds(10, getHeight()-140, getWidth()-20, 80);
    //
    //freqSlider.setBounds(5, getHeight()-250, getWidth()-10, 70);
    //
    delaySlider.setBounds(2.5, getHeight()-300, getWidth()-10, 70);
}


void Evans_a2AudioProcessorEditor::sliderValueChanged(Slider *slider) {
    if (slider == &mixSlider)
    {
        // get the value on our current slider, pass that valueto our audio processor
        processor.mixLevel = mixSlider.getValue();
        processor.mixLevel.setTargetValue(mixSlider.getValue());

    } else if ((slider = &delaySlider))
    {
        processor.delayLevel = delaySlider.getValue();
        processor.delayLevel.setTargetValue(delaySlider.getValue());
        
        //processor.freqLevel = freqSlider.getValue();
        //processor.freqLevel.setTargetValue(mixSlider.getValue());
        
    }
}
