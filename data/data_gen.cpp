#include <iostream>
#include <fstream>
#include <string>
#include <vector>
using namespace std;
string outfile = "output.csv";

void generateCSV(const vector<string>& data)
{
    ofstream file(outfile);
    if (file.is_open())
    {
        for (const auto& str : data)
        {
            file << str << ",";
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
    vector<string> data = {"Hello", "World", "GitHub", "Copilot"};
    generateCSV(data);

    return 0;
}