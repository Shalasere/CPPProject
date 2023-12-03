#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>


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

struct Square {
    SDL_Rect rect;
    SDL_Color color;
    Movie movie;
};

struct Tab {
    std::string title;
    SDL_Rect contentArea;
    SDL_Rect tabRect;
    std::vector<Square> squares;
    
};

// Define a struct to represent movie information


bool pointInsideRect(int x, int y, const SDL_Rect& rect) {
    return x >= rect.x && x < rect.x + rect.w && y >= rect.y && y < rect.y + rect.h;
}

void assignMoviesToSquares(std::vector<Tab>& tabs, const std::vector<Movie>& movies, int activeTab) {
    const int moviesPerTab = tabs[activeTab].squares.size(); // Adjust as needed
        // Distribute movies to squares in the current tab
        for (int i = 0; i < std::min(moviesPerTab, static_cast<int>(movies.size())); ++i) {
            const Movie& movie = movies[i];
            SDL_Color randomColor = {
                static_cast<Uint8>(rand() % 256),
                static_cast<Uint8>(rand() % 256),
                static_cast<Uint8>(rand() % 256),
                255
            };

            // Create a square with the movie information
            Square square{
                {0, 0, 0, 0}, // Adjust the rect later in renderSquares
                randomColor,
                movie
            };

            // Add the square to the tab
            tabs[activeTab].squares.push_back(square);
        }
    }

void resizeTabs(std::vector<Tab>& tabs, int windowWidth, int windowHeight) {
    for (int i = 0; i < tabs.size(); ++i) {
        tabs[i].contentArea.w = windowWidth;
        tabs[i].contentArea.h = windowHeight - 30; // Adjust as needed
        tabs[i].tabRect = {i * (windowWidth / int(tabs.size())), 0, windowWidth / int(tabs.size()), 30};
    }
}

void renderSquares(SDL_Renderer* renderer, std::vector<Tab>& tabs, int activeTab) {
    Tab& tab = tabs[activeTab];  // Use a reference here
    tabs[activeTab].squares.clear(); // Clear existing squares
    const int targetNumSquares = 100;
    const int minSquareSize = 50;
    double aspectRatio = double(tab.contentArea.w) / tab.contentArea.h;

    int rows = std::sqrt(targetNumSquares / aspectRatio);
    int cols = targetNumSquares / rows;
    int squareSize = std::min(tab.contentArea.w / cols, tab.contentArea.h / rows);
    squareSize = std::max(squareSize, minSquareSize);

    for (int row = 0; row < rows; ++row) {
        for (int col = 0; col < cols; ++col) {
            Square square;
            SDL_Color randomColor = {
                static_cast<Uint8>(rand() % 256),
                static_cast<Uint8>(rand() % 256),
                static_cast<Uint8>(rand() % 256),
                255
            };

            square.rect.x = static_cast<Sint16>(tab.contentArea.x + col * squareSize);
            square.rect.y = static_cast<Sint16>(tab.contentArea.y + row * squareSize);
            square.rect.w = static_cast<Uint16>(squareSize);
            square.rect.h = static_cast<Uint16>(squareSize);
            square.color = randomColor;
            tab.squares.push_back(square);
        }
    }

    // Render the content of the active tab
    SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);
    SDL_RenderFillRect(renderer, &tab.contentArea);

    // Render the squares
    for (const auto& square : tab.squares) {
        SDL_SetRenderDrawColor(renderer, square.color.r, square.color.g, square.color.b, 255);
        SDL_RenderFillRect(renderer, &square.rect);
    }
    std::cout << "Rendered squares: " << tab.squares.size() << std::endl;
}

void renderContent(SDL_Renderer* renderer, TTF_Font* font, std::vector<Tab>& tabs, int activeTab, int windowWidth, int windowHeight, std::vector<Movie>& movies) {
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderClear(renderer);

    resizeTabs(tabs, windowWidth, windowHeight); // Resize the tabs
    assignMoviesToSquares(tabs,movies,activeTab);
    renderSquares(renderer, tabs, activeTab);
    SDL_RenderPresent(renderer);
}



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


