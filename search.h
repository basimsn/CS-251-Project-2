//  Project 2 - Search
//  Course: CS 251
//  Author: Basim Nabulsi
//  This file allows for a database file to search for key words and output the urls that match the in put. You can also input many words with + and - as well to change what urls are outputted.

#pragma once

#include <iostream>
#include <fstream>
#include <set>
#include <vector>
#include <map>
#include <string>
using namespace std;

string cleanToken(string s) {
    int size = s.size()-1; //Changes all uppercase to lowercase and removes any punctuation
    int counter=0;
    int front =0;
    for(int i=0; i<=size; i++){   //I started off by make all letters lowercase and I set a counter to see if there were any letters
        if(isalpha(s[i])){
            counter++;
            s[i] = tolower(s[i]);
        }
    }
    if(counter == 0){       //If there were no letters I erased the whole string and returned it empty
        s.erase(0, size+1);
    }
    else{       
        for(int i=size; i>=0; i--){ //This for loop would start from the end of the string and pop until ispunct is false
            if(!ispunct(s[i])){
                break;
            }
            else{
                s.pop_back();
            }
        }
        for(int i=0; i<=size; i++){ //This loop started from the beginning and count how many punct was in the front
            if(!ispunct(s[i])){
                break;
            }
            else{
                front++;
            }
        }
        s.erase(0, front);  //This wolud erase all the punct in front of the string
    }
    return s;
}

set<string> gatherTokens(string text) {
    set<string> tokens; //Takes in a string, cleans it and removes any repeats
    string word;
    for (auto x : text){  //Every word was separated by a space, cleaned, and inserted in a set that was then returned
        if (x == ' '){
            if(cleanToken(word) !=""){
                tokens.insert(cleanToken(word));
                word = "";
            }
            else {
                word = word + x;
                word = "";
            }
        }
        else {
            word = word + x;
        }
    }
    if(cleanToken(word) != ""){
        tokens.insert(cleanToken(word));
    }
    return tokens;
}

int buildIndex(string fileName, map<string, set<string>> &index, set<string> stop) {
    vector<string> websites; //This function adds to the map for it to be then compared and for the values to be printed
    vector<string> tokenWords; //For my creative component I added set string stop as a parameter to test it without the parameter -->
    set<string> tokens; //delete the parameter and comment out line 96 and 98 and erase the stop from call function on line 234. If the parameter-->
    int websiteNum=0; //does not matter just comment out line 233 to see the results.
    int uniqueTerms=0;
    ifstream inFS;
    string fileWords;
    inFS.open(fileName);

    while(!inFS.eof()){ //Reads into the file into tokenWords
        getline(inFS, fileWords);
        tokenWords.push_back(fileWords);
    }
    for(int i=0; i<tokenWords.size(); i+=2){ 
        if(tokenWords[i]!=""){  //Pushes websites into a vector
            websites.push_back(tokenWords[i]);
        }
        for(auto &j: gatherTokens(tokenWords[i+1])){ //Puts all the key words in a set
            tokens.insert(j);
        }
    }
    for(int i=0; i<tokenWords.size(); i+=2){
        for(auto &it : gatherTokens(tokenWords[i+1])){ //Loops through the set that is returned
            if(stop.count(it) != 1){  //Prevents any stop words
                index[it].insert(websites[websiteNum]); //Inserts all the urls for each key word
            }   
        }
        websiteNum++; //Moves on to the next website
    }
    
    cout<<"Stand by while building index...\nIndexed "<<websites.size()<<" pages containing "<<tokens.size()<<" unique terms\n";

    
    return websites.size(); //Returns the amount of urls
}

