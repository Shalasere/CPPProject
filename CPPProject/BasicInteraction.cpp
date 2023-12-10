/* Author: Jacob Burns
    Program: JMC - Jacob's Movie Collection
    Purpose: To interact and get data about the movies
    that CinemaSins has covered.
*/ //1

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>

// Define a struct to represent movie information // 4
struct Movie {
    int index;
    std::string name;
    std::string link;
    std::string released;
    std::string alternateVersions;
    std::string alternateLink;
    std::string duration;
    long long sinCount;
    std::string writer1;
    std::string writer2;
    std::string writer3;
    std::string writer4;
    std::string writer5;
    std::string apiUrl;
    std::string apiResponse;
    std::string genre;
    std::string rating;
    std::string metascore;
    std::string awards;
};

// Function to split a CSV line into fields // 4,5,7
std::vector<std::string> splitCSVLine(const std::string& line) {
    std::vector<std::string> fields;
    std::istringstream ss(line);
    std::string field;

    while (std::getline(ss, field, ',')) {
        // Check if the field is quoted
        if (!field.empty() && field.front() == '"' && field.back() == '"') {
            // If the field is enclosed in quotes, remove leading and trailing quotes
            field = field.substr(1, field.size() - 2);
        } else {
            // Handle the case where the field contains a quote within it
            std::string temp;
            while (ss >> temp) {
                field += ',' + temp;
                if (temp.back() == '"') {
                    // If the last token ends with a quote, break
                    field = field.substr(1, field.size() - 2);
                    break;
                }
            }
        }

        // Remove leading and trailing whitespaces
        size_t start = field.find_first_not_of(" \t\r\n");
        size_t end = field.find_last_not_of(" \t\r\n");
        if (start != std::string::npos && end != std::string::npos)
            field = field.substr(start, end - start + 1);

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

// Function to search the movie array //4,5,7,9
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
            std::cout << m.index +1 << ". Movie Name: " << m.name << std::endl;

            found = true;
        }
    }
    return found;
}

// Functions to list movies below //3,8
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

// Function to display information about a specific movie
void displayMovieInfo(const Movie& movie) {
    std::cout << "Movie Name: " << movie.name << std::endl;
    std::cout << "Released: " << movie.released << std::endl;
    std::cout << "Alternate Versions: " << movie.alternateVersions << std::endl;
    std::cout << "Duration: " << movie.duration << std::endl;
    std::cout << "Sin Count: " << movie.sinCount << std::endl;
    std::cout << "genre: " << movie.genre << std::endl;
    std::cout << "metascore: " << movie.metascore << std::endl;
    std::cout << "awards: " << movie.awards << std::endl;
}

/// Function to select a movie and display its information
void selectMovie(const std::vector<Movie>& movies) {
    int selectedIndex;

    // Loop until a valid numeric input is received
    while (true) {
        std::cout << "Enter the number of the movie to see details (enter 0 to go back to menu): ";
        std::cin >> selectedIndex;

        if (std::cin.fail()) {
            // Input is not numeric, clear the error flag and discard the input
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Invalid input. Please enter a numeric value." << std::endl;
        } else {
            // Input is numeric, check if it's within a valid range
            if (selectedIndex == 0) {
                // User wants to go back to the menu
                return;
            } else if (selectedIndex > 0 && selectedIndex <= movies.size()) {
                // Valid selection
                const Movie& selectedMovie = movies[selectedIndex - 1];
                displayMovieInfo(selectedMovie);
                break;  // Exit the loop if a valid selection is made
            } else {
                std::cout << "Invalid selection. Please enter a valid movie number." << std::endl;
            }
        }
    }
}

// Function to display ASCII art from a file//6
void displayAsciiArt(const std::string& filename) {
    std::ifstream artFile(filename);
    if (artFile.is_open()) {
        std::string line;
        while (std::getline(artFile, line)) {
            std::cout << line << std::endl;
        }
        artFile.close();
    } else {
        std::cerr << "Failed to open ASCII art file: " << filename << std::endl;
    }
}

