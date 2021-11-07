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

        // so the current chord can play immediately
        currentChord = chordNew.signal_list[14];
     }

    // Define variables needed throughout the program
    // For creating the signal
    std::string currentChord; // holds name of current chord based on selection
    Chord chordNew;
    std::vector<tact::Signal> channelSignals;
    bool isSim = false; // default is sequential
    int amp, sus;
    // For saving the signal
    std::string sigName; // name for saved signal
    std::string fileLocal; // for storing the signal
    // For saving records
    int trial_num = 0; // counter for overall trials
    int experiment_num = 5; // amount of trials in experiment
    int val = 0, arous = 0;
    int final_trial_num = 5;
    // For playing the signal
    Clock play_clock; // keeping track of time for non-blocking pauses
    bool playTime = false;   // for knowing how long to play cues
    // Set up timing within the trials itself
    Clock trial_clock;
    double timeRespond;
    // The amplitudes in a vector
    std::vector<int> list = {0, 1, 2, 3};
    // Vector for if play can be pressed
    bool dontPlay = false;
    bool first_in_trial = false;
    // for collecting data
    int item_current_val = 0;
    int item_current_arous = 0;
    int currentChordNum = 14;
    // for screens
    std::string screen_name = "begin_screen";

    virtual void update() override
    {
        ImGui::Begin("Playing GUI");
        ImGui::Text("The current experiment number is: %i", trial_num);
        
        if (screen_name == "begin_screen")
        {
            beginScreen();
        }
        else if (screen_name == "trans_screen")
        {
            transScreen();
            first_in_trial = true;
        }
        else if (screen_name == "trial_screen")
        {
            trialScreen();
        }
        else if (screen_name == "end_screen")
        {
            endScreen();
        }
        
        ImGui::End();

    }


/*
// beginning screen
Subject Number Prompt
Input Subject Number
Press submit button
    store the subject number
    create an excel file based on the subject number
*/
void beginScreen()
{
    // in case I can possibly make a new file this way!
    if(ImGui::Button("Subject Number", buttonSize))
    {
        ImGui::OpenPopup("subject_num"); // open a popup and name it for calling
        // This just needs its own space, no curlies for the if
    }  
    static char name[12]; // info holder 
    // take the subjects number
    if(ImGui::BeginPopup("subject_num")) // if clicked essentially
    {
        ImGui::Text("What is the subject's number: "); // precursor for me to understand
        ImGui::InputText("##edit", name, IM_ARRAYSIZE(name)); // size wanted           
        if (ImGui::Button("Close"))
        {
            ImGui::CloseCurrentPopup();
            // put things here for what should happen once closed or else it will run foreverrrr 
            
            // Declare value for saveSubject everywhere
            saveSubject = name;
            // Create a new file 
            file_name.open("../../Data/" + saveSubject + "_pilotingAmp.csv"); // saves the csv name for all parameters
            // First line of the code
            file_name << "Trial" << "," << "Chord" << "," << "Sus" << "," << "Amp" << "," << "IsSim" << "," << "IsMajor" << ","
                      << "Valence" << "," << "Arousal" << "," << "Time" << std::endl; // theoretically setting up headers

            // Go to next screen
            screen_name = "trans_screen";
        }
        ImGui::EndPopup();
    }
}

/*
// transition screen
update trial number
make sure the excel file is being created for the following trials
    create new name for new excel file based on subject number and trial number
write the first line of the excel file
tell the person to talk to me, or give them more information
*/
void transScreen()
{
    
    // Write message for person
    ImGui::Text("Trial number is your intermediate screen.");

    if (ImGui::Button("Begin Next Experiment")){
        // Go to next screen
        screen_name = "trial_screen";
        // likely where I will be determining the base cue, trial number makes sense to code here

        // update trial number to make sense to me
        trial_num++; // this will be used to determine what are the characteristics held steady
    }
}

