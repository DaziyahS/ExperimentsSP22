#include <Mahi/Gui.hpp>
#include <Mahi/Util.hpp>
#include <Mahi/Util/Logging/Log.hpp>
#include <syntacts>
#include <random>
#include <iostream> 
#include <fstream> // need to include inorder to save to csv
#include <chrono> 
#include <string> // for manipulating file name

// local includes
#include <Chord.hpp>
#include <Note.hpp> // would this imply that the .cpp functionality is attached?
// #include <notes_info.hpp> // probably do not need

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
// std::string deviceNdx = "Speakers (USB Sound Device)"; // Put my device name or number, is for at home name
int deviceNdx = 6;
// tactors of interest
int topTact = 4;
int botTact = 6;
int leftTact = 0;
int rightTact = 2;

// trying to figure out how to save to an excel document
std::string saveSubject; // experiment details, allows me to customize
std::ofstream file_name; // this holds the trial information

class MyGui : public Application
{
    // Start by declaring the session variable
    tact::Session s; // this ensures the whole app knows this session

public:
    // this is a constructor. It initializes your class to a specific state
    MyGui() : 
    Application(windowWidth, windowHeight, my_title, 0),
    chordNew(),
    channelSignals(3)
    {
        s.open(deviceNdx); // , tact::API::MME); // opens session with the application
        // keep in mind, if use device name must also use the API
        // something the GUI needs *shrugs*
        ImGui::GetIO().ConfigFlags &= ~ImGuiConfigFlags_ViewportsEnable;
        set_background(Cyans::Teal); //background_color = Grays::Black; 

         // trying to figure out how to save to an excel document
        file_name.open("../../Data/" + saveSubject + "_piloting.csv"); // saves the csv name for all parameters
        file_name << "Trial" << "," << "Chord" << "," << "Sus" << "," << "Amp" << "," << "IsSim" << "," << "IsMajor" << ","
                  << "Valence" << "," << "Arousal" << "," << "Notes" << std::endl; // theoretically setting up headers
     }

    // Define variables needed throughout the program
    // For creating the signal
    std::string defaultName = "a_minor_n1"; // what is the first chord name
    std::string currentChord; // holds name of current chord based on selection
    Chord chordNew;
    std::vector<tact::Signal> channelSignals;
    bool isSim = false; // default is sequential
    // For saving the signal
    std::string sigName; // name for saved signal
    std::string fileLocal; // for storing the signal
    // For saving records
    int trial_num = 1;
    int val = 0, arous = 0;
    // For playing the signal
    Clock play_clock; // keeping track of time for non-blocking pauses
    bool play_once = false;    // for playing a cue one time
    bool start_loop = false;  // for playing a cue multiple times
    int pause = 1; // can pause the cue at any time
    // for screens
    std::string screen_name = "trial_screen";
    int exp_num = 1;

    virtual void update() override
    {
        ImGui::Begin("Playing GUI");
        ImGui::Text("The current experiment number is: %i", exp_num);
        
        if (screen_name == "trial_screen")
        {
            trialScreen();
        }
        else if (screen_name == "mid_screen")
        {
            midScreen();
        }
        
        ImGui::End();

    }

// for between trials
void midScreen()
{
    // in case I can possibly make a new file this way!
    if(ImGui::Button("New Name", buttonSize))
    {
        ImGui::OpenPopup("change_files"); // open a popup and name it for calling
        // This just needs its own space, no curlies for the if
    }  
    static char name[12]; // info holder 
    if(ImGui::BeginPopup("change_files")) // if clicked essentially
    {
        ImGui::Text("What is the new name: "); // precursor for me to understand
        ImGui::InputText("##edit", name, IM_ARRAYSIZE(name)); // size wanted           
        if (ImGui::Button("Close"))
        {
            ImGui::CloseCurrentPopup();
            // put things here for what should happen once closed or else it will run foreverrrr 
            
            // theoretically creates a new file 
            file_name.close();
            file_name.open("../../Data/" + saveSubject + name + "_piloting.csv"); // saves the csv name for all parameters
            file_name << "Trial" << "," << "Chord" << "," << "Sus" << "," << "Amp" << "," << "IsSim" << "," << "IsMajor" << ","
                      << "Valence" << "," << "Arousal" << "," << "Notes" << std::endl; // theoretically setting up headers
        }
        ImGui::EndPopup();
    }
    
    if (ImGui::Button("Next Screen",buttonSize))
    {
        screen_name = "trial_screen";
        exp_num++; // go to new experiment number
        trial_num = 1; // reset the trial number
        file_name << "This is now experiment " << exp_num << std::endl; // create a line to distinguish the experiments
    }
}

// for trial
void trialScreen()
{
    // for preset, a list of the chords
    const char* items[] = {"D Minor 3", "D Major 3", "E Minor 3", "E Major 3"}; // chord names
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
        currentChord = chordNew.signal_list[item_current+14]; // theoretically gives name needed

        ImGui::EndCombo();
    };

