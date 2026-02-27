#include "httplib.h"
#include <iostream>
#include <csignal>
#include <string>

// Global server object so the signal handler can see it
httplib::Server svr;

// --- 1. CLEAN TERMINATION HANDLER ---
void handle_sigint(int sig) {
    std::cout << "\n[SHUTDOWN] Closing Alpaca Server..." << std::endl;
    svr.stop(); // This breaks the svr.listen() loop gracefully
}

// Helper to create a standard Alpaca JSON response
std::string alpaca_json(int value, int clientID) {
    return "{\"Value\": " + std::to_string(value) + 
           ", \"ClientTransactionID\": " + std::to_string(clientID) + 
           ", \"ServerTransactionID\": 1, \"ErrorNumber\": 0, \"ErrorMessage\": \"\"}";
}

int main() {
    // Register the clean exit handler
    std::signal(SIGINT, handle_sigint);

    // --- 2. THE REAL ROUTES ---

    // Route: Get Focuser Position
    // Example: GET http://192.168.5.11:8080/api/v1/focuser/0/position?ClientTransactionID=123
    svr.Get("/api/v1/focuser/0/position", [](const httplib::Request &req, httplib::Response &res) {
        int clientID = 0;
        if (req.has_param("ClientTransactionID")) {
            clientID = std::stoi(req.get_param_value("ClientTransactionID"));
        }

        // For now, we return a dummy value of 5000
        res.set_content(alpaca_json(5000, clientID), "application/json");
        std::cout << "[GET] NINA asked for Focuser Position. Replied: 5000" << std::endl;
    });

    // Route: Move Focuser (This is what NINA calls when you click 'Move')
    svr.Put("/api/v1/focuser/0/move", [](const httplib::Request &req, httplib::Response &res) {
        int clientID = 0;
        if (req.has_param("ClientTransactionID")) {
            clientID = std::stoi(req.get_param_value("ClientTransactionID"));
        }
        
        // NINA sends the target in the "Position" parameter
        std::string target = req.get_param_value("Position");
        
        std::cout << "[PUT] NINA requested Move to: " << target << std::endl;
        res.set_content(alpaca_json(0, clientID), "application/json");
    });

    std::cout << "Real Alpaca Server running on http://192.168.5.11:8080" << std::endl;
    std::cout << "Press Ctrl+C to shut down cleanly." << std::endl;

    svr.listen("0.0.0.0", 8080);

    std::cout << "[EXIT] Program terminated safely." << std::endl;
    return 0;
}