/*
    NERD FUNCTIONS ARE BELOW
*/
// Function for format conversion
int convertDurationToMinutes(const std::string& duration) {
    // Parse the duration in h:mm format and convert to minutes
    std::istringstream iss(duration);
    int hours = 0, minutes = 0;
    char colon = 0;

    // Attempt to parse hours and minutes
    if (!(iss >> hours) || (iss >> colon >> minutes).fail() || (colon != ':')) {
        throw std::invalid_argument("Invalid duration format");
    }

    return hours * 60 + minutes;
}

std::string calculateAverageDurationAndFindLongest(const std::vector<Movie>& movies) {
    if (movies.empty()) {
        return "0:00";  // Avoid division by zero
    }

    int totalMinutes = 0;
    Movie longestMovie = movies[0];

    for (const Movie& movie : movies) {
        try {
            // Parse the duration and convert to minutes
            int movieMinutes = convertDurationToMinutes(movie.duration);
            totalMinutes += movieMinutes;

            // Compare with the current longest movie
            if (movieMinutes > convertDurationToMinutes(longestMovie.duration)) {
                longestMovie = movie;
            }
        } catch (const std::invalid_argument& e) {
            continue;
        }
    }

    double averageMinutes = static_cast<double>(totalMinutes) / static_cast<double>(movies.size());

    int averageHours = static_cast<int>(averageMinutes) / 60;
    int averageRemainingMinutes = static_cast<int>(averageMinutes) % 60;

    // Convert to strings
    std::string avgHoursStr = std::to_string(averageHours);
    std::string avgMinutesStr = (averageRemainingMinutes < 10 ? "0" : "") + std::to_string(averageRemainingMinutes);

    std::string avgtime = avgHoursStr + ":" + avgMinutesStr;

    std::cout << "Average Duration: "<< avgtime << std::endl;

    std::cout << "Longest Movie: " << longestMovie.name << " - Duration: " << longestMovie.duration << std::endl;

    return avgtime;
}

void countGenres(const std::vector<Movie>& movies, std::vector<std::string>& uniqueGenres, std::vector<int>& genreCounts, std::vector<long long>& totalSinsPerGenre, std::vector<long long>& averageSinsPerGenre) {
    for (const Movie& movie : movies) {
        std::istringstream genreStream(movie.genre);
        std::string individualGenre;

        while (genreStream >> individualGenre) {
            auto it = std::find(uniqueGenres.begin(), uniqueGenres.end(), individualGenre);
            if (it == uniqueGenres.end()) {
                // Genre not found, add it to the vectors
                uniqueGenres.push_back(individualGenre);
                genreCounts.push_back(1);
                totalSinsPerGenre.push_back(movie.sinCount);
            } else {
                // Genre found, increment the corresponding count and update total sins
                int index = std::distance(uniqueGenres.begin(), it);
                genreCounts[index]++;
                totalSinsPerGenre[index] += movie.sinCount;
            }
        }
    }
    // Calculate average sins per genre
    averageSinsPerGenre.resize(uniqueGenres.size());
    for (size_t i = 0; i < uniqueGenres.size(); ++i) {
        averageSinsPerGenre[i] = static_cast<long long>(totalSinsPerGenre[i]) / genreCounts[i];
    }
}



// Function to display all the different stats from the data
void displayStats(const std::vector<Movie>& movies) {
    int totalMovies = movies.size();
    // Add more statistical calculations as needed
    // ...

    std::cout << "Total Movies: " << totalMovies << std::endl;
    // Display other statistics
    // ...
}

// Function to sanitize a string
std::string sanitizeString(const std::string& input) {
    std::string result = input;

    // Remove leading and trailing backslashes and double quotes
    while (!result.empty() && (result.front() == '\\' || result.front() == '"')) {
        result = result.substr(1);
    }
    while (!result.empty() && (result.back() == '\\' || result.back() == '"')) {
        result.pop_back();
    }

    return result;
}

// Function to parse the json-formatted api Response
std::string extractfield(const std::string& category, const std::string& jsonString) {
    size_t fieldStart = jsonString.find("\\\"" + category + "\\\":\\\"");
    if (fieldStart == std::string::npos) {
        return "";  // Genre not found
    }

    fieldStart += 12;  // Move to the start of the genre value
    size_t fieldEnd = jsonString.find("\\\"", fieldStart);
    if (fieldEnd == std::string::npos) {
        return "";  // Closing quote not found
    }
    std::string pretty = jsonString.substr(fieldStart, fieldEnd - fieldStart);
    std::replace(pretty.begin(), pretty.end(), ',', ' ');
    std::replace(pretty.begin(), pretty.end(), '"', ' ');
    std::replace(pretty.begin(), pretty.end(), ':', ' ');
    return pretty;
}

