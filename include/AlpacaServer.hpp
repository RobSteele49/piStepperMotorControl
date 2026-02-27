#ifndef ALPACA_SERVER_HPP
#define ALPACA_SERVER_HPP

#include "httplib.h"
#include "WaveShareStepper.hpp"
#include "config.h"
#include <thread>
#include <iostream>

class AlpacaServer {
public:
    AlpacaServer(WaveShareStepper* foc, WaveShareStepper* rot) 
        : focuser(foc), rotator(rot), running(false) {}

    void start(int port = 8080) {
        running = true;
        server_thread = std::thread(&AlpacaServer::run, this, port);
    }

    void stop() {
        running = false;
        svr.stop();
        if (server_thread.joinable()) server_thread.join();
    }

private:
    WaveShareStepper* focuser;
    WaveShareStepper* rotator;
    httplib::Server svr;
    std::thread server_thread;
    bool running;

    void run(int port) {


      // 1. Management: Description (Tells ASCOM who made this)
      svr.Get("/management/v1/description", [&](const httplib::Request& req, httplib::Response& res) {
	res.set_content("{\"Value\":{\"ServerName\":\"Pi-LX200-Focuser\",\"Manufacturer\":\"Steele-Astronomy\",\"ManufacturerVersion\":\"1.0\",\"Location\":\"Observatory\"},\"ClientTransactionID\":0,\"ServerTransactionID\":1,\"ErrorNumber\":0,\"ErrorMessage\":\"\"}", "application/json");
});

      // 2. Management: Configured Devices (Tells ASCOM what is plugged in)
      svr.Get("/management/v1/configureddevices", [&](const httplib::Request& req, httplib::Response& res) {
	res.set_content("{\"Value\":[{\"DeviceName\":\"Dual Controller Focuser\",\"DeviceType\":\"Focuser\",\"DeviceNumber\":0,\"UniqueID\":\"pi-foc-01\"},{\"DeviceName\":\"Dual Controller Rotator\",\"DeviceType\":\"Rotator\",\"DeviceNumber\":0,\"UniqueID\":\"pi-rot-01\"}],\"ClientTransactionID\":0,\"ServerTransactionID\":1,\"ErrorNumber\":0,\"ErrorMessage\":\"\"}", "application/json");
});

      // 3. Device: Connected (NINA always checks this first)
      svr.Get("/api/v1/focuser/0/connected", [&](const httplib::Request& req, httplib::Response& res) {
	res.set_content("{\"Value\":true,\"ClientTransactionID\":0,\"ServerTransactionID\":1,\"ErrorNumber\":0,\"ErrorMessage\":\"\"}", "application/json");
      });
      
        // --- FOCUSER ROUTES ---
        svr.Get("/api/v1/focuser/0/position", [&](const httplib::Request& req, httplib::Response& res) {
            long long pos = focuser->getCurrentPosition();
            std::cout << "[NET] Focuser Position Request -> " << pos << std::endl;
            res.set_content(formatResponse(pos, req), "application/json");
        });

        svr.Put("/api/v1/focuser/0/move", [&](const httplib::Request& req, httplib::Response& res) {
            if (req.has_param("Position")) {
                long long target = std::stoll(req.get_param_value("Position"));
                std::cout << "[NET] Focuser Move Command -> " << target << std::endl;
                focuser->moveTo(target, FOC_SPEED_MED);
            }
            res.set_content(formatResponse(0, req), "application/json");
        });

        // --- ROTATOR ROUTES ---
        svr.Get("/api/v1/rotator/0/position", [&](const httplib::Request& req, httplib::Response& res) {
            long long pos = rotator->getCurrentPosition();
            std::cout << "[NET] Rotator Position Request -> " << pos << std::endl;
            res.set_content(formatResponse(pos, req), "application/json");
        });

        svr.Put("/api/v1/rotator/0/move", [&](const httplib::Request& req, httplib::Response& res) {
            if (req.has_param("Position")) {
                long long target = std::stoll(req.get_param_value("Position"));
                std::cout << "[NET] Rotator Move Command -> " << target << std::endl;
                rotator->moveTo(target, ROT_SPEED_MED);
            }
            res.set_content(formatResponse(0, req), "application/json");
        });

        std::cout << "\n[SUCCESS] Alpaca Server active on http://192.168.5.11:" << port << std::endl;
        
        if (!svr.listen("0.0.0.0", port)) {
            std::cerr << "[ERROR] Alpaca Server failed to bind to port " << port << "!" << std::endl;
        }
    }

    std::string formatResponse(long long val, const httplib::Request& req) {
        std::string clientID = req.has_param("ClientTransactionID") ? req.get_param_value("ClientTransactionID") : "0";
        return "{\"Value\":" + std::to_string(val) + ",\"ClientTransactionID\":" + clientID + ",\"ErrorNumber\":0,\"ErrorMessage\":\"\"}";
    }
};

#endif
