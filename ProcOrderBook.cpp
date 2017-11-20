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
using namespace std;

// Contain the details of an order, except orderID
struct Order_cont {
    int volume;
    int price;
    bool isbuy;
};

// Contain the results for every minutes
struct Result
{
    int hour, minute;
    int price, volume;
};


int getMidOrder(map<string, struct Order_cont>& orderbook) {
// Function: getMidOrder
//     Given a minute has zero volume, calculate the mid price between highest
//     bid and lowest ask.
// Input:
//     - orderbook: the hash table of order book
// Output:
//     The mid price

    int maxbuy = 0, minsell = 100000000;
    if (orderbook.size() == 0)
        return 0;
    for (map<string, struct Order_cont>::iterator it=orderbook.begin();
        it!=orderbook.end(); ++it) {
        if ((it->second.isbuy == true) && (it->second.price > maxbuy))
            maxbuy = it->second.price;
        if ((it->second.isbuy == false) && (it->second.price < minsell))
            minsell = it->second.price;
    }
    //cout << minsell << " " << maxbuy << endl;
    return (maxbuy + minsell) / 2;
}

string toTime(int x) {
// Function: toTime
//     Give hour and minute numbers good formats for time representation.
// Input:
//     - x: the number of minute or hour
// Output:
//     The string of time format
    if (x > 9)
        return to_string(x);
    return "0" + to_string(x);
}

int main () {
    ifstream inputfile;
    ofstream outputfile;
    inputfile.open("mdma-pitch-20170828.dat");
    outputfile.open("YuhengZhou_res.csv");
    map<string, struct Order_cont> orderbook;
    map<string, struct Order_cont>::iterator it;
    string line;
    string rawtime, rawid, rawvolume, rawprice;
    int current_minute, proc_minute=0, volume, price, transvolume;
    int current_price, current_volume;
    struct Order_cont current_order;
    vector<struct Result> res_list;
    struct Result thismin_res;
    vector<int> volume_list;

    // Initialize the volume list
    for (int i=0; i<601; ++i)
        volume_list.push_back(0);
    proc_minute = current_minute = 420;

    //outputfile << "time,price,quantity" << endl;

    while (getline(inputfile, line)) {

        transvolume = 0;

        // Process order adding records, add orders into limit order book
        if ((line[9] == 'A') && (line.substr(29,4).compare("SPY ") == 0)) {
            rawtime = line.substr(1,9);
            current_minute = stoi(rawtime);
            current_minute /= 60000;
            rawid = line.substr(10,12);
            rawvolume = line.substr(23,6);
            rawprice = line.substr(35,10);
            current_order.volume = stoi(rawvolume);
            current_order.price = stoi(rawprice);
            //cout << rawid << " " << volume << " " << price << endl;
            if (line[22] == 'B')
                current_order.isbuy = true;
            else 
                current_order.isbuy = false;
            orderbook.insert(pair<string, struct Order_cont>(rawid, current_order));
        }

        // Process order adding records
        if (line[9] == 'X') {
            rawtime = line.substr(1,9);
            current_minute = stoi(rawtime);
            current_minute /= 60000;
            rawid = line.substr(10,12);
            rawvolume = line.substr(22,6);
            volume = stoi(rawvolume);
            it = orderbook.find(rawid);
            // The cancellation is for our ticker
            if (it != orderbook.end()) {
                // If it is a full cancellation, delete the order in the book
                if (volume >= it->second.volume)
                    orderbook.erase(it);
                // If it is a partial cancellation, minus the number from the order
                else
                    it->second.volume -= volume;
            }
        }

        /*if ((line[9] == 'A') && (line.substr(29,4).compare("SPY ") == 0)) {
            for (it=orderbook.begin(); it!=orderbook.end(); ++it)
                cout << it->first << " " << it->second.volume << " " << 
                    it->second.price << endl;
            cout << endl;
            cout << endl;
        }*/

        // Process market execution records
        if (line[9] == 'E') {
            rawtime = line.substr(1,9);
            current_minute = stoi(rawtime);
            current_minute /= 60000;
            rawid = line.substr(10,12);
            it = orderbook.find(rawid);
            if (it != orderbook.end()) {
                rawvolume = line.substr(22,6);
                transvolume = stoi(rawvolume);
                // Add executed volumes into results
                volume_list[current_minute-420] += transvolume;
                price = it->second.price;
                // If it is a full cancellation, delete the order in the book
                if (transvolume == it->second.volume) 
                    orderbook.erase(it);
                // If it is a partial cancellation, minus the number from the order
                else 
                    it->second.volume -= transvolume;
            }
            //cout << line << endl;
        }

        // Process hidden execution records
        if ((line[9] == 'P') && (line.substr(29,4).compare("SPY ") == 0)) {
            rawtime = line.substr(1,9);
            current_minute = stoi(rawtime);
            current_minute /= 60000;
            rawvolume = line.substr(23,6);
            transvolume = stoi(rawvolume);
            // Add executed volumes into results
            volume_list[current_minute-420] += transvolume;
            rawprice = line.substr(35,10);
            price = stoi(rawprice);
        }

        // When a new minute comes, conclude the last minute and give out results
        if (current_minute > proc_minute) {
            thismin_res.hour = proc_minute / 60;
            thismin_res.minute = proc_minute % 60;
            thismin_res.price = current_price;
            if (current_minute == 0)
                thismin_res.price = 0;
            thismin_res.volume = volume_list[proc_minute-420];
            // In zero-volume minutes, calculate mid price
            if ((current_volume == 0) && (proc_minute != 0))
                thismin_res.price = getMidOrder(orderbook);
            
            // Print out last minute's information
            if ((thismin_res.hour != 0) || (thismin_res.minute != 0))
                outputfile << toTime(thismin_res.hour) << ":" << 
                    toTime(thismin_res.minute) << "," << 
                    thismin_res.price/10000.0 << "," << thismin_res.volume << endl;
            proc_minute = current_minute;
            current_volume = 0;
        }
        // Still in this minute, go on accumulating
        else {
            current_price = price;
            current_volume += transvolume;
        }
    }
    
    inputfile.close();
    outputfile.close();
    return 0;
}