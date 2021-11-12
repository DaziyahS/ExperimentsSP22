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
#include <Note.hpp> 
#include <stb_image.h>

// open the namespaces that are relevant for this code
using namespace mahi::gui;
using namespace mahi::util;
using tact::Signal;
using tact::sleep;
using tact::Sequence;

// deteremine application variables
int windowWidth = 1920; // 1920 x 1080 is screen dimensions
int windowHeight = 1080;
std::string my_title= "Play GUI";
ImVec2 buttonSizeBegin = ImVec2(400, 65);  // Size of buttons on begin & transition screen
ImVec2 buttonSizeTrial = ImVec2(400, 65); // Size of buttons on trial scean
ImVec2 buttonSizeSAMs = ImVec2(100, 100); // Size of SAMs buttons
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
private:
 // Loading in of images
        bool loadTextureFromFile(
            const char *filename, GLuint *out_texture, int *out_width, int *out_height)
            {
                // Load from file
                int image_width = 0;
                int image_height = 0;
                unsigned char *image_data = stbi_load(filename, &image_width, &image_height, NULL, 4);
                if (image_data == NULL)
                    return false;

                // Create a OpenGL texture identifier
                GLuint image_texture;
                glGenTextures(1, &image_texture);
                glBindTexture(GL_TEXTURE_2D, image_texture);

                // Setup filtering parameters for display
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // This is required on WebGL for non power-of-two textures
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); // Same

                // Upload pixels into texture
                    #if defined(GL_UNPACK_ROW_LENGTH) && !defined(__EMSCRIPTEN__)
                            glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
                    #endif
                            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image_width, image_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
                            stbi_image_free(image_data);

                            *out_texture = image_texture;
                            *out_width = image_width;
                            *out_height = image_height;

                            return true;
            }
    // simple wrapper to simplify importing images
        bool loadIcon(const char *imgPath, GLuint *my_image_texture)
            {
                int my_image_width = 0;
                int my_image_height = 0;
                bool ret = loadTextureFromFile(imgPath, my_image_texture, &my_image_width, &my_image_height);
                IM_ASSERT(ret);
                return true;
            }
    // Define the variables for the SAMs
    // Valence
    GLuint valSAMs[5];
    GLuint arousSAMs[5];
    std::string iconValues[5] = {"neg2", "neg1", "0", "1", "2"};
    // Arousal

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
        flags = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse;

        // so the current chord can play immediately
        currentChord = chordNew.signal_list[14];

        // create the icons
        for (int i = 0; i < 5; i++)
        {
            loadIcon(("../../Figures/arous_" + iconValues[i] + ".png").c_str(), &arousSAMs[i]);
            loadIcon(("../../Figures/val_" + iconValues[i] + ".png").c_str(), &valSAMs[i]);
        }
     }

    // Define variables needed throughout the program
    ImGuiWindowFlags flags;
    // For creating the signal
    std::string currentChord; // holds name of current chord based on selection
    Chord chordNew;
    std::vector<tact::Signal> channelSignals;
    bool isSim = false; // default is sequential
    int amp, sus;
    int pressed = -1;
    int pressed2 = -1;
    // For saving the signal
    std::string sigName; // name for saved signal
    std::string fileLocal; // for storing the signal
    // For saving records
    int trial_num = 0; // counter for overall trials
    int experiment_num = 40; // amount of trials in experiment
    int val = 0, arous = 0;
    int final_trial_num = 6;
    // For playing the signal
    Clock play_clock; // keeping track of time for non-blocking pauses
    bool playTime = false;   // for knowing how long to play cues
    // Set up timing within the trials itself
    Clock trial_clock;
    double timeRespond;
    // The amplitudes in a vector
    std::vector<int> list = {0, 1, 2, 3};
    // The base parameters for my chords
    std::vector<int> chordList = {14, 15}; // for D Note
    std::vector<int> susList = {0, 1, 2};
    std::vector<int> baseChordList;
    std::vector<int> baseSusList;
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
        ImGui::Begin("", 0, flags);
        
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
    if(ImGui::Button("Subject Number", buttonSizeBegin))
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

            // Determine the parameters for base cue values********
            static auto rng1 = std::default_random_engine {}; // for major or minor
            std::shuffle(std::begin(chordList), std::end(chordList), rng1); 
            baseChordList = {chordList[0], chordList[0], chordList[0], chordList[1], chordList[1], chordList[1]};
            static auto rng2 = std::default_random_engine {};
            std::shuffle(std::begin(susList), std::end(susList), rng2); 
            baseSusList = {susList[0], susList[1], susList[2], susList[1], susList[2], susList[0]}; // psuedo randomization

        }
        ImGui::EndPopup();
    }
}

void transScreen()
{
    
    // Write message for person
    ImGui::Text("During the following experiment, you will receive a haptic cue and be asked");
    ImGui::Text("to rate it on two dimensions of emotion (valence and arousal). For reference,");
    ImGui::Text("valence measures pleasantness (from negative to positive), while arousal measures");
    ImGui::Text("bodily activation (low to high energy).");
    ImGui::Text(" ");
    ImGui::Text("When you are ready to begin the next trial, press the button below.");

    if (ImGui::Button("Begin Next Experiment")){
        // Go to next screen
        screen_name = "trial_screen";
        // likely where I will be determining the base cue, trial number makes sense to code here
        sus = baseSusList[trial_num];
        currentChordNum = baseChordList[trial_num];

        // update trial number to make sense to me
        trial_num++; // this will be used to determine what are the characteristics held steady

        // Start playing cue as enter the trial
        int cue_num = 0;
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

void trialScreen()
{
    // Set up the paramaters
    // Define the base cue paramaters
    currentChord = chordNew.signal_list[currentChordNum];

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
            
        }
        else {
            ImGui::Text("Valence");
            
            for (int i = 0; i < 10; i++)
            {
                if (i < 5)
                {    
                    if (i > 0)
                    {
                        ImGui::SameLine();
                    }
                    ImGui::PushID(i);
                        if (pressed == i){
                            ImGui::PushStyleColor(ImGuiCol_Button,(ImVec4)ImColor::HSV(5 / 7.0f, 0.8f, 0.8f));
                        }
                        else
                            ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(5 / 7.0f, 0.3f, 0.3f));
                    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(5 / 7.0f, 0.6f, 0.6f));
                    ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(5 / 7.0f, 0.9f, 0.9f));
                    if(ImGui::ImageButton((void *)(intptr_t)valSAMs[i],buttonSizeSAMs))
                    {
                        std::cout << i << std::endl;
                        pressed = i;
                        val = pressed - 2;
                    };
                }
                else
                {
                    if (i > 5)
                    {
                        ImGui::SameLine();
                    }
                    ImGui::PushID(i);
                        if (pressed2 == i){
                            ImGui::PushStyleColor(ImGuiCol_Button,(ImVec4)ImColor::HSV(5 / 7.0f, 0.8f, 0.8f));
                        }
                        else
                            ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(5 / 7.0f, 0.3f, 0.3f));
                    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(5 / 7.0f, 0.6f, 0.6f));
                    ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(5 / 7.0f, 0.9f, 0.9f));
                    if(ImGui::ImageButton((void *)(intptr_t)arousSAMs[i-5],buttonSizeSAMs))
                    {
                        std::cout << i << std::endl;
                        pressed2 = i;
                        arous = pressed2 - 7;
                    };  
                }
                ImGui::PopStyleColor(3);
                ImGui::PopID();
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
                pressed = -1;
                pressed2 = -1;

                // shuffle the amplitude list if needed
                int cue_num = count % 4;
                if (cue_num == 3){
                    std::shuffle(std::begin(list), std::end(list), rng);            
                }
                // increase the list number
                count++;
                dontPlay = false;
                std::cout << "count is " << count << std::endl; 

                // Play the next cue for listening purposes
                // determine which part of the list should be used
                cue_num = count%4;
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
        }
        else // if final trial
        {
            screen_name = "end_screen";
            file_name.close();
        }
    }    
}

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
