/*-----------------------------------------------------------------------------
* File: VWAPcalc.cpp
*     Get information from the orders flow from the exchange. Generate price
*     and volume series for a specific ticker (SPY here).
* Purpose: Course project 1 for ORIE 5255: Real-time Risk
*     Directed by Irene Aldridge, Zhexiao Zhang
* Copyright (c) 2017 Yuheng Zhou (yz978), Yuan Gao (yg396)
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
-----------------------------------------------------------------------------*/

#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <vector>
using namespace std;

int main() {
	int dateCount = 2;
	ifstream inputfile; 
	ofstream outputfile;
	inputfile.open("802.csv"); // the csv file that contains all the minute price

	// store VWAP value in the following two vectors
	vector<string> minute;
	vector<double> VWAP;
	string line;

	// read through every minute of the csv file
	while (getline(inputfile, line)) {
		vector<string> parsedLine(1);
		for (auto c : line) {
			if (c != ',') parsedLine[parsedLine.size() - 1] += c;
			else parsedLine.push_back("");
		}

		// record current minute
		minute.push_back(parsedLine[0]);

		// calculate VWAP for the minute
		double VWAPcur = 0;
		double volumeCur = 0;
		for (int i = 1; i <= 2*dateCount - 1; i=i+2) {
			VWAPcur += stod(parsedLine[i])*stod(parsedLine[i + 1]);
			volumeCur += stod(parsedLine[i + 1]);
		}
		if (volumeCur != 0) VWAP.push_back(VWAPcur/volumeCur);
		else {} // ASK Irene!
	}
	
	for (int i = 0; i <= minute.size() - 1; i++) cout << minute[i] << ' ' << VWAP[i] << endl;
	system("pause");
	return 0;
}
