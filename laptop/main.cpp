#include <iostream>
#include <thread>
#include <atomic>
#include <chrono>
#include <Windows.h>

#include "dma/memprocfs_wrapper.h"
#include "../shared/math/vector.h"
#include "../shared/network/protocol.h"

using namespace phaethon;

std::atomic<bool> g_running{ true };

BOOL WINAPI ConsoleHandler(DWORD signal) {
    if (signal == CTRL_C_EVENT || signal == CTRL_CLOSE_EVENT) {
        std::cout << "\n[!] Shutdown signal received...\n";
        g_running = false;
        return TRUE;
    }
    return FALSE;
}

void PrintBanner() {
    std::cout << R"(
    ____  __  _____    __________ __  ______  _   __
   / __ \/ / / /   |  / ____/_  // / / / __ \/ | / /
  / /_/ / /_/ / /| | / __/   / // /_/ / / / /  |/ /
 / ____/ __  / ___ |/ /___  / /__  __/ /_/ / /|  /
/_/   /_/ /_/_/  |_/_____/ /____/_/  \____/_/ |_/

)" << "\n";
    std::cout << "  \"Know your limits\"\n";
    std::cout << "  DMA Framework v1.0\n";
    std::cout << "  ==========================================\n\n";
}

int main() {
    // Setup console
    SetConsoleTitleA("__phaethon__ - DMA Client");
    SetConsoleCtrlHandler(ConsoleHandler, TRUE);

    PrintBanner();

    // Initialize DMA
    std::cout << "[*] Initializing MemProcFS...\n";
    dma::MemProcFS dma;

    if (!dma.Initialize("fpga")) {
        std::cerr << "[!] Failed to initialize MemProcFS\n";
        std::cerr << "    Make sure:\n";
        std::cerr << "      1. DMA card is connected to desktop\n";
        std::cerr << "      2. Laptop is connected to DMA card (DATA port)\n";
        std::cerr << "      3. FTDI drivers are installed\n";
        std::cerr << "\n    Press any key to exit...\n";
        std::cin.get();
        return 1;
    }

    std::cout << "[+] MemProcFS initialized successfully\n\n";

    // Try to attach to CS2
    std::cout << "[*] Looking for CS2 process...\n";

    bool attached = false;
    for (int attempt = 0; attempt < 30 && g_running; attempt++) {
        if (dma.AttachToProcess("cs2.exe")) {
            attached = true;
            break;
        }

        std::cout << "    Attempt " << (attempt + 1) << "/30 - CS2 not found, retrying...\n";
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    if (!attached) {
        std::cerr << "[!] Failed to find CS2 process\n";
        std::cerr << "    Make sure CS2 is running on the desktop\n";
        std::cerr << "\n    Press any key to exit...\n";
        std::cin.get();
        dma.Close();
        return 1;
    }

    std::cout << "[+] Attached to CS2 (PID: " << dma.GetPID() << ")\n\n";

    // Get module information
    std::cout << "[*] Loading module information...\n";
    dma.RefreshModuleCache();

    auto clientModule = dma.GetModule("client.dll");
    auto engineModule = dma.GetModule("engine2.dll");

    if (clientModule.base_address == 0) {
        std::cerr << "[!] Failed to find client.dll\n";
        std::cerr << "    CS2 may not be fully loaded yet\n";
        std::cerr << "\n    Press any key to exit...\n";
        std::cin.get();
        dma.Close();
        return 1;
    }

    std::cout << "[+] Modules loaded:\n";
    std::cout << "    client.dll  : 0x" << std::hex << clientModule.base_address << std::dec << " (" << clientModule.size << " bytes)\n";
    if (engineModule.base_address != 0) {
        std::cout << "    engine2.dll : 0x" << std::hex << engineModule.base_address << std::dec << " (" << engineModule.size << " bytes)\n";
    }
    std::cout << "\n";

    // Test scatter-gather read
    std::cout << "[*] Testing scatter-gather memory operations...\n";
    auto scatter = dma.CreateScatter();
    if (scatter && scatter->IsValid()) {
        std::cout << "[+] Scatter-gather system ready\n";
    } else {
        std::cerr << "[!] Failed to initialize scatter-gather\n";
    }
    std::cout << "\n";

    // Main loop
    std::cout << "[+] Entering main loop (Press Ctrl+C to exit)\n";
    std::cout << "==========================================\n\n";

    auto startTime = std::chrono::high_resolution_clock::now();
    int loopCount = 0;
    int fpsCounter = 0;
    auto lastFpsUpdate = std::chrono::high_resolution_clock::now();

    while (g_running) {
        auto loopStart = std::chrono::high_resolution_clock::now();

        try {
            // TODO: Read game state here
            // - Entity list
            // - Local player
            // - View matrix
            // - Player data

            // For now, just demonstrate memory reading
            if (loopCount % 100 == 0) {
                // Test read (read first 4 bytes of client.dll as validation)
                uint32_t magic = dma.Read<uint32_t>(clientModule.base_address);

                // Should be PE signature (0x5A4D = "MZ")
                if ((magic & 0xFFFF) == 0x5A4D) {
                    // Successfully reading memory
                }
            }

            loopCount++;
            fpsCounter++;

            // Calculate FPS
            auto now = std::chrono::high_resolution_clock::now();
            auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastFpsUpdate);

            if (elapsed.count() >= 1000) {
                int fps = fpsCounter;
                fpsCounter = 0;
                lastFpsUpdate = now;

                auto runtime = std::chrono::duration_cast<std::chrono::seconds>(now - startTime);
                std::cout << "\r[*] Runtime: " << runtime.count() << "s | Loop FPS: " << fps << " | Total Iterations: " << loopCount << "    " << std::flush;
            }

            // Target ~100 Hz
            std::this_thread::sleep_for(std::chrono::milliseconds(10));

        } catch (const std::exception& e) {
            std::cerr << "\n[!] Error in main loop: " << e.what() << "\n";
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }

    std::cout << "\n\n[*] Shutting down...\n";
    dma.Close();
    std::cout << "[+] Clean shutdown complete\n";

    return 0;
}
