#include <gtest/gtest.h>
#include <array>
#include <fstream>
#include <memory>
#include <fcntl.h>        
#include <unistd.h>      
#include <filesystem>     
#include <thread>         
#include <chrono>         
#include <sys/mman.h>     
#include <sys/stat.h>     
#include <string.h>

extern "C" {
    #include <utils.h>
    #include <parent.h>
}

TEST(CalculateSumTest, HandlesValidInput) {
    const char *input = "10 20 30";
    int result = calculate_sum(input);
    EXPECT_EQ(result, 60);  
}

TEST(CalculateSumTest, HandlesInvalidCharacters) {
    const char *input = "10 abc 30";
    int result = calculate_sum(input);
    EXPECT_EQ(result, 40);  
}

TEST(CalculateSumTest, HandlesNegativeNumbers) {
    const char *input = "-10 20 -30";
    int result = calculate_sum(input);
    EXPECT_EQ(result, -20);  
}


TEST(TestParent, HandlesSharedMemoryCommunication) {
    const char* fileWithInput = "input.txt";
    const std::vector<std::string> numberGroup = {"10 20 30"};
    const std::vector<int> expectedSums = {60};

    const char* tempInputFile = "temp_input.txt";
    {
        auto tempInFile = std::ofstream(tempInputFile);
        tempInFile << fileWithInput << "\n";
        for (const auto& numbers : numberGroup) {
            tempInFile << numbers << "\n";
        }
    }

    int original_stdin = dup(STDIN_FILENO);
    int temp_in_fd = open(tempInputFile, O_RDONLY);
    dup2(temp_in_fd, STDIN_FILENO);
    close(temp_in_fd);

    std::ofstream clearFile(fileWithInput, std::ios::trunc);
    clearFile.close();

    Parent("../LW3/child", stdin);

    dup2(original_stdin, STDIN_FILENO);
    close(original_stdin);

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    std::ifstream resultFile(fileWithInput);
    ASSERT_TRUE(resultFile.good()) << "Файл input.txt не создан.";

    std::string line;
    ASSERT_TRUE(std::getline(resultFile, line)) << "Недостаточно строк в файле input.txt.";
    std::string expectedLine = "Сумма: " + std::to_string(expectedSums[0]);
    ASSERT_EQ(line, expectedLine) << "Неверная сумма в файле input.txt: ожидалось \"" << expectedLine << "\", но получено \"" << line << "\"";

    std::filesystem::remove(tempInputFile);
    std::filesystem::remove(fileWithInput);
}

