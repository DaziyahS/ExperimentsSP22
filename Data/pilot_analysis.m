%% Analyzing Data: Pilots
% Daziyah Sullivan
% November 16, 2021

%% Pull in the Data

% Read the data files
pilot1 = xlsread('PILOT1_pilotingAmp.csv');
pilot2 = xlsread('PILOT2_pilotingAmp.csv');

% Separate the wanted values
chordValues = [pilot1(:,2); pilot1(:,2)];
susValues = [pilot1(:,3); pilot1(:,3)];
ampValues = [pilot1(:,4); pilot1(:,4)];
majorValues = [pilot1(:,6); pilot1(:,6)];
valValues = [pilot1(:,7); pilot1(:,7)];
arousValues = [pilot1(:,8); pilot1(:,8)];

%% Analyze the Data

X = [chordValues susValues ampValues]; % variables that are changing
valGROUP = valValues; % variable of interest
arousGROUP = arousValues; % variable of interest
[valD, valP, valSTATS] = manova1(X, valGROUP); 
[arousD, arousP, arousSTATS] = manova1(X, arousGROUP); 

%% Determine the Means

% Find the means for each data set
mean1 = mean(pilot1);
mean2 = mean(pilot2);

% Options
majorN = 2;
susN = 3;
ampN = 4;

% Find the valence and arousal means for the 24 options
% Breakdown the 24 options, store the valence and arousal values into
% separate variables
% Define the valence and arousal means for each option
% Add the means to a matrix for manova

