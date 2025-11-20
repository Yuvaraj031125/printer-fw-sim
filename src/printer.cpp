#include "printer.h"
#include <iostream>

Printer::Printer() : state(PrinterState::IDLE), paperCount(10) {}

void Printer::addJob(const std::string& job) {
    jobQueue.push(job);
}

void Printer::processJob() {
    if (state == PrinterState::ERROR) {
        return;
    }
    if (paperCount <= 0) {
        state = PrinterState::OUT_OF_PAPER;
        return;
    }
    if (jobQueue.empty()) {
        state = PrinterState::IDLE;
        return;
    }
    state = PrinterState::PRINTING;
    std::string job = jobQueue.front();
    jobQueue.pop();
    paperCount--;
    std::cout << "Printing: " << job << std::endl;
    state = PrinterState::IDLE;
}
int Printer::getPaperCount() const { return paperCount; }
// cppcheck-suppress unusedFunction
void Printer::setError(const std::string& errorMsg) {
    state = PrinterState::ERROR;
    lastError = errorMsg;
}

// cppcheck-suppress unusedFunction
void Printer::refillPaper(int sheets) {
    paperCount += sheets;
    if (state == PrinterState::OUT_OF_PAPER) state = PrinterState::IDLE;
}

// cppcheck-suppress unusedFunction
PrinterState Printer::getState() const {
    return state;
}

// cppcheck-suppress unusedFunction
std::string Printer::getLastError() const {
    return lastError;
}
