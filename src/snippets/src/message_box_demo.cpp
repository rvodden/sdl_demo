#include <iostream>
#include "message_box.h"
#include "sdl.h"

int main() {
    try {
        // Initialize SDL for message box functionality
        sdl::SDL sdl;
        sdl.initSubSystem(sdl::SDL::kVideo);

        std::cout << "Demonstrating MessageBox builder pattern...\n\n";

        // Example 1: Simple message box with default OK button
        std::cout << "1. Simple information message:\n";
        auto result1 = sdl::MessageBox("Hello", "This is a simple message box!")
            .setType(sdl::MessageBox::Type::Information)
            .show();

        if (result1.success) {
            std::cout << "   User clicked button with ID: " << result1.buttonId << "\n";
        } else {
            std::cout << "   Error: " << result1.error << "\n";
        }

        // Example 2: Warning with custom buttons
        std::cout << "\n2. Warning with custom buttons:\n";
        auto result2 = sdl::MessageBox("Warning", "This action cannot be undone!")
            .addButton("Continue", 1)
            .addButton("Cancel", 0)
            .makeLastButtonCancel()
            .setType(sdl::MessageBox::Type::Warning)
            .show();

        if (result2.success) {
            std::cout << "   User clicked button with ID: " << result2.buttonId << "\n";
            if (result2.buttonId == 1) {
                std::cout << "   User chose to continue.\n";
            } else if (result2.buttonId == 0) {
                std::cout << "   User cancelled the action.\n";
            }
        } else {
            std::cout << "   Error: " << result2.error << "\n";
        }

        // Example 3: Error dialog
        std::cout << "\n3. Error dialog:\n";
        auto result3 = sdl::MessageBox("Critical Error", "A fatal error has occurred!")
            .addButton("Restart", 2)
            .addButton("Exit", 1)
            .makeLastButtonDefault()
            .setType(sdl::MessageBox::Type::Error)
            .show();

        if (result3.success) {
            std::cout << "   User clicked button with ID: " << result3.buttonId << "\n";
            if (result3.buttonId == 2) {
                std::cout << "   User chose to restart.\n";
            } else if (result3.buttonId == 1) {
                std::cout << "   User chose to exit.\n";
            }
        } else {
            std::cout << "   Error: " << result3.error << "\n";
        }

        std::cout << "\nMessageBox demo completed successfully!\n";
        return 0;

    } catch (const std::exception& e) {
        std::cerr << "Exception occurred: " << e.what() << std::endl;
        return 1;
    }
}