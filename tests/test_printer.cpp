#include "printer.h"
#include <gtest/gtest.h>

TEST(PrinterTest, PaperDecreasesAfterPrint) {
    Printer printer;
    int initial = printer.getPaperCount();
    printer.addJob("TestJob");
    printer.processJob();
    EXPECT_EQ(printer.getPaperCount(), initial - 1);
}

TEST(PrinterTest, OutOfPaperState) {
    Printer printer;
    printer.refillPaper(-printer.getPaperCount()); // empty paper
    printer.addJob("Job");
    printer.processJob();
    EXPECT_EQ(printer.getState(), PrinterState::OUT_OF_PAPER);
}

TEST(PrinterTest, ErrorState) {
    Printer printer;
    printer.setError("Paper Jam");
    EXPECT_EQ(printer.getState(), PrinterState::ERROR);
    EXPECT_EQ(printer.getLastError(), "Paper Jam");
}