void separate(string &sentence, vector<string> & keyWords, vector<char> &commands){
    string temp; //Separates the input for it to be processed
    for(int i=0; i<sentence.size(); i++){   //Pushes back the commands to know which function the to use
        if(sentence[i] == ' ' && (sentence[i+1] != '+' && sentence[i+1] != '-' )){
            commands.push_back('|'); //or
        }
        else if(sentence[i] == '+'){
            commands.push_back('+'); //and
        }
        else if(sentence[i] == '-'){
            commands.push_back('-'); //minus
        }
    }
    string word = "";
    for (auto x : sentence){ //Separates the commands from the word
        if (x == ' ' || x=='+' || x== '-'){
            if(word != ""){
                keyWords.push_back(word);
                word = "";
            }
        }
        else {
            word = word + x;
        }
    }
    if(word != ""){
        keyWords.push_back(word);
    }
    if(keyWords.size() ==1){  //Cleans the input if theres only one word
        temp = cleanToken(keyWords[0]);
        keyWords.clear();
        keyWords.push_back(temp);
        commands.clear();
    }
}

void plusCom(map<string, set<string>> index, string keyWords, set<string> &result){
    vector<string>repeat; //Intersection Function
    set<string> similarities;
    for(auto &i : result){ //temp string
        similarities.insert(i);
    }
    result.clear(); //cleared
    for(auto &i : index[keyWords]){ 
        repeat.push_back(i);
    }
    for(int i=0; i<repeat.size(); i++){ //If there are similarities insert it in the results set to have the correct union
        if(similarities.count(repeat[i]) == 1){
            result.insert(repeat[i]);
        }
    }
    
}

void minusCom(map<string, set<string>> index, string keyWords, set<string> &result){
    for(auto &i : index[keyWords]){ //Subtraction Function
        if(result.count(i) == 1){ //Loops through and if found erases the url
            result.erase(i);
        }
    }
}

void orCom(map<string, set<string>> index, string keyWords, set<string> &result){
    for(auto &i : index[keyWords]){ //Or function
        if(result.count(i) == 0){   //Loops through and if not found added
            result.insert(i);
        }
    }
}

set<string> findQueryMatches(map<string, set<string>>& index, string &sentence) {
    set<string> result; //Function uses the input to find all the urls that match the inputs
    vector<string> keyWords;
    vector<char> commands;
    getline(cin, sentence); //Gets the command
    separate(sentence, keyWords, commands); //Separates 
    for(auto &i : index[keyWords[0]]){ //Starts off by looping through the all the urls for the first word
        result.insert(i);
    }

    for(int i =0; i<commands.size(); i++){  //Loops through all the commands and it determines which function the set goes through
        if(commands[i] == '+'){
            plusCom(index, keyWords[i+1], result); //keyWords[i+1] because we already added keyWords[0]
        }
        else if(commands[i] == '-'){ 
            minusCom(index, keyWords[i+1], result);
        }
        else{ 
            orCom(index, keyWords[i+1], result);
        }
    }

    cout<<"Found "<<result.size()<<" matching pages"<<endl;
    for(auto &i : result){ //Prints out all the urls
        cout<<i<<endl;
    }

    return result; 
}

void stopWords(set<string> &stop){ //Creative component
    ifstream inFS; //Removes any stop words from being able to be searched. This is helpful because when searching for words such as the.
    string fileWords; 
    inFS.open("stopwords.txt"); // Txt file from minimal-stop.txt without the #determiners, #coordinating conjuctions, and #prepositions
    inFS>>fileWords;
    while(!inFS.eof()){ //Inserts all the stop words in a set
        stop.insert(fileWords);
        inFS>>fileWords;
    }
    inFS.close();
}

void searchEngine(string fileName) {
    map<string, set<string>> index; //This function acts as the main and directs the project to be able to search for the urls based on the input
    vector<string> websites; //Initializing variables, sets, vectors, and the map index
    vector<string> tokenWords;
    set<string>stop;
    int websiteNum =0;
    int i=0;
    int uniqueTerms =0;
    string sentence = "";
    stopWords(stop); // comment this out to the results without the stopWords function
    buildIndex(fileName, index, stop); 

    while(sentence !="\n"){ //Allows for inputs until enter is pressed
        cout<<"Enter query sentence (press enter to quit): ";
        findQueryMatches(index, sentence);
    }
}