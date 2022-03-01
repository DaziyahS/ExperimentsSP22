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

void trialScreen1();
{
 // Set up the paramaters
    // Define the base cue paramaters
    currentChord = chordNew.signal_list[currentChordNum];

    // internal trial tracker
    static int count = 0;
    // random number generator
    static auto rng = std::default_random_engine {};

    if (first_in_trial){
        list = chordList;
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
                            ImGui::PushStyleColor(ImGuiCol_Button,ImVec4(1.0f, 1.0f, 0.0f, 1.0f));
                        }
                        else
                            ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(1 / 7.0f, 0.3f, 0.3f));
                    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(2 / 7.0f, 0.6f, 0.6f));
                    ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(5 / 7.0f, 0.9f, 0.9f));
                    if(ImGui::ImageButton((void *)(intptr_t)valSAMs[i],buttonSizeSAMs, ImVec2(0,0), ImVec2(1,1), 5))
                    {
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
                    else
                    {
                        ImGui::Text("Arousal");
                    }
                    ImGui::PushID(i);
                        if (pressed2 == i){
                            ImGui::PushStyleColor(ImGuiCol_Button,ImVec4(1.0f, 1.0f, 0.0f, 1.0f));
                        }
                        else
                            ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(1 / 7.0f, 0.3f, 0.3f));
                    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(2 / 7.0f, 0.6f, 0.6f));
                    ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(5 / 7.0f, 0.9f, 0.9f));
                    if(ImGui::ImageButton((void *)(intptr_t)arousSAMs[i-5],buttonSizeSAMs, ImVec2(0,0), ImVec2(1,1), 5))
                    {
                        pressed2 = i;
                        arous = pressed2 - 7;
                    };  
                }
                ImGui::PopStyleColor(3);
                ImGui::PopID();
            }
            
            ImGui::NewLine();
            ImGui::NewLine();
            ImGui::SameLine(205);
            // Go to next cue
            if(ImGui::Button("Next",buttonSizeTrial)){
                // Record the answers
                if (pressed > 0 && pressed2 > 0)
                {
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
                    
                    if(count < experiment_num) // if not final trial
                    {
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
                }
        }
    }
    else // if trials are done
    {
            cout << "done" << endl;
    }        
}