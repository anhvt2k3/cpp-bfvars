#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <math.h>
#include <random>
using namespace std;
string names = "names.csv";
string dataset0 = "dataset0.csv"; //@ 0 => all possible values
string dataset1 = "dataset1.csv"; //@ 1 => values: 0 -> 300k
string dataset2 = "dataset2.csv"; //@ 2 => values: 300k001 -> 1M
string dataset3 = "dataset3.csv"; //@ 3 => values: 0 -> 500k
string dataset4 = "dataset4.csv"; //@ 4 => values: 500k001 -> 1M

//todo dataset1 => keys (size = 700k)
//todo dataset2 => faulties (size = 300k)
//todo dataset3 => mixed by mergeWthOrder(50% of keys + 50% of faulties)
//todo dataset4 => mixed by mergeWthOrder(30% of faulties + 70% of keys)
//todo dataset4 => mixed by mergeWthoutOrder(50% of faulties + 50% of keys) 

//# value format: E<11-digits>_<name1> <name2> <6-digits>
vector<string> genDigit(int numLength, int numEle)
{
    vector<string> res;
    long long leng = pow(10, numLength);
    for (int i = 0; i < numEle; i++)
    {
        auto num = leng + i;
        res.push_back(to_string(num).erase(0, 1));
    }
    return res;
}

vector<string> readCSV(const string& filename)
{
    vector<string> lines;
    ifstream file(filename);
    if (file.is_open())
    {
        string line;
        while (getline(file, line))
        {
            lines.push_back(line);
        }
        file.close();
    }
    else
    {
        cout << "Failed to open file." << endl;
    }
    return lines;
}

void genDataset(string filename, long long min = 0, long long max = 1000000)
{   
    // 1000000 to fit size of both digit1 and digit2
    auto digit1 = genDigit(11, max);
    auto digit2 = genDigit(6, max);
    auto names = readCSV("names.csv");
    ofstream file(filename);
    if (file.is_open())
    {
        for (int i = min; i < max; i++)
        {
            string value = "E";
            // 1000 is the number of names in names.csv
            int nameidx = i % 999;
            value += digit1[i] + "_" + names[nameidx] + " " + names[nameidx+1] + " " + digit2[i];
            file << value << endl;
        }

        file.close();
        cout << "CSV file generated successfully." << endl;
    }
    else
    {
        cout << "Failed to open file." << endl;
    }
}

int main()
{
    // genDataset(dataset3,0,500000);
    // genDataset(dataset4,500001,1000000);
    return 0;
}