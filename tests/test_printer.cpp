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

// Test: Printing with no jobs should keep printer IDLE
TEST(PrinterTest, NoJobsKeepsIdle) {
    Printer printer;
    printer.processJob();
    EXPECT_EQ(printer.getState(), PrinterState::IDLE);
}

// Test: Refilling paper increases paper count
TEST(PrinterTest, RefillPaperIncreasesCount) {
    Printer printer;
    int initial = printer.getPaperCount();
    printer.refillPaper(5);
    EXPECT_EQ(printer.getPaperCount(), initial + 5);
}

// Test: Printer returns to IDLE after printing
TEST(PrinterTest, ReturnsToIdleAfterPrint) {
    Printer printer;
    printer.addJob("Job1");
    printer.processJob();
    EXPECT_EQ(printer.getState(), PrinterState::IDLE);
}

// Test: Multiple jobs are processed correctly
TEST(PrinterTest, MultipleJobsProcessed) {
    Printer printer;
    printer.refillPaper(10);
    printer.addJob("Job1");
    printer.addJob("Job2");
    int initial = printer.getPaperCount();
    printer.processJob();
    printer.processJob();
    EXPECT_EQ(printer.getPaperCount(), initial - 2);
    EXPECT_EQ(printer.getState(), PrinterState::IDLE);
}

// Test: Adding multiple jobs increases queue size
TEST(PrinterTest, AddMultipleJobsIncreasesQueue) {
    Printer printer;
    printer.addJob("Job1");
    printer.addJob("Job2");
    printer.addJob("Job3");
    int initial = printer.getPaperCount();
    printer.processJob();
    printer.processJob();
    printer.processJob();
    EXPECT_EQ(printer.getPaperCount(), initial - 3);
}

// Test: Processing job with no paper does not decrease paper count
TEST(PrinterTest, NoPaperDoesNotPrint) {
    Printer printer;
    printer.refillPaper(-printer.getPaperCount()); // empty paper
    printer.addJob("Job1");
    int initial = printer.getPaperCount();
    printer.processJob();
    EXPECT_EQ(printer.getPaperCount(), initial);
    EXPECT_EQ(printer.getState(), PrinterState::OUT_OF_PAPER);
}

// Test: Setting error prevents printing
TEST(PrinterTest, ErrorPreventsPrinting) {
    Printer printer;
    printer.setError("Jam");
    printer.addJob("Job1");
    int initial = printer.getPaperCount();
    printer.processJob();
    EXPECT_EQ(printer.getPaperCount(), initial); // Should not print
    EXPECT_EQ(printer.getState(), PrinterState::ERROR);
}

// Test: Refilling paper after out-of-paper allows printing
TEST(PrinterTest, RefillAfterOutOfPaperAllowsPrint) {
    Printer printer;
    printer.refillPaper(-printer.getPaperCount()); // empty paper
    printer.addJob("Job1");
    printer.processJob();
    EXPECT_EQ(printer.getState(), PrinterState::OUT_OF_PAPER);
    printer.refillPaper(5);
    printer.processJob();
    EXPECT_EQ(printer.getState(), PrinterState::IDLE);
    EXPECT_EQ(printer.getPaperCount(), 4);
}

// Test: getLastError returns empty string by default
TEST(PrinterTest, DefaultLastErrorIsEmpty) {
    Printer printer;
    EXPECT_EQ(printer.getLastError(), "");
}

