#pragma once
#include <string>
#include <queue>

enum class PrinterState { IDLE, PRINTING, ERROR, OUT_OF_PAPER };

class Printer {
public:
    Printer();
    void addJob(const std::string& job);
    void processJob();
    void setError(const std::string& errorMsg);
    void refillPaper(int sheets);
    PrinterState getState() const;
    std::string getLastError() const;
    int getPaperCount() const;

private:
    PrinterState state;
    std::queue<std::string> jobQueue;
    std::string lastError;
    int paperCount;
};
