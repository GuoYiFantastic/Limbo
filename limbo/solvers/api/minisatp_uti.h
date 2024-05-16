#include <cstdio>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <array>
#include <vector>
#include <sstream>

std::string exec(const char* cmd)  // cmd 是你要执行的命令
{
    std::array<char, 128> buffer;
    std::string result;  // result 保存命令执行的结果

    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd, "r"), pclose);

    if (!pipe) {
        throw std::runtime_error("popen() failed!");
    }

    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result += buffer.data();
    }

    return result;
}

std::string getEleventhLastLine(const std::string& str) {
    std::vector<std::string> lines;
    std::stringstream ss(str);
    std::string line;

    while (std::getline(ss, line)) {
        lines.push_back(line);
    }

    // 检查是否有足够的行
    if (lines.size() < 11) {
        return "Not enough lines";  // 或者可以返回空字符串 ""
    }

    return lines[lines.size() - 11];
}

std::vector<int> parseSolution(const std::string& input) {
    std::istringstream iss(input);
    std::vector<int> raw_solution;
    std::string token;

    // 找到最大的变量索引以初始化数组大小
    int max_index = 0;
    while (iss >> token) {
        // std::cout << token << std::endl;
        if (token[0] == '-' || token[0] == 'x') {
            int n = token[0] == 'x'? 1:2;
            int index = std::stoi(token.substr(n));
            if (index > max_index) {
                max_index = index;
            }
        }
    }

    // 初始化解决方案数组，默认值为0
    raw_solution.resize(max_index + 1, 0);

    // 重新解析输入以设置正确的值
    iss.clear();
    iss.str(input);
    while (iss >> token) {
        // std::cout << token << std::endl;
        if (token[0] == 'x') { // 正值
            int index = std::stoi(token.substr(1));
            raw_solution[index] = 1;
        } else if (token[0] == '-') { // 负值
            int index = std::stoi(token.substr(2));
            raw_solution[index] = 0;
        }
    }

    return raw_solution;
}

std::vector<int> minisatp(std::string opbpath)
{
    std::string cmd = "minisatp " + opbpath;
    std::string res = exec(cmd.c_str()); // 使用 c_str() 转换为 const char*

    std::string line11 = getEleventhLastLine(res);
    std::vector<int> raw_solution = parseSolution(line11);

    return raw_solution;
}