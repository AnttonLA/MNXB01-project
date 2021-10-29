#include <iostream>
#include<fstream>
#include <sstream>
#include<TH1F.h>
#include<TLegend.h>
#include <TCanvas.h> // canvas object
#include "tempTrender.h"

using namespace std;

//Utils
bool hasEnding (std::string const &fullString, std::string const &ending) {
  //compares two strings to check if the first string ends with the second string
  //e.g. hasEnding("binary", "nary") returns True
  //Function taken from https://stackoverflow.com/questions/874134/find-out-if-string-ends-with-another-string-in-c
  //maybe should go in its own file and then get included here ?
	if (fullString.length() >= ending.length()) {
	    return (0 == fullString.compare (fullString.length() - ending.length(), ending.length(), ending));
	} else {
	    return false;
	}
}

//Member Functions
tempTrender::tempTrender(const string& filePath) : _path{filePath}{ //filePath has now been stored in member variable _path
  cout << "The user supplied " << filePath <<" as the path to the data file.\n";
  //std::cout << "You should probably store this information in a member variable of the class! Good luck with the project! :)\n";

}

void tempTrender::tempOnDay(int monthToCalculate, int dayToCalculate) const { //Make a histogram of the temperature on this day

  //Incorrect input handling
  if (monthToCalculate>12 || dayToCalculate>31) {
    cout << "Invalid date requested. Please input a valid date with the format tempOnDay(Month, Day)" << endl;
    return;
  }

  //Convert inputed ints to strings, and combine them in "02-14" format
  string month_prefix = "";
  if (monthToCalculate < 10) { 
    month_prefix = "0";
  }
  string day_prefix = "";
  if (dayToCalculate < 10) {
    day_prefix = "0";
  }
  string dateToCalculate_sting = month_prefix+to_string(monthToCalculate)+"-"+day_prefix+to_string(dayToCalculate);
  cout<<"The requested date was " << dateToCalculate_sting << endl;

  //Code to open the csv files
  fstream fin; //File pointer
  fin.open(_path, ios::in); //Open file at '_path'
  if (fin.fail()) { //Check that file can be opened.
    cout << "File could not be opened. Please check that the provided path is correct." << endl;
	return;
  }

  //TODO: I can't figure out how to do the damn degree symbol in the xlabel :/
  // CREATE HISTOGRAM that we will fill with data from specified date
	//(name, title;xlabel;ylabel, bins, xmin, xmax)
  TH1D* hDayTemp = new TH1D("one_day_tempt", "Temperature for date;Temperature [C]; Entries", 100, -20, 40); 

  //Iterate through file, line by line, checking if the date matches with input
  vector<string> row;
  string line, cell, date_string;
  int temp;
  int i = 0;
  while (getline(fin, line)){ //read whole file, row by row, store line in variable 'line' each loop
    i++;
    row.clear();

    stringstream lineStream(line); //Slice line by ; and store each part in vector 'row'
	while (lineStream.good() && getline(lineStream, cell, ';')) {
        row.push_back(cell);
    }
	date_string = row[0]; //save the date of the line
    
    //If the date matches with requested date
    if (hasEnding(date_string, dateToCalculate_sting)) {
      temp = stoi(row[2]);
      //cout << "line " << i << ", at date " << date_string << " the temperature was " << temp << endl;
      hDayTemp->Fill(temp);
    }

  }

  //Histogram drawing
  TCanvas* c1 = new TCanvas("c1", "histogram canvas", 900, 600);
  hDayTemp->SetFillColorAlpha(kRed, 0.35);
  hDayTemp->Draw();

  //Legend
  //TODO: make legend nicer
  auto legend = new TLegend();
  legend->AddEntry(hDayTemp,("Temperature on "+dateToCalculate_sting).c_str(),"f");
  legend->Draw();

  //Answering questions
  Double_t mean_val = hDayTemp->GetMean(1);
  Double_t stdev = hDayTemp->GetRMS(1);
  cout << "The mean value for date " << dateToCalculate_sting << " is: " << mean_val << "C" << endl;
  cout << "The standard deviation is: " << stdev << endl;

  //TODO: Fit gaussian to histogram to calulcate probabilities?
}
// void tempTrender::tempOnDay(int dateToCalculate) const {} //Make a histogram of the temperature on this date
// void tempTrender::tempPerDay() const {} //Make a histogram of the average temperature of each day of the year
// void tempTrender::hotCold() const {} //Make a histogram of the hottest and coldest day of the year
// void tempTrender::tempPerYear(int yearToExtrapolate) const {} //Make a histogram of average temperature per year, then fit and extrapolate to the given year
