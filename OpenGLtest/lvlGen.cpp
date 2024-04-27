#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <string>

// Function to generate a random number between min and max (inclusive)
int getRandomNumber(int min, int max)
{
    return rand() % (max - min + 1) + min;
}

// Function to check if a cell is valid for placing a 1
bool isValidCell(const std::vector<std::vector<int>> &matrix, int row, int col)
{
    // Check if the cell is out of bounds or if it already contains a 1
    if (row < 0 || row >= matrix.size() || col < 0 || col >= matrix[0].size() || matrix[row][col] == 1)
    {
        return false;
    }
    // Check if the cell is not next to another 1
    for (int i = row - 1; i <= row + 1; ++i)
    {
        for (int j = col - 1; j <= col + 1; ++j)
        {
            if (i >= 0 && i < matrix.size() && j >= 0 && j < matrix[0].size() && matrix[i][j] == 1)
            {
                return false;
            }
        }
    }
    return true;
}

// Function to generate the matrix
std::vector<std::vector<int>> generateMatrix(int height, int width)
{
    srand(static_cast<unsigned int>(time(nullptr)));

    std::vector<std::vector<int>> matrix(height, std::vector<int>(width, 2));

    int onesPlaced = 0;
    while (onesPlaced < 5)
    {
        int row = getRandomNumber(0, height - 1);
        int col = getRandomNumber(0, width - 1);
        if (isValidCell(matrix, row, col) && getRandomNumber(1, 10) == 1)
        {
            matrix[row][col] = 1;
            onesPlaced++;
        }
    }

    // Fill the matrix with values
    for (int row = 0; row < height; ++row)
    {
        for (int col = 0; col < width; ++col)
        {
            if (matrix[row][col] != 0 && matrix[row][col] != 1)
            {
                if (getRandomNumber(1, 100) <= 10)
                { // 5% chance for 0
                    matrix[row][col] = 0;
                }
                else
                {
                    matrix[row][col] = row + 2; // default value
                }
            }
        }
    }
    return matrix;
}

// Function to write the matrix to a file with spaces between numbers
void writeMatrixToFile(const std::vector<std::vector<int>> &matrix, const std::string &filename)
{
    std::ofstream file(filename);
    if (file.is_open())
    {
        for (size_t i = 0; i < matrix.size(); ++i)
        {
            for (size_t j = 0; j < matrix[i].size(); ++j)
            {
                file << matrix[i][j];
                if (j != matrix[i].size() - 1)
                { // Add space if not the last column
                    file << " ";
                }
            }
            file << std::endl;
        }
        file.close();
        std::cout << "Matrix written to file: " << filename << std::endl;
    }
    else
    {
        std::cerr << "Unable to open file: " << filename << std::endl;
    }
}

int main()
{
    const int numLevels = 4; // Number of levels to create

    // Define different dimensions for each level
    std::vector<std::pair<int, int>> levelDimensions = {{4, 6}, {5, 8}, {6, 10}, {7, 12}};

    for (int i = 0; i < numLevels; ++i)
    {
        int height = levelDimensions[i].first;
        int width = levelDimensions[i].second;

        std::vector<std::vector<int>> matrix = generateMatrix(height, width);
        std::string filename = "lvl" + std::to_string(i + 1) + ".txt";
        writeMatrixToFile(matrix, filename);
    }

    return 0;
}
