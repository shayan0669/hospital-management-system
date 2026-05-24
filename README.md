# Hospital Management System

A robust, object-oriented desktop application built in C++ to efficiently manage hospital daily operations. This system handles doctor directories, patient appointment scheduling, and billing management, providing a seamless workflow integrated with a Qt-based graphical user interface.

## Overview

This project was developed to apply core Object-Oriented Programming (OOP) principles and custom data structures to a real-world scenario. By avoiding standard library containers for the core logic where appropriate, the system demonstrates a deep understanding of memory management, algorithmic efficiency, and software architecture. 

### Key Features
* **Doctor Management:** Add, update, and search for doctors by ID, specialization, and consultation fees.
* **Appointment Scheduling:** Book patient appointments into available time slots without double-booking conflicts.
* **Billing System:** Generate and track patient consultation charges with accurate timestamping.
* **Data Persistence:** Seamlessly save and load all records (doctors, appointments, billing) to and from local CSV/text files.

## Architecture & Data Structures

The system's backend is entirely custom-built, utilizing specific data structures to optimize performance for different tasks:

* **Binary Search Tree (BST):** Used in the `DoctorManager` to store doctor profiles. This allows for highly efficient $O(\log n)$ search, insertion, and retrieval based on the Doctor ID.
* **Stacks (LIFO):** Implemented in the `BillingStack` to manage financial transactions. The most recent bills are kept at the top for quick access and end-of-day auditing.
* **Linked Lists:** Utilized in the `ApptManager` to maintain a sequential, dynamic list of patient appointments, allowing for flexible memory allocation without fixed capacities.

## Software Design

*Feel free to add your UML class diagrams, sequence diagrams, or swimlanes here to showcase your understanding of the system development life cycle and software analysis.*

## Build & Installation

This project is configured to be fully cross-platform, supporting both MSVC (Windows) and CMake build systems, making it easy to run on a primary Windows host or an Ubuntu virtual machine.

### Prerequisites
* C++17 Compiler
* Qt 5 or Qt 6 (Widgets module)
* Visual Studio (for Windows) OR CMake (for Linux/cross-platform)

### Compiling with Visual Studio (Windows)
1. Open `Hospital_Management_System.sln` in Visual Studio.
2. Ensure the Qt VS Tools extension is installed and pointing to your local Qt directory.
3. Build and run the solution.

### Compiling with CMake (Cross-Platform)
```bash
mkdir build
cd build
cmake ..
make
./hospital_qt
