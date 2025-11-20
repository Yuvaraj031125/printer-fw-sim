#include "printer.h"
#include <iostream>

int main() {
    Printer printer;
    printer.addJob("Document1.pdf");
    printer.addJob("Report.docx");

    printer.processJob();
    printer.processJob();

    std::cout << "Remaining paper: " << printer.getPaperCount() << std::endl;
    return 0;
}
