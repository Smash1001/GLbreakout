#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <string>

class lvlGen 
{
public:
	void makeRand();
private:
	int getRandomNumber(int min, int max);
	bool isValidCell(const std::vector<std::vector<int>>& matrix, int row, int col);
	std::vector<std::vector<int>> generateMatrix(int height, int width);
	void writeMatrixToFile(const std::vector<std::vector<int>>& matrix, const std::string& filename);

};