/*
// trial 1
include information for timestamp
set up the parameters
    define the base cue parameters
randomize the amplitudes wanted into a vector
    math to determine trials: 10 minute session, 15 seconds to choose, 600 s / 15 s = 40 trials
    create vector of length 40 with 10 of each amplitude option
    randomize the vector
display
    display each of the SAMs with their numbers
    have a drop down for person to choose which OR have them have buttons underneath them (must hold value chose visibly)
        updates the valence and arousal values
press button
    if not at end of vector
        record the data
        reset the values of the valence and arousal display
        increase the trial iterator
    else if end of vector && not at max trial
        record the data
        go to transition screen
        reset the trial iterator
        increase the experiment number
    else // aka at max trial number
        record the data
        go to end of experiment screen
*/
void trialScreen()
{
    // Set up the paramaters
    // Define the base cue paramaters
    sus = 1;
    currentChord = chordNew.signal_list[currentChordNum];
    // for (size_t i = 0; i < 10; i++)
    // {
    //     for (size_t j = 0; j < 4; j++)
    //     {
    //         list.push_back(j);
    //     }    
    // }

    // internal trial tracker
    static int count = 0;
    // random number generator
    static auto rng = std::default_random_engine {};

    if (first_in_trial){
        // initial randomization
        std::shuffle(std::begin(list), std::end(list), rng);
        // counter for trial starts at 0 in beginning
        count = 0;
        // set first_in_trial to false so initial randomization can happen once
        first_in_trial = false;
    }
    
    if (count < experiment_num){
        if (!dontPlay){
            // Play the cue
            if(ImGui::Button("Play")){
                // determine which part of the list should be used
                int cue_num = count%4;
                // determine what is the amp
                amp = list[cue_num];
                // create the cue
                chordNew = Chord(currentChord, sus, amp, isSim);
                // determine the values for each channel
                channelSignals = chordNew.playValues();
                // play_trial(cue_num);
                s.play(leftTact, channelSignals[0]);
                s.play(botTact, channelSignals[1]); 
                s.play(rightTact, channelSignals[2]); 

                // reset the play time clock 
                play_clock.restart();
                // allow for the play time to be measured and pause to be enabled
                playTime = true;
            }
        }
        else {
            // Give option to provide input
            // Valence Drop Down
            const char* itemsVal[] = {" ", "-2", "-1","0", "1", "2"};
            const char* combo_labelVal = itemsVal[item_current_val];
            if(ImGui::BeginCombo("Valence", combo_labelVal)){
                for (int n = 0; n < IM_ARRAYSIZE(itemsVal); n++)
                {
                    const bool is_selected = (item_current_val == n);
                    if (ImGui::Selectable(itemsVal[n], is_selected))
                        item_current_val = n; // gives a value to the selection states
                    if (is_selected)
                        ImGui::SetItemDefaultFocus(); // focuses on item selected
                }

                // determine the valence value selected
                switch(item_current_val)
                {
                    case 1:
                        val = -2;
                        break;
                    case 2:
                        val = -1;
                        break;
                    case 3:
                        val = 0;
                        break;
                    case 4:
                        val = 1;
                        break;
                    case 5:
                        val = 2;
                        break;
                    default:
                        // throw an error?
                        val = 100; // this way I know this one does not count?
                        break;
                }

                ImGui::EndCombo();
            }
            // Arousal Drop Down
            const char* itemsArous[] = {" ", "-2", "-1","0", "1", "2"};
            const char* combo_labelArous = itemsArous[item_current_arous];
            if(ImGui::BeginCombo("Arousal", combo_labelArous)){
                for (int n = 0; n < IM_ARRAYSIZE(itemsArous); n++)
                {
                    const bool is_selected = (item_current_arous == n);
                    if (ImGui::Selectable(itemsArous[n], is_selected))
                        item_current_arous = n; // gives a value to the selection states
                    if (is_selected)
                        ImGui::SetItemDefaultFocus(); // focuses on item selected
                }

                // determine the valence value selected
                switch(item_current_arous)
                {
                    case 1:
                        arous = -2;
                        break;
                    case 2:
                        arous = -1;
                        break;
                    case 3:
                        arous = 0;
                        break;
                    case 4:
                        arous = 1;
                        break;
                    case 5:
                        arous = 2;
                        break;
                    default:
                        // throw an error?
                        arous = 100; // this way I know this one does not count?
                        break;
                }
                
                ImGui::EndCombo();
            }
            
            // Go to next cue
            if(ImGui::Button("Next")){
                // Record the answers
                // timestamp information**********
                timeRespond = trial_clock.get_elapsed_time().as_seconds(); // get response time
                // put in the excel sheet
                file_name << count << ","; // track trial
                file_name << currentChordNum << "," << sus << "," << amp << "," << isSim << "," << chordNew.getMajor() << ","; // gathers experimental paramaters
                file_name << val << "," << arous << "," << timeRespond << std::endl; // gathers experimental input

                // reset values for drop down list
                item_current_val = 0;
                item_current_arous = 0;

                // shuffle the amplitude list if needed
                int cue_num = count % 4;
                if (cue_num == 3){
                    std::shuffle(std::begin(list), std::end(list), rng);            
                }
                // increase the list number
                count++;
                dontPlay = false;
                std::cout << "count is " << count << std::endl; 
            }
        }
        // Dictate how long the signal plays
        if (playTime)
        {   
            // Let the user know that they should feel something
            ImGui::Text("The cue is currently playing.");
            int cue_num = count % 4;
            // if the signal time has passed, stop the signal on all channels
            if(play_clock.get_elapsed_time().as_seconds() > channelSignals[0].length()){ // if whole signal is played
                    s.stopAll();
                    playTime = false; // do not reopen this until Play is pressed again
                    trial_clock.restart(); // start recording the response time
                    // Don't allow the user to press play again
                    dontPlay = true;
                    std::cout << list[cue_num] << " ";
                }
        }
    }
    else // if trials are done
    {
        if(trial_num < final_trial_num) // if not final trial
        {
            screen_name = "trans_screen";
            file_name.close();
        }
        else // if final trial
        {
            screen_name = "end_screen";
            file_name.close();
        }
    }    
}

/*
// end of experiment screen
blank?
All done. Thank you for your participation
*/
void endScreen()
{
    ImGui::Text("Thank you for your participation!");
    ImGui::Text("Please let the experimenter know that you are finished.");
}

};

// actually open GUI
int main() {
    MyGui my_gui;
    my_gui.run();
    return 0;
} 

/*

Let's do a fake code

// beginning screen
Subject Number Prompt
Input Subject Number
Press submit button
    store the subject number
    create an excel file based on the subject number

// transition screen
update trial number
make sure the excel file is being created for the following trials
    create new name for new excel file based on subject number and trial number
write the first line of the excel file
tell the person to talk to me, or give them more information

// trial 1
include information for timestamp
set up the parameters
    define the base cue parameters
randomize the amplitudes wanted into a vector
    math to determine trials: 10 minute session, 15 seconds to choose, 600 s / 15 s = 40 trials
    create vector of length 40 with 10 of each amplitude option
    randomize the vector
display
    display each of the SAMs with their numbers
    have a drop down for person to choose which OR have them have buttons underneath them (must hold value chose visibly)
        updates the valence and arousal values
press button
    if not at end of vector
        record the data
        reset the values of the valence and arousal display
        increase the trial iterator
    else if end of vector && not at max trial
        record the data
        go to transition screen
        reset the trial iterator
        increase the experiment number
    else // aka at max trial number
        record the data
        go to end of experiment screen

// end of experiment screen
blank?
All done. Thank you for your participation

*/