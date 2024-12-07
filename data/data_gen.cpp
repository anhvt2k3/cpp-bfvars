#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <math.h>
#include <random>
using namespace std;

string namescsv = "D:\\Desktop\\csbf\\data\\names.csv";
string dataset0 = "D:\\Desktop\\csbf\\data\\dataset0.csv"; //@ 0 => all possible values
string dataset1 = "D:\\Desktop\\csbf\\data\\dataset1.csv"; //@ 1 => values: 1 -> 200k+1
string dataset2 = "D:\\Desktop\\csbf\\data\\dataset2.csv"; //@ 2 => values: 200k+1 -> 400k
string dataset3 = "D:\\Desktop\\csbf\\data\\dataset3.csv"; //@ 3 => values: 400k+1 -> 600k
string dataset4 = "D:\\Desktop\\csbf\\data\\dataset4.csv"; //@ 4 => values: 600k+1 -> 800k
string dataset5 = "D:\\Desktop\\csbf\\data\\dataset5.csv"; //@ 5 => values: 800k+1 -> 1M

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
        cout << "Failed to open file in readCSV." << endl;
    }
    return lines;
}

void genDataset(string filename, long long min = 0, long long max = 1000000)
{   
    // 1000000 to fit size of both digit1 and digit2
    auto digit1 = genDigit(11, max+1);
    auto digit2 = genDigit(6, max+1);
    auto names = readCSV(namescsv);
    // a random number between min and max
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(0, digit2.size() - 1);

    ofstream file(filename);
    if (file.is_open())
    {
        for (int i = min; i < max+1; i++)
        {
            string value = "E";
            // 1000 is the number of names in names.csv
            auto dg2index = dis(gen);
            auto nameidx = dg2index % 999;
            auto nameidx_ = i % 999;
            value += digit1[i] + "_" + names[nameidx] + " " + names[nameidx_] + " " + digit2[dg2index];
            file << value << endl;
        }

        file.close();
        cout << "CSV file generated successfully." << endl;
    }
    else
    {
        cout << "Failed to open file in genDataset." << endl;
    }
}

void createDataset0(string filename = dataset0, vector<string> sets = {dataset1, dataset2, dataset3, dataset4, dataset5})
{
    ofstream outfile(filename);
    if (outfile.is_open())
    {
        for (auto set:sets)
        {
            ifstream fileread(set); // outfile opening error not included
            string line;
            while (getline(fileread, line))
            {
                outfile << line << endl; // outfile writing error not included
            }
            fileread.close();
        }
        cout << "All datasets combined successfully." << endl;
        outfile.close();
    } else 
    {
        cout << "Failed to open output file in createWholeset." << endl;
    }
}

int main()
{
    genDataset(dataset1,1,200000);
    genDataset(dataset2,200001,400000);
    genDataset(dataset3,400001,600000);
    genDataset(dataset4,600001,800000);
    genDataset(dataset5,800001,1000000);
    createDataset0();
    return 0;
}