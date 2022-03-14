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
ImVec2 buttonSizeBegin = ImVec2(800, 65);  // Size of buttons on begin & transition screen
ImVec2 buttonSizeTrial = ImVec2(400, 65); // Size of buttons on trial scean
ImVec2 buttonSizeSAMs = ImVec2(150, 150); // Size of SAMs buttons
int deviceNdx = 5;
// tactors of interest
int topTact = 4;
int botTact = 6;
int leftTact = 0;
int rightTact = 2;

// how to save to an excel document
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
    GLuint valSAMs[5]; // valence
    GLuint arousSAMs[5]; // arousal
    std::string iconValues[5] = {"neg2", "neg1", "0", "1", "2"};
    
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
    // to determine state of image selection
    int pressed = -1; // valence
    int pressed2 = -1; // arousal
    // For keeping track of trials in each block
    int trial_num = 0; // counter for overall trials
    // block 1
    int train_num1 = 40; // amount of trials in training session
    int corr_train_num1 = 40; // amount of trials in corrective training session
    int experiment_num1 = 40; // amount of trials in experiment
    // block2
    int train_num2 = 40; // amount of trials in training session
    int corr_train_num2 = 40; // amount of trials in corrective training session
    int experiment_num2 = 40; // amount of trials in experiment
    int val = 0, arous = 0; // initialize val&arous values
    int final_block_num = 6; // number of blocks total
    // For playing the signal
    Clock play_clock; // keeping track of time for non-blocking pauses
    bool playTime = false;   // for knowing how long to play cues
    // Set up timing within the trials itself
    Clock trial_clock;
    double timeRespond; // track how long people take to decide
    // The amplitudes in a vector
    std::vector<int> listAmp = {0, 1, 2, 3};
    // The sustains in a vector
    std::vector<int> listSus = {0, 1, 2};
    // The base parameters for my chords
    std::vector<int> chordList = {0, 1, 2, 3, 4, 5, 6, 7}; // for all chords
    std::vector<int> baseChordList; // to determine the chords list for the next screen
    // Vector for if play can be pressed
    bool dontPlay = false;
    bool first_in_trial = false;
    // for collecting data
    int item_current_val = 0;
    int item_current_arous = 0;
    int currentChordNum; // chord number to be played
    // for screens
    std::string screen_name = "begin_screen"; // start at the beginning screen
    int exp_num = 1; // start with major/minor identification

     virtual void update() override
    {
        ImGui::BeginFixed("", {50,50}, {(float)windowWidth-100, (float)windowHeight-100}, flags);
        
        if (screen_name == "begin_screen")
        {
            beginScreen(); // starting the experiment
        }
        else if (screen_name == "trans_screen1")
        {
            transScreen1();
            first_in_trial = true; // transition to the training session
        }
        else if (screen_name == "trial_screen1")
        {
            trialScreen1(); // training session
        }        
        else if (screen_name == "trans_screen2")
        {
            transScreen2(); // transition to the corrective training session
            first_in_trial = true;
        }
        else if (screen_name == "trial_screen2")
        {
            trialScreen2(); // corrective training session
        }        
        else if (screen_name == "trans_screen3")
        {
            transScreen3();
            first_in_trial = true; // transition to the testing session
        }
        else if (screen_name == "trial_screen3")
        {
            trialScreen3(); // testing session
        }
        else if (screen_name == "end_screen")
        {
            endScreen(); // end of experiment
        }
        
        ImGui::End();

    }   

void beginScreen()
{
    // give space to breathe
    ImGui::NewLine();
    ImGui::NewLine();
    ImGui::NewLine();
    ImGui::NewLine();

    // center the object
    ImGui::SameLine((float)((windowWidth-100)/2)-400);

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
            file_name.open("../../Data/" + saveSubject + "_experiment1.csv"); // saves the csv name for all parameters
            // First line of the code
            file_name << "Trial" << "," << "Chord" << "," << "Sus" << "," << "Amp" << "," << "IsMajor" << ","
                      << "Valence" << "," << "Arousal" << "," << "Response Time" << std::endl; // theoretically setting up headers

            // Go to next screen
            screen_name = "trans_screen";

            // Determine the parameters for base cue values********
            static auto rng1 = std::default_random_engine {}; // for major or minor
            std::shuffle(std::begin(chordList), std::end(chordList), rng1);
        }
        ImGui::EndPopup();
    }
    baseChordList = chordList; // 1 through 8 mixed up chord values
}

void transScreen1() // introduction to the entire session, then specifically training
{
    if (exp_num == 1) // maj/min identification
        {
        // Write message for person
        ImGui::Text("During the following experiment, you will receive a haptic cue and be asked");
        ImGui::Text("to rate it on two dimensions of emotion (valence and arousal). For reference,");
        ImGui::Text("valence measures pleasantness (from negative to positive), while arousal measures");
        ImGui::Text("bodily activation (low to high energy).");
        ImGui::Text(" ");
        ImGui::Text("When you are ready to begin the next trial, press the button below.");
        
        // give space to breathe
        ImGui::NewLine();
        ImGui::NewLine();
        ImGui::NewLine();
        ImGui::NewLine();

        // center the object
        ImGui::SameLine((float)((windowWidth-100)/2)-400);

        if (ImGui::Button("Begin Next Experiment",buttonSizeBegin)){
            // Go to next screen
            screen_name = "trial_screen1";
            // likely where I will be determining the base cue, trial number makes sense to code here
            sus = susChosen;
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
    if(exp_num == 2) // amp/sus identification AND emotion
    {
        // Write message for person
        ImGui::Text("During the following experiment, you will receive a haptic cue and be asked");
        ImGui::Text("to rate it on two dimensions of emotion (valence and arousal). For reference,");
        ImGui::Text("valence measures pleasantness (from negative to positive), while arousal measures");
        ImGui::Text("bodily activation (low to high energy).");
        ImGui::Text(" ");
        ImGui::Text("When you are ready to begin the next trial, press the button below.");
        
        // give space to breathe
        ImGui::NewLine();
        ImGui::NewLine();
        ImGui::NewLine();
        ImGui::NewLine();

        // center the object
        ImGui::SameLine((float)((windowWidth-100)/2)-400);

        if (ImGui::Button("Begin Next Experiment",buttonSizeBegin)){
            // Go to next screen
            screen_name = "trial_screen2";
            // likely where I will be determining the base cue, trial number makes sense to code here
            sus = susChosen;
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
}

};

int main(){
    MyGui my_gui;
    my_gui.run();
    return 0;
}