int main() {
    std::srand(static_cast<unsigned>(std::time(nullptr)));
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL initialization failed: " << SDL_GetError() << std::endl;
        return 1;
    }

    // Create a window
    SDL_Window* window = SDL_CreateWindow("Movies in CSV", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 800, 600, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    int window_w, window_h;
    SDL_GetWindowSize(window, &window_w, &window_h);
    if (window == nullptr) {
        std::cerr << "Window creation failed: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    // Create a renderer
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == nullptr) {
        std::cerr << "Renderer creation failed: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    // Initialize SDL_ttf
    if (TTF_Init() == -1) {
        std::cerr << "SDL_ttf initialization failed: " << TTF_GetError() << std::endl;
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    // Load a font
    TTF_Font* font = TTF_OpenFont("font.ttf", 18); // Replace with the path to your font file
    if (font == nullptr) {
        std::cerr << "Failed to load font: " << TTF_GetError() << std::endl;
        TTF_Quit();
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

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






    // Define the number of tabs
    const int numTabs = 10;  // You can adjust this as needed

    // Define tabs using a range-based for loop
    std::vector<Tab> tabs;
    for (int i = 1; i <= numTabs; ++i) {
        tabs.push_back({"Tab " + std::to_string(i), {0,30,window_w,window_h-30}, {},});
    }
    // Define tab size
    for (size_t i = 0; i < tabs.size(); ++i) {
        tabs[i].tabRect = {i * (window_w/tabs.size()), 0, (window_w/tabs.size()), 30};
    }
    renderContent(renderer, font, tabs, 0, window_w, window_h, movies);


    

    int activeTab = 0;
    bool quit = false;
    SDL_Event event;

    while (!quit) {
        while (SDL_PollEvent(&event) != 0) {
            if (event.type == SDL_QUIT) {
                quit = true;
            } else if (event.type == SDL_MOUSEBUTTONDOWN) {
                int mouseX, mouseY;
                SDL_GetMouseState(&mouseX, &mouseY);

                for (size_t i = 0; i < tabs.size(); ++i) {
                    if (pointInsideRect(mouseX, mouseY, tabs[i].tabRect)) {
                        activeTab = static_cast<int>(i);
                        assignMoviesToSquares(tabs,movies,activeTab);
                        renderSquares(renderer, tabs, activeTab);
                    }
                }
            } else if (event.type == SDL_WINDOWEVENT) {
                if (event.window.event == SDL_WINDOWEVENT_RESIZED) {
                    int mouseX, mouseY;
                    SDL_GetMouseState(&mouseX, &mouseY);

                    for (size_t i = 0; i < tabs.size(); ++i) {
                        if (pointInsideRect(mouseX, mouseY, tabs[i].tabRect)) {
                            activeTab = static_cast<int>(i);
                            assignMoviesToSquares(tabs,movies,activeTab);
                            renderSquares(renderer, tabs, activeTab);
                        }
                    }
                    // Handle window resize event here
                    // Update the dimensions of your content areas accordingly
                    std::cout << "resize has happened" << std::endl;
                    std::cout << "new window_w:" << event.window.data1 << std::endl;
                    std::cout << "new window_h:" << event.window.data2 << std::endl;
                    int newwindow_w = event.window.data1;
                    int newwindow_h = event.window.data2;
                    renderContent(renderer, font, tabs, 0, newwindow_w, newwindow_h, movies);
                }
            }       
        }
       
        // Render the tab titles
        for (const auto& tab : tabs) {
            SDL_SetRenderDrawColor(renderer, (tab.tabRect.x == tabs[activeTab].tabRect.x) ? 100 : 150, (tab.tabRect.x == tabs[activeTab].tabRect.x) ? 150 : 200, 255, 255);
            SDL_RenderFillRect(renderer, &tab.tabRect);

             // Scale the font size based on the width of the tab
            int fontSize = tab.tabRect.w / 5;
            TTF_Font* scaledFont = TTF_OpenFont("font.ttf", fontSize);
            if (scaledFont != nullptr) {
                SDL_Color textColor = {0, 0, 0, 255};
                SDL_Surface* textSurface = TTF_RenderText_Solid(scaledFont, tab.title.c_str(), textColor);
                SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
                SDL_Rect textRect = {tab.tabRect.x + 10, tab.tabRect.y + 5, textSurface->w, textSurface->h};
                SDL_RenderCopy(renderer, textTexture, nullptr, &textRect);

                SDL_FreeSurface(textSurface);
                SDL_DestroyTexture(textTexture);
                TTF_CloseFont(scaledFont);  // Close the scaled font
            }

            SDL_RenderDrawLine(renderer, tab.tabRect.x, tab.tabRect.y + tab.tabRect.h, tab.tabRect.x + tab.tabRect.w, tab.tabRect.y + tab.tabRect.h);
        }

        SDL_RenderPresent(renderer);
    }

    // Clean up
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_CloseFont(font);
    TTF_Quit();
    SDL_Quit();

    return 0;
}
