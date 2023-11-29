#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <algorithm>
#include <iomanip>
#include <cpr/cpr.h> // You'll need to install a C++ HTTP library like 'cpr' for making HTTP requests. You can install it using a package manager like Conan.

using namespace std;

// Function to format movie title for URL
string formatMovieTitle(const string& movieTitle) {
    string formattedTitle = movieTitle;
    replace(formattedTitle.begin(), formattedTitle.end(), ' ', '+');
    return formattedTitle;
}

// Function to call the API
map<string, string> callApi(const string& movieTitle, const string& apiKey) {
    string baseUrl = "http://www.omdbapi.com/?";
    string formattedTitle = formatMovieTitle(movieTitle);
    
    string apiUrl = baseUrl + "t=" + formattedTitle + "&apikey=" + apiKey;

    cpr::Response response = cpr::Get(cpr::Url{apiUrl});
    cout << response.url << endl;
    map<string, string> data;

    if (response.status_code == 200) {
        // Assuming the response is JSON and contains key-value pairs
        istringstream jsonStream(response.text);
        string line;
        while (getline(jsonStream, line)) {
            istringstream lineStream(line);
            string key, value;
            getline(lineStream, key, ':');
            getline(lineStream, value);
            data[key] = value;
        }
    } else {
        cout << "API call failed for '" << movieTitle << "' with status code: " << response.status_code << endl;
    }
    return data;
}

int main() {
    string apiKey = "e80cde5e";
    ifstream inputFile("Grid_view.csv");
    
    if (!inputFile) {
        cerr << "Failed to open input CSV file." << endl;
        return 1;
    }

    ofstream outputFile("output_movies.csv");

    if (!outputFile) {
        cerr << "Failed to open output CSV file." << endl;
        return 1;
    }

    string line;
    bool firstLine = true;

    while (getline(inputFile, line)) {
        istringstream lineStream(line);
        vector<string> tokens;
        string token;

        while (getline(lineStream, token, ',')) {
            tokens.push_back(token);
        }

        if (firstLine) {
            // Header line
            tokens.push_back("API Url");
            tokens.push_back("API Response");
            firstLine = false;
        } else {
            string movieTitle = tokens[0];
            string apiUrl = "http://www.omdbapi.com/?t=" + formatMovieTitle(movieTitle) + "&apikey=" + apiKey;
            map<string, string> apiData = callApi(movieTitle, apiKey);
            
            if (!apiData.empty()) {
                tokens.push_back(apiUrl);
                 // Loop through the values in the apiData map and concatenate them
                string concatenatedApiResponse;
                for (const auto& entry : apiData) {
                    concatenatedApiResponse += entry.second + " ";
                }
        
                tokens.push_back(concatenatedApiResponse);
            } else {
                tokens.push_back(apiUrl);
                tokens.push_back("API call failed");
            }
        }

        for (size_t i = 0; i < tokens.size(); ++i) {
            outputFile << quoted(tokens[i]);
            if (i < tokens.size() - 1) {
                outputFile << ",";
            }
        }
        outputFile << endl;
    }

    inputFile.close();
    outputFile.close();

    cout << "Processing completed." << endl;

    return 0;
}