int main() {
    // Display ascii art at program launch.
    displayAsciiArt("Title-art.txt");
    std::cout << "Starting the program..." << std::endl;

    std::ifstream file("output_movies.csv"); //6,9
    if (!file.is_open()) {
        std::cerr << "Failed to open the CSV file." << std::endl;
        return 1;
    }

    std::vector<Movie> movies;
    std::string line;

    // Read the header line and discard it
    std::getline(file, line);

    int index = 0;
    // Read and parse each line of the CSV
    while (std::getline(file, line)) {
        //std::vector<std::string> fields = ParseCSV(line);
        std::vector<std::string> fields = splitCSVLine(line);


        if (fields.size() >= 13) {
            Movie movie;
            movie.name = fields[0];
            movie.released = fields[2];
            movie.alternateVersions = fields[3];
            movie.duration = fields[5];
            try {
                movie.sinCount = std::stoll(fields[6]);
            } catch (const std::invalid_argument& e) {
                std::cerr << "Failed to parse sinCount for line: " << line << std::endl;
                continue;
            }
            int j = 0;
            for (const std::string&field : fields){
                
                if (field.find("http://www.om") != std::string::npos) {
                    break;
                }
                j++;
            }
            
            movie.apiUrl = fields[j];
            movie.apiResponse = fields[j+1];
            movie.apiResponse = sanitizeString(movie.apiResponse);
            if (movie.apiResponse != "False") {
                std::string cat;
                cat = "Genre";
                movie.genre = extractfield(cat,fields[j+2]);
                cat = "Rating";
                movie.rating = extractfield(cat,fields[j+2]);
                cat = "Awards";
                movie.awards = extractfield(cat,fields[j+2]);
                cat = "Metascore";
                movie.metascore = extractfield(cat,fields[j+2]);
            }
            movie.index = index++;
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
        // Display a consistent prompt for user choice
        std::cout << "Do you want to 'list', 'search','quit' or get 'stats' like a nerd?? ";
        std::string userChoice;
        std::cin >> userChoice;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Clear input buffer

        if (userChoice == "quit") {
            break;
        } else if (userChoice == "list") {
            int pageSize = 10;
            int page = 0;
            bool keepListing = true;  // Add a flag to control whether to stay in the listing mode

            while (keepListing) {
                displayMovieList(movies, page, pageSize);
                selectMovie(movies);  // Call the common movie selection function
                int nextPage = getPageIndex(page, pageSize);

                if (nextPage == -1) {
                    break;
                } else {
                    page = nextPage;
                }
            }
        } else if (userChoice == "search") {
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

                    if (movieFound) {
                        selectMovie(movies);  // Call the common movie selection function
                        break;
                    } else {
                        std::cout << "Movie not found." << std::endl;
                        break; // Allow user to go back to the menu
                    }
                }
            }
        } else if (userChoice == "debug") {
            for (const Movie& m : movies) {
                std::cout << m.name << std::endl;
            }
        } else if (userChoice == "stats"){
            calculateAverageDurationAndFindLongest(movies);

            // Vectors to store unique genres, their counts, and total sins per genre
            std::vector<std::string> uniqueGenres;
            std::vector<int> genreCounts;
            std::vector<long long> totalSinsPerGenre;
            std::vector<long long> averageSinsPerGenre;

            // Count genres
            countGenres(movies, uniqueGenres, genreCounts, totalSinsPerGenre, averageSinsPerGenre);

            std::cout << "Genre info (Genre: # of Genre, Average sins per Genre, Total Sins per Genre)" << std::endl;
            for (size_t i = 0; i < uniqueGenres.size(); ++i) {
                std::cout << uniqueGenres[i] << ": " << genreCounts[i] << " , " << averageSinsPerGenre[i] << " , " << totalSinsPerGenre[i] << std::endl;
            }

        } else {
            std::cout << "Invalid choice. Please enter 'list', 'search', or 'quit'." << std::endl;
        }
    }
}