#include "printer.h"
#include <iostream>

Printer::Printer() : state(PrinterState::IDLE), paperCount(10) {}

void Printer::addJob(const std::string& job) {
    jobQueue.push(job);
}

void Printer::processJob() {
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
