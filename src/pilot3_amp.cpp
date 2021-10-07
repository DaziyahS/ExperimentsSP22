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