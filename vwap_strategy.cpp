/*-----------------------------------------------------------------------------
* File: ProcOrderBook.cpp
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
#include <sstream>
using namespace std;

vector<string> split(const string& s, char delimiter)
{
   vector<string> tokens;
   string token;
   istringstream tokenStream(s);
   while (getline(tokenStream, token, delimiter))
   {
      tokens.push_back(token);
   }
   return tokens;
}

int main() {
	const int knodays = 43;
	string file_list[44] = {"0801","0802","0803","0804","0807","0808","0809","0810",
							"0811","0814","0815","0816","0817","0818","0821","0822",
							"0823","0824","0825","0828","0829","0830","0831","0901",
							"0905","0906","0907","0908","0911","0912","0913","0914",
							"0915","0918","0919","0920","0921","0922","0925","0926",
							"0927","0928","0929"};
    ifstream inputfile;
	ofstream outputfile;
	string line;
	vector<string> elements;
	vector<double> dayvol, avg_vol;
	bool firstday = true;
	int vol, cnt, sum;
	for (int ifile = 0; ifile < knodays; ifile++) {
		inputfile.open("volume_curves/" + file_list[ifile] + "_res.csv");
		if (file_list[ifile] < "0911") {
			cnt = 0;
			while (getline(inputfile, line)) {
				elements = split(line, ',');
				vol = stof(elements[2]);
				if (firstday) 
					avg_vol.push_back(vol);
				else
					avg_vol[cnt] += vol;
				cnt++;
			}
		}
		inputfile.close();
		firstday = false;
	}

	sum = 0;
	for (int i = 0; i < avg_vol.size(); i++)
		sum += avg_vol[i];
	outputfile.open("volume_dist.csv");
	for (int i = 0; i < avg_vol.size(); i++) {
		avg_vol[i] /= sum;
		outputfile << avg_vol[i] << endl;
	}
	outputfile.close();

	/*sum = 0;
	for (int i = 0; i < avg_vol.size(); i++)
		sum += avg_vol[i];
	cout << sum << endl;*/

	outputfile.open("day_volume.csv");
	double tot_amount = 1e6;
	double price, acc_ratio, acc_amount, real_amount, tot_cost;
	double cum_vol, bench_cost;
	int this_amount;
	vector<vector<double> > hist_vol;
	for (int ifile = 0; ifile < knodays; ifile++) {
		inputfile.open("volume_curves/" + file_list[ifile] + "_res.csv");
		if (file_list[ifile] >= "0911") {
			acc_amount = 0;
			real_amount = 0;
			acc_ratio = 0.0;
			cnt = 0;
			cum_vol = 0;
			tot_cost = 0;
			bench_cost = 0;
			dayvol.clear();
			while (getline(inputfile, line)) {
				elements = split(line, ',');
				price = stof(elements[1]);
				acc_ratio += avg_vol[cnt];
				acc_amount = acc_ratio * tot_amount;
				this_amount = (int)(acc_amount - real_amount);
				real_amount += this_amount;
				tot_cost += this_amount * price;
				//tot_cost += avg_vol[cnt] * tot_amount * price;
				bench_cost += price;
				cum_vol += this_amount;
				dayvol.push_back(this_amount);
				cnt++;
			}
			hist_vol.push_back(dayvol);
			cout << tot_cost / 1e6 << endl;
		}
		inputfile.close();
		firstday = false;
	}
	for (int i = 0; i < hist_vol[0].size(); i++){
		for (int j = 0; j < hist_vol.size(); j++){
			dayvol = hist_vol[j];
			outputfile << dayvol[i] << ",";
		}
		outputfile << endl;
	}
	return 0;
}