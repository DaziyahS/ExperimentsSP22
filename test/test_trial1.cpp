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
    GLuint susGraphs[3]; // sustain graphs
    GLuint ampGraphs[4]; // amplitude graphs
    GLuint ASFaces[4]; // affective slider faces
    std::string iconValues[4] = {"1", "2", "3", "4"};
    
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
        for (int i = 0; i < 3; i++)
        {
            loadIcon(("../../Figures/time" + iconValues[i] + ".png").c_str(), &susGraphs[i]);
        }
        for (int i = 0; i < 4; i++){
            loadIcon(("../../Figures/val_" + iconValues[i] + ".png").c_str(), &ampGraphs[i]);
        }
        loadIcon("../../Figures/AS_sleepy_alt.png", &ASFaces[0]);
        loadIcon("../../Figures/AS_wideawake.png", &ASFaces[1]);
        loadIcon("../../Figures/AS_unhappy.png", &ASFaces[2]);
        loadIcon("../../Figures/AS_happy.png", &ASFaces[3]);
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
    int majPress = -1; // major/minor buttons
    int ampPress = -1; // amplitude buttons
    int durPress = -1; // duration buttons
    int pressed = -1; // valence
    int pressed2 = -1; // arousal
    // block 1
    int train_num1 = 40; // amount of trials in training session
    int corr_train_num1 = 40; // amount of trials in corrective training session
    int experiment_num1 = 40; // amount of trials in experiment
    // block2
    int train_num2 = 40; // amount of trials in training session
    int corr_train_num2 = 40; // amount of trials in corrective training session
    int experiment_num2 = 40; // amount of trials in experiment
    int val = 0, arous = 0; // initialize val&arous values
    int maj = -1; // major value, 0 is major 1 is minor 
    int dur = 0; // duration value chosen
    int ampl = 0; // amplitude value chosen
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
    std::vector<int> baseChordList = chordList; // to determine the chords list for the next screen
    // Vector for if play can be pressed
    bool dontPlay = false;
    bool first_in_trial = true;
    // for collecting data
    int item_current_val = 0;
    int item_current_arous = 0;
    int currentChordNum = 0; // chord number to be played
    // for screens
    std::string screen_name = "begin_screen"; // start at the beginning screen
    int exp_num = 1; // start with major/minor identification

     virtual void update() override
    {
        ImGui::BeginFixed("", {50,50}, {(float)windowWidth-100, (float)windowHeight-100}, flags);
            trialScreen1();
        ImGui::End();

    }   

void trialScreen1(){
 // Set up the paramaters
    // Define the base cue paramaters
    amp = 0; // full amplitude
    sus = 1; // mid sustain

    // internal trial tracker
    static int count = 0;
    // random number generator
    static auto rng = std::default_random_engine {};

    if (first_in_trial){
        baseChordList = chordList;
        // initial randomization
        std::shuffle(std::begin(baseChordList), std::end(baseChordList), rng);
        // counter for trial starts at 0 in beginning
        count = 0;
        // set first_in_trial to false so initial randomization can happen once
        first_in_trial = false;
    }
    
    if (count < experiment_num1){
        if (!dontPlay){
            // do nothing
        }
        else {
            ImGui::Text("Valence");
            
            if(ImGui::Button("Major")){
                majPress = 1;
                maj = 0;
            }
            if(ImGui::Button("Minor")){
                majPress = 2;
                maj = 1;
            }
            
            if(ImGui::Button("Play",buttonSizeTrial)){
                // Record the answers
                if (majPress > 0)
                {
                    // timestamp information**********
                    timeRespond = trial_clock.get_elapsed_time().as_seconds(); // get response time
                    // // put in the excel sheet
                    // file_name << count << ","; // track trial
                    // file_name << currentChordNum << "," << sus << "," << amp << "," << chordNew.getMajor() << ","; // gathers experimental paramaters
                    // file_name << maj << "," << timeRespond << std::endl; // gathers experimental input

                    // reset values for drop down list
                    majPress = -1;

                    // shuffle the chord list if needed
                    int cue_num = count % 8;
                    if (cue_num == 7){
                        std::shuffle(std::begin(baseChordList), std::end(baseChordList), rng);            
                    }
                    // increase the list number
                    count++;
                    dontPlay = false;
                    
                    if(count < experiment_num1) // if not final trial
                    {
                        // Play the next cue for listening purposes
                        // determine which part of the list should be used
                        cue_num = count%8;
                        // determine what is the amp
                        currentChord = baseChordList[cue_num];
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
                else
                {
                    ImGui::OpenPopup("Error");
                }
            }
            if(ImGui::BeginPopup("Error")){
                ImGui::Text("Please make both a valence and arousal selection before continuing.");
                if(ImGui::Button("Close"))
                {
                    ImGui::CloseCurrentPopup();
                }
            }
        }
            if(playTime){   
                // Let the user know that they should feel something
                ImGui::Text("The cue is currently playing.");
                int cue_num = count % 8;
                // if the signal time has passed, stop the signal on all channels
                if(play_clock.get_elapsed_time().as_seconds() > channelSignals[0].length()){ // if whole signal is played
                        s.stopAll();
                        playTime = false; // do not reopen this until Play is pressed again
                        trial_clock.restart(); // start recording the response time
                        // Don't allow the user to press play again
                        dontPlay = true;
                    }
            }
            // play cue
            // say cue is playing
            // highlight major or minor
        }
    }
    else // if trials are done
    {
        screen_name = "end_screen";
        file_name.close();
    }      
}  