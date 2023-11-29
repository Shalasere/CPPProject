#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>

// Define a struct to represent movie information
struct Movie {
    std::string name;
    std::string link;
    std::string released;
    std::string alternateVersions;
    std::string alternateLink;
    std::string duration;
    std::string sinCount;
    std::string writer1;
    std::string writer2;
    std::string writer3;
    std::string writer4;
    std::string writer5;
    std::string apiUrl;
    std::string apiResponse;
};

// Function to split a CSV line into fields
std::vector<std::string> splitCSVLine(const std::string& line) {
    std::vector<std::string> fields;
    std::istringstream ss(line);
    std::string field;

    while (std::getline(ss, field, ',')) {
        // Remove leading and trailing quotes from the field
        while (!field.empty() && field.front() == '"') {
            field = field.substr(1);
        }
        while (!field.empty() && field.back() == '"') {
            field.pop_back();
        }
        fields.push_back(field);
    }

    return fields;
}

// Function to convert a string to lowercase
std::string toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(),
        [](unsigned char c) { return std::tolower(c); });
    return result;
}

// Function to search the movie array
bool searchMovie(const std::vector<Movie>& movies, const std::string& queryLower) {
    bool found = false;
    for (const Movie& m : movies) {
        std::istringstream iss(m.name);
        std::string word;
        bool movieNameLowerContainsQuery = false;

        while (iss >> word) {
            std::string movieNameWordLower = toLower(word);
            if (movieNameWordLower == queryLower) {
                movieNameLowerContainsQuery = true;
                break;
            }
        }
        if (movieNameLowerContainsQuery) {
            std::cout << "Movie Name: " << m.name << std::endl;
            found = true;
        }
    }
    return found;
}

// Functions to list movies below
void listMovies(const std::vector<Movie>& movies, int start, int end) {
    for (int i = start; i < end && i < movies.size(); ++i) {
        std::cout << i + 1 << ". " << movies[i].name << std::endl;
    }
}

int getPageIndex(int currentPage, int pageSize) {
    std::string input;
    std::cout << "Enter 'next' to see more, 'back' to return to menu: ";
    std::cin >> input;

    if (input == "back") {
        return -1;
    } else if (input == "next") {
        return currentPage + 1;
    }

    return currentPage;
}

void displayMovieList(const std::vector<Movie>& movies, int page, int pageSize) {
    int startIndex = page * pageSize;
    int endIndex = startIndex + pageSize;

    std::cout << "Page " << (page + 1) << " (Movies " << (startIndex + 1) << " to " << endIndex << "):" << std::endl;
    listMovies(movies, startIndex, endIndex);

    if (endIndex >= movies.size()) {
        std::cout << "End of the list." << std::endl;
    }
}

int main() {
    std::cout << "Starting the program..." << std::endl;

    std::ifstream file("output_movies.csv");
    if (!file.is_open()) {
        std::cerr << "Failed to open the CSV file." << std::endl;
        return 1;
    }

    std::vector<Movie> movies;
    std::string line;

    // Read the header line and discard it
    std::getline(file, line);

    // Read and parse each line of the CSV
    while (std::getline(file, line)) {
        std::vector<std::string> fields = splitCSVLine(line);

        if (fields.size() >= 13) {
            Movie movie;
            movie.name = fields[0];
            movie.link = fields[1];
            movie.released = fields[2];
            movie.alternateVersions = fields[3];
            movie.alternateLink = fields[4];
            movie.duration = fields[5];
            try {
                movie.sinCount = fields[6];
            } catch (const std::invalid_argument& e) {
                std::cerr << "Failed to parse sinCount for line: " << line << std::endl;
                continue;
            }
            movie.writer1 = fields[7];
            movie.writer2 = fields[8];
            movie.writer3 = fields[9];
            movie.writer4 = fields[10];
            movie.writer5 = fields[11];
            movie.apiUrl = fields[12];

            movies.push_back(movie);
        } else {
            std::cerr << "Invalid number of fields for line: " << line << std::endl;
        }
    }

    std::cout << "Number of Movies Read: " << movies.size() << std::endl;

    if (movies.empty()) {
        std::cerr << "No movies were successfully read from the CSV file." << std::endl;
        return 1;
    }
    bool menuMode = true;

    while (menuMode) {
        // Display an initial prompt to ask the user if they want to search or list first
        std::cout << "Do you want to 'list' or 'search' for a movie? (Enter 'quit' to exit): ";
        std::string initialChoice;
        std::cin >> initialChoice;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Clear input buffer


        if (initialChoice == "quit") {
            break;
        } else if (initialChoice == "list") {
            int pageSize = 10;
            int page = 0;
            bool keepListing = true;  // Add a flag to control whether to stay in the listing mode

            while (keepListing) {
                displayMovieList(movies, page, pageSize);
                int nextPage = getPageIndex(page, pageSize);
                
                if (nextPage == -1) {
                    break;
                } else {
                    page = nextPage;
                }
            }
        } else if (initialChoice == "search") {
            // Prompt for a movie search
            while (true) {
                std::string query;
                std::cout << "Enter a movie name to search for, or enter 'back' to return to menu: ";
                std::getline(std::cin, query);

                // Convert the user input to lowercase for case-insensitive comparison
                std::string queryLower = toLower(query);

                if (queryLower == "back") {
                    break;
                } else {
                    bool movieFound = searchMovie(movies, queryLower);

                    if (!movieFound) {
                        std::cout << "Movie not found." << std::endl;
                    }
                }
            }
        } else if (initialChoice == "debug"){
            for (const Movie& m : movies) {
                std::cout << m.name << std::endl;
            }
        }
    }
    return 0;
}
