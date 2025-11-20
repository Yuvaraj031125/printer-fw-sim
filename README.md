# printer-fw-sim

A C++ project that simulates printer firmware behavior for testing and CI/CD demonstration purposes. Includes modular source code, unit tests with GoogleTest, and integrated pipelines for Jenkins and GitHub Actions. Supports build, test, coverage, packaging, and deployment workflows using CMake and modern CI tools.



\## Features

\- Manage print jobs (queue)

\- Handle states: Idle, Printing, Error, Out of Paper

\- Error simulation (paper jam, low ink)

\- Unit tests with GoogleTest

\- CI/CD pipeline with Jenkins



\## Build \& Run

```bash

mkdir build \&\& cd build

cmake ..

make

./printer



