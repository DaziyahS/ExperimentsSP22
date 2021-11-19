#include <Mahi/Gui.hpp>
#include <Mahi/Util.hpp>
#include <notes_info.hpp>
#include <syntacts>
#include <random>
#include <iostream> 
#include <fstream> // need to include inorder to save to csv
#include <chrono> 
#include <string> // for manipulating file name

%%
/*
QUESTIONNNNN    
What if the only change I need to make to pilot3_amp is calling
a different version of the function to make the cues and the actual
adjustments would happen inside the chord function... QTNA
*/

// open the namespaces that are relevant for this code
using namespace mahi::gui;
using namespace mahi::util;
using tact::Signal;
using tact::sleep;
using tact::Sequence;

// deteremine application variables
int windowWidth = 1800; // 1920 x 1080 is screen dimensions
int windowHeight = 1000;
std::string my_title= "Play GUI";
ImVec2 buttonSize = ImVec2(400, 65);  // Size of buttons on GUI
int deviceNdx = 5; // Put my device name or number

class MyGui : public Application
{
    // Start by declaring the session variable
    tact::Session s; // this ensures the whole app knows this session

public:
    // this is a constructor. It initializes your class to a specific state
    MyGui() : Application(windowWidth, windowHeight, my_title, 0) {
        s.open(deviceNdx); // opens session with the application
        // keep in mind, if use device name must also use the API

        // Initializing values that require constructors
        sigAmp = std::vector(3,1.0);
        env = std::vector(3, og_env);


        // something the GUI needs *shrugs*
        ImGui::GetIO().ConfigFlags &= ~ImGuiConfigFlags_ViewportsEnable;
        set_background(Cyans::Teal); //background_color = Grays::Black; 
    }

    // Define variables needed throughout the program
    // For creating the signal
    std::vector<Signal> current_signal; // the signal that is previously inputted
    Signal og_env = normall; // is based on my preference
    std::vector<Signal> env; // envelope adjustment, defined as OGs in the constructor
    std::vector<double> sigAmp; // amplitude adjustment, defined as 1s in the constructor
    Signal note1 = a_minor_n1[0], note2 = a_minor_n1[1], note3 = a_minor_n1[2]; // initialize notes based on first drop down
    tact::Sequence finalSignal; // the signal for all adjustments at any time
    // For saving the signal
    std::string sigName; // name for saved signal
    std::string fileLocal; // for storing the signal
    // For playing the signal
    Clock play_clock; // keeping track of time for non-blocking pauses
    bool play_once = false;    // for playing a cue one time
    bool start_loop = false;  // for playing a cue multiple times
    int pause = 1; // can pause the cue at any time

