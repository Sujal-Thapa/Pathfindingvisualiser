#include <iostream>
#include <vector>
#include <queue>
#include <SDL2/SDL.h>

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;
const int CELL_SIZE = 20;
const int GRID_WIDTH = SCREEN_WIDTH / CELL_SIZE;
const int GRID_HEIGHT = SCREEN_HEIGHT / CELL_SIZE;

SDL_Window* gWindow = NULL;
SDL_Renderer* gRenderer = NULL;

enum class CellState {
    EMPTY,
    WALL,
    START,
    TARGET,
    PATH
};

std::vector<std::vector<CellState>> grid(GRID_WIDTH, std::vector<CellState>(GRID_HEIGHT, CellState::EMPTY));

void initSDL() {
    SDL_Init(SDL_INIT_VIDEO);
    gWindow = SDL_CreateWindow("Pathfinding Visualizer", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED);
}

void closeSDL() {
    SDL_DestroyRenderer(gRenderer);
    SDL_DestroyWindow(gWindow);
    SDL_Quit();
}

void drawCell(int x, int y, CellState state) {
    SDL_Rect cellRect = { x * CELL_SIZE, y * CELL_SIZE, CELL_SIZE, CELL_SIZE };
    switch (state) {
        case CellState::EMPTY:
            SDL_SetRenderDrawColor(gRenderer, 255, 255, 255, 255); // White color
            break;
        case CellState::WALL:
            SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 255); // Black color
            break;
        case CellState::START:
            SDL_SetRenderDrawColor(gRenderer, 0, 255, 0, 255); // Green color
            break;
        case CellState::TARGET:
            SDL_SetRenderDrawColor(gRenderer, 255, 0, 0, 255); // Red color
            break;
        case CellState::PATH:
            SDL_SetRenderDrawColor(gRenderer, 0, 0, 255, 255); // Blue color
            break;
    }
    SDL_RenderFillRect(gRenderer, &cellRect);
}

bool isValid(int x, int y) {
    return x >= 0 && x < GRID_WIDTH && y >= 0 && y < GRID_HEIGHT && grid[x][y] != CellState::WALL;
}

void findShortestPath(int startX, int startY, int targetX, int targetY) {
    std::vector<std::vector<bool>> visited(GRID_WIDTH, std::vector<bool>(GRID_HEIGHT, false));
    std::vector<std::vector<std::pair<int, int>>> parent(GRID_WIDTH, std::vector<std::pair<int, int>>(GRID_HEIGHT, {-1, -1}));

    std::queue<std::pair<int, int>> q;
    q.push({startX, startY});
    visited[startX][startY] = true;

    int dx[] = {-1, 1, 0, 0};
    int dy[] = {0, 0, -1, 1};

    while (!q.empty()) {
        int x = q.front().first;
        int y = q.front().second;
        q.pop();

        if (x == targetX && y == targetY) {
            // Reconstruct path
            while (x != startX || y != startY) {
                grid[x][y] = CellState::PATH;
                std::pair<int, int> p = parent[x][y];
                x = p.first;
                y = p.second;
            }
            return;
        }

        for (int i = 0; i < 4; ++i) {
            int nx = x + dx[i];
            int ny = y + dy[i];
            if (isValid(nx, ny) && !visited[nx][ny]) {
                q.push({nx, ny});
                visited[nx][ny] = true;
                parent[nx][ny] = {x, y};
            }
        }
    }
}

void handleEvents(int& startX, int& startY, int& targetX, int& targetY) {
    SDL_Event e;
    while (SDL_PollEvent(&e) != 0) {
        if (e.type == SDL_QUIT) {
            closeSDL();
            exit(EXIT_SUCCESS);
        } else if (e.type == SDL_MOUSEBUTTONDOWN) {
            int mouseX, mouseY;
            SDL_GetMouseState(&mouseX, &mouseY);
            int gridX = mouseX / CELL_SIZE;
            int gridY = mouseY / CELL_SIZE;
            if (e.button.button == SDL_BUTTON_LEFT) {
                if (gridX >= 0 && gridX < GRID_WIDTH && gridY >= 0 && gridY < GRID_HEIGHT) {
                    if (grid[gridX][gridY] == CellState::EMPTY)
                        grid[gridX][gridY] = CellState::WALL;
                    else if (grid[gridX][gridY] == CellState::WALL)
                        grid[gridX][gridY] = CellState::EMPTY;
                }
            } else if (e.button.button == SDL_BUTTON_RIGHT) {
                if (gridX >= 0 && gridX < GRID_WIDTH && gridY >= 0 && gridY < GRID_HEIGHT) {
                    if (grid[gridX][gridY] != CellState::WALL) {
                        if (startX == -1 && startY == -1) {
                            grid[gridX][gridY] = CellState::START;
                            startX = gridX;
                            startY = gridY;
                        } else if (targetX == -1 && targetY == -1) {
                            grid[gridX][gridY] = CellState::TARGET;
                            targetX = gridX;
                            targetY = gridY;
                        }
                    }
                }
            }
        } else if (e.type == SDL_KEYDOWN) {
            if (e.key.keysym.sym == SDLK_SPACE && startX != -1 && startY != -1 && targetX != -1 && targetY != -1) {
                findShortestPath(startX, startY, targetX, targetY);
            }
        }
    }
}

void drawGrid() {
    SDL_SetRenderDrawColor(gRenderer, 255, 255, 255, 255); // White color
    SDL_RenderClear(gRenderer);

    for (int x = 0; x < GRID_WIDTH; ++x) {
        for (int y = 0; y < GRID_HEIGHT; ++y) {
            drawCell(x, y, grid[x][y]);
        }
    }

    SDL_RenderPresent(gRenderer);
}

int main(int argc, char* args[]) {
    initSDL();

    int startX = -1, startY = -1, targetX = -1, targetY = -1;

    bool quit = false;
    while (!quit) {
        handleEvents(startX, startY, targetX, targetY);
        drawGrid();
        SDL_Delay(10); // Delay to control the frame rate
    }

    closeSDL();
    return 0;
}
