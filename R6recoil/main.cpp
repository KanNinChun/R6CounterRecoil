#include <iostream>
#include <vector>
#include <fstream>
#include <Windows.h>
#include <string>

struct Position {
    int x;
    int y;
};

// Function to parse the positions from the file
std::vector<Position> parsePositions(const std::string& filename) {
    std::vector<Position> positions;
    std::ifstream inFile(filename);

    if (inFile.is_open()) {
        std::string line;
        while (std::getline(inFile, line)) {
            if (line.find("->") != std::string::npos) {
                size_t pos = line.find("->");
                std::string coords = line.substr(pos + 3);
                int x, y;
                sscanf_s(coords.c_str(), "%d , %d", &x, &y);
                positions.push_back({ x, y });
            }
        }
        inFile.close();
    }
    else {
        std::cerr << "Unable to open file." << std::endl;
    }

    return positions;
}

// Function to move the mouse smoothly to a position using SendInput
void moveMouseRelative(int deltaX, int deltaY) {
    INPUT input = { 0 };
    input.type = INPUT_MOUSE;
    input.mi.dx = deltaX;
    input.mi.dy = deltaY;
    input.mi.dwFlags = MOUSEEVENTF_MOVE;
    SendInput(1, &input, sizeof(INPUT));
}

int main() {
    std::string filename = "R6pattern.txt";
    std::vector<Position> positions = parsePositions(filename);

    if (positions.empty()) {
        std::cerr << "No positions found in the file." << std::endl;
        return 1;
    }
    else {
        std::cout << "Recoil pattern found! Nice one." << std::endl;
    }

    bool start = false;

    // Wait for Numpad 2 to start
    while (!start) {
        if (GetAsyncKeyState(VK_NUMPAD2) & 0x8000) { // Numpad key 2 to start
            start = true;
            std::cout << "Status: " << start << std::endl;
            Sleep(500); // To avoid multiple triggers
        }
    }

    std::cout << "Countering recoil pattern:" << std::endl;

    bool stop = false;

    while (!stop) {
        if ((GetAsyncKeyState(VK_LBUTTON) & 0x8000) && (GetAsyncKeyState(VK_RBUTTON) & 0x8000)) {
            Position previousPos = positions[0];
            for (size_t i = 0; i < positions.size(); ++i) {
                const auto& pos = positions[i];
                if (!(GetAsyncKeyState(VK_LBUTTON) & 0x8000) || !(GetAsyncKeyState(VK_RBUTTON) & 0x8000)) {
                    break; // Stop if either button is released
                }
                if (GetAsyncKeyState(VK_NUMPAD3) & 0x8000) { // Numpad key 3 to stop
                    stop = true;
                    std::cout << "Counter-recoil stopped." << std::endl;
                    break;
                }
                // Calculate the delta for countering recoil
                int deltaX = previousPos.x - pos.x;
                int deltaY = previousPos.y - pos.y;
                moveMouseRelative(deltaX, deltaY);
                std::cout << "Countered Bullet " << (i + 1) << " \nMoved by delta: (" << deltaX << ", " << deltaY << ")" << std::endl;
                previousPos = pos;
                Sleep(7); // Small delay to smooth out the movement
            }
        }
    }

    std::cout << "Press Enter to exit the program." << std::endl;
    std::cin.get(); // Wait for user input before closing

    return 0;
}