    // for sustain and delay
    static int sus = 0; // I think this is the vector being adjusted
    if(ImGui::SliderInt("Sustain", &sus, 0, 2)){  // if use SliderInt2 will have 2 back to back same range
        // sus is determined here, this is duration value
    }; 
    static int amp = 0; // The value to be adjusted
    if(ImGui::SliderInt("Intensity", &amp, 0, 3)){
        // amp is determined here, this is amplitude value
    };
    /* // if I just want it to be an arrow down and would go before it
    if (ImGui::CheckboxFlags("ImGuiComboFlags_NoPreview", &flags, ImGuiComboFlags_NoPreview))
            flags &= ~ImGuiComboFlags_NoArrowButton; // Clear the other flag, as we cannot combine both
    */
    if(ImGui::Button("Sequential", buttonSize)){
        isSim = 0; // false
    };
    ImGui::SameLine();
    if(ImGui::Button("Simultaneous", buttonSize)){
        isSim = 1; // true
    };

    // for play, loop, pause, save
    if(ImGui::Button("Play", buttonSize)){
        chordNew = Chord(currentChord, sus, amp, isSim);
        channelSignals = chordNew.playValues(); // get the values of the signal

        // replace the loop
        pause = 0;
        play_once = true;
        start_loop = true;
        // start_loop = true;
        play_clock.restart();

        // Play the signal
        s.play(leftTact, channelSignals[0]); // play has (channel, signal)
        s.play(botTact, channelSignals[1]); 
        s.play(rightTact, channelSignals[2]); 
        // sleep(finalSignal.length()); // sleep makes sure you cannot play another cue before that cue is done (in theory)
        // sleep is a blocking function

        }; 
    ImGui::SameLine();
    if(ImGui::Button("Loop", buttonSize)){
        // Initialize the ability to repeat
        pause = 0;

        // determine the signal
        chordNew = Chord(currentChord, sus, amp, isSim);
        channelSignals = chordNew.playValues(); // get the values of the signal

        start_loop = true;
    }; 
    ImGui::SameLine();        
    if(ImGui::Button("Pause", buttonSize)){
            pause = 1;
        }; 
    /*
    if(ImGui::Button("Reverse", buttonSize)){ // You may want to create a popup with repeat option
        chordNew(currentChord, sus, amp, isSim);
        channelSignals = chordNew.playValues(); // get the values of the signal


        // replace the loop
        pause = 0;
        play_once = true;
        start_loop = true;
        // start_loop = true;
        play_clock.restart();

        // Play the signal
        s.play(1, channelSignals[0]); // play has (channel, signal)
        s.play(2, channelSignals[1]); 
        s.play(3, channelSignals[2]); 
        // sleep(finalSignal.length()); // sleep makes sure you cannot play another cue before that cue is done (in theory)
        // sleep is a blocking function
    }; */

    // Stop the signal with pause loop
    if (pause == 1 || !start_loop){
        s.stopAll(); // stop all channels playing
        start_loop = false; // turn off the looping
        play_once = false; // turn off the play once
    }

    // Play the signal once
    if (play_once)
    {
        // Play the signal
        s.play(leftTact, channelSignals[0]); // play has (channel, signal)
        s.play(botTact, channelSignals[1]); 
        s.play(rightTact, channelSignals[2]); 
        if(play_clock.get_elapsed_time().as_seconds() > channelSignals[0].length()){ // if whole signal is played
            play_once = false; // set bool to false
            start_loop = false;
            pause = 1;
        }
    }
    // Play the signal repeatedly
    else if ((play_clock.get_elapsed_time().as_seconds() > channelSignals[0].length() && pause == 0) || start_loop)
    { // if pause has not been pressed and is time to restart signal
        // Play the signal
        s.play(leftTact, channelSignals[0]); // play has (channel, signal)
        s.play(botTact, channelSignals[1]); 
        s.play(rightTact, channelSignals[2]); 
        play_clock.restart(); // attempting a non-blocking version of sleep, reset the counter
    }

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
            if (isSim == false){
            ImGui::CloseCurrentPopup();
            // put things here for what should happen once closed or else it will run foreverrrr
            std::string predone(buf); // gets rid of null characters
            sigName = predone + ".sig"; // now set the name to what we want

            // determine the signal
            chordNew = Chord(currentChord, sus, amp, isSim);
            channelSignals = chordNew.playValues(); // get the values of the signal


            // Save the signal
            fileLocal = "../../Library/" + sigName; // create file path for library
            tact::Library::exportSignal(channelSignals[0], fileLocal); // export signal to library
            }
            else{
                // do nothing because cannot because simultaneous and all different
            }
        }
        ImGui::EndPopup();            
    }
    ImGui::SameLine();
    if(ImGui::Button("Log", buttonSize))
    {
        ImGui::OpenPopup("logging_things"); // open a popup and name it for calling
        // This just needs its own space, no curlies for the if

        // need to update information that was used even if did not press play
        chordNew = Chord(currentChord, sus, amp, isSim);
    }  
    static char num[120]; // info holder 
    if(ImGui::BeginPopup("logging_things")) // if clicked essentially
    {
        ImGui::Text("What are your notes: "); // precursor for me to understand
        ImGui::InputText("##edit", num, IM_ARRAYSIZE(num)); // size wanted
        ImGui::InputInt("valence?", &val);
        ImGui::SameLine();
        ImGui::InputInt("arousal?", &arous);            
        if (ImGui::Button("Close"))
        {
            ImGui::CloseCurrentPopup();
            // put things here for what should happen once closed or else it will run foreverrrr
            std::string notes_taken(num); // gets rid of null characters
            // LOG(Info) << "Notes for trial " << trial_num << " are: " + notes_taken + " for the " << item_current << " chord with a hold of " << sus << " and amplitude of " << amp << "."; // now log this information
            // LOG(Info) << trial_num << "," << item_current << "," << sus << "," << amp << "," << val << "," << arous << "," + notes_taken;

            // excel
            file_name << trial_num << ",";
            file_name <<  item_current << "," << sus << "," << amp << "," << isSim << "," << chordNew.getMajor() << ",";
            file_name << val << "," << arous << ",";
            file_name << notes_taken << std::endl;

            trial_num++;
        }
        ImGui::EndPopup();
    }
    if (ImGui::Button("Next Screen",buttonSize))
    {
        screen_name = "mid_screen";
        std::cout << "We are now going to trial " << exp_num+1 << "after " << trial_num << " trials." << std::endl;
    }
}

};

// actually open GUI
int main() {
    std::cout << "What is today's date followed by a letter of the alphabet?" << std::endl;
    std::cin >> saveSubject;

    MyGui my_gui;
    my_gui.run();
    return 0;
} 