    virtual void update() override
    {
        ImGui::Begin("Playing GUI");

        // for preset, a list of the chords
        const char* items[] = { "A Minor 1", "C Major 2", "D Minor 2", "E Minor 2", "F Major 2", "G Major 2"}; // chord names
        static int item_current = 0; // Starts at first item in the list
        const char* combo_label = items[item_current]; // Gives the preview before anything is played
        if(ImGui::BeginCombo("Presets", combo_label)){ // no flags
            for (int n = 0; n < IM_ARRAYSIZE(items); n++)
            {
                const bool is_selected = (item_current == n);
                if (ImGui::Selectable(items[n], is_selected))
                    item_current = n; // gives a value to each selection when intialized
                if (is_selected)
                    ImGui::SetItemDefaultFocus(); // focuses on item selected, item_current is set as selected
            }
            
            // determine the current signal
            current_signal = signal_list[item_current];

            // determine the notes contained
            // NOTE: PAY ATTENTION TO INDEXING --> STARTS AT 0!!!
            note1 = current_signal[0];
            note2 = current_signal[1];
            note3 = current_signal[2];
            
            ImGui::EndCombo();
        };
        
        // for sustain and delay
        static int sus [3] = {0, 0, 0}; // I think this is the vector being adjusted
        if(ImGui::SliderInt3("Sustain", sus, 0, 3)){  // if use SliderInt2 will have 2 back to back same range
            // see how to call the displayed value
            
            // need to have a way to determine the OG value possibly?
            for(int n = 0; n < 3; n++)
            {
                 switch (sus[n])
                {
                   case 1: // sharp
                        env[n] = sharp;
                        break;
                    case 2: // normal
                        env[n] = normall;
                        break;
                    case 3: // hold
                        env[n] = hold;
                        break;
                    default: // no change
                        env[n] = og_env;
                        break;
                }
            }
        }; 
        static int amp [3] = {0, 0, 0}; // The vector to be adjusted
        if(ImGui::SliderInt3("Intensity", amp, 0, 3)){
            // need to have a way to determine the OG value possibly?
            for(int n = 0; n < 3; n++)
            { 
                switch (amp[n])
                {
                     case 1: // medium-high amplitude
                        sigAmp[n] = 0.8;
                        break;
                    case 2: // high amplitude
                        sigAmp[n] = 0.5;
                        break;
                    case 3: // low amplitude
                        sigAmp[n] = 0.3;
                        break;
                    
                    default: // full amplitude
                        sigAmp[n] = 1;
                        break;
                }
            }
        };
        /* // if I just want it to be an arrow down and would go before it
        if (ImGui::CheckboxFlags("ImGuiComboFlags_NoPreview", &flags, ImGuiComboFlags_NoPreview))
                flags &= ~ImGuiComboFlags_NoArrowButton; // Clear the other flag, as we cannot combine both
        */

        // for play, loop, pause, save
        if(ImGui::Button("Play", buttonSize)){
            // Putting together the final signal
            // initializing the notes themselves
            Signal note1_new = sigAmp[0] * note1 * env[0];
            Signal note2_new = sigAmp[1] * note2 * env[1];
            Signal note3_new = sigAmp[2] * note3 * env[2];
            // creating the signal itself
            finalSignal = note1_new << note2_new << note3_new;
            
            // replace the loop
            pause = 0;
            play_once = true;
            start_loop = true;
            // start_loop = true;
            play_clock.restart();

            // Play the signal
            s.playAll(finalSignal); // play has (channel, signal)
            // sleep(finalSignal.length()); // sleep makes sure you cannot play another cue before that cue is done (in theory)
            // sleep is a blocking function

            std::cout << finalSignal.length() << " s for playing" << std::endl;
            }; 
        ImGui::SameLine();

        // include a bool for play

        if(ImGui::Button("Loop", buttonSize)){
            // Initialize the ability to repeat
            pause = 0;

            // Putting together the final signal
            // initializing the notes themselves
            Signal note1_new = sigAmp[0] * note1 * env[0];
            Signal note2_new = sigAmp[1] * note2 * env[1];
            Signal note3_new = sigAmp[2] * note3 * env[2];
            // creating the signal itself
            finalSignal = note1_new << note2_new << note3_new;

            std::cout << "Pause value is " << pause << std::endl;
            std::cout << finalSignal.length() << " s for playing" << std::endl;
            start_loop = true;
        }; 
        ImGui::SameLine();
        if(ImGui::Button("Reverse", buttonSize)){ // You may want to create a popup with repeat option
            // Putting together the final signal
            // initializing the notes themselves
            Signal note1_new = sigAmp[2] * note1 * env[2];
            Signal note2_new = sigAmp[1] * note2 * env[1];
            Signal note3_new = sigAmp[0] * note3 * env[0];
            // creating the signal itself
            finalSignal = note1_new << note2_new << note3_new;
            
            // replace the loop
            pause = 0;
            play_once = true;
            start_loop = true;
            // start_loop = true;
            play_clock.restart();

            // Play the signal
            s.playAll(finalSignal); // play has (channel, signal)
            // sleep(finalSignal.length()); // sleep makes sure you cannot play another cue before that cue is done (in theory)
            // sleep is a blocking function
        }; 
        
        // Stop the signal with pause loop
        if (pause == 1 || !start_loop){
            s.stopAll(); // stop all channels playing
            start_loop = false; // turn off the looping
            play_once = false; // turn off the play once
        }

        // Play the signal once
        if (play_once)
        {
            s.playAll(finalSignal); // play on all channels
            if(play_clock.get_elapsed_time().as_seconds() > finalSignal.length()){ // if whole signal is played
                play_once = false; // set bool to false
                start_loop = false;
            }
        }
        // Play the signal repeatedly
        else if ((play_clock.get_elapsed_time().as_seconds() > finalSignal.length() && pause == 0) || start_loop)
        { // if pause has not been pressed and is time to restart signal
            s.playAll(finalSignal); // on all channels
            play_clock.restart(); // attempting a non-blocking version of sleep, reset the counter
        }

        if(ImGui::Button("Pause", buttonSize)){
                pause = 1;
            }; 
        ImGui::SameLine();
        if(ImGui::Button("Save", buttonSize))
        {
            ImGui::OpenPopup("saving_things"); // open a popup and name it for calling
            // This just needs its own space, no curlies for the if
        }   
        // necessary variables
        static char buf[31]; // name holder
        if(ImGui::BeginPopup("saving_things")) // if clicked essentially
        {
            ImGui::Text("Signal name is: "); // precursor for people to understand
            ImGui::InputText("##edit", buf, IM_ARRAYSIZE(buf), ImGuiInputTextFlags_CharsNoBlank); // no space allowed, size wanted
            if (ImGui::Button("Close"))
            {
                ImGui::CloseCurrentPopup();
                // put things here for what should happen once closed or else it will run foreverrrr
                std::string predone(buf); // gets rid of null characters
                sigName = predone + ".sig"; // now set the name to what we want
                std::cout << sigName << std::endl;

                // Putting together the final signal
                // **** Might Want To Create A Function For This ****
                // initializing the notes themselves
                Signal note1_new = sigAmp[0] * note1 * env[0];
                Signal note2_new = sigAmp[1] * note2 * env[1];
                Signal note3_new = sigAmp[2] * note3 * env[2];
                // creating the signal itself
                finalSignal = note1_new << note2_new << note3_new;

                // Save the signal
                fileLocal = "../../Library/" + sigName; // create file path for library
                tact::Library::exportSignal(finalSignal, fileLocal); // export signal to library
            }
            ImGui::EndPopup();            
        }

        ImGui::End();
        
    }
};

// actually open GUI
int main() {
    MyGui my_gui;
    my_gui.run();
    return 0;
}