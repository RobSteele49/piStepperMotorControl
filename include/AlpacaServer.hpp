/*
 * Project:    LX200 Focuser Automation
 * Component:  WaveShare Stepper Driver Header
 * File:       AlpacaServer.hpp
 * Author:     Robert D. Steele
 * Date:       2026-02-23
 * Version:    2.0 (Update 2/28/23 from Gemini)
 * Copyright (c) 2026 Robert D. Steele. All Rights Reserved.
 */

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
        // --- MANAGEMENT ROUTES ---
        svr.Get("/management/v1/description", [&](const httplib::Request& req, httplib::Response& res) {
            res.set_content("{\"Value\":{\"ServerName\":\"Pi-LX200-Focuser\",\"Manufacturer\":\"Steele-Astronomy\",\"ManufacturerVersion\":\"1.0\",\"Location\":\"Observatory\"},\"ClientTransactionID\":0,\"ServerTransactionID\":1,\"ErrorNumber\":0,\"ErrorMessage\":\"\"}", "application/json");
        });

        svr.Get("/management/v1/configureddevices", [&](const httplib::Request& req, httplib::Response& res) {
            res.set_content("{\"Value\":[{\"DeviceName\":\"Dual Controller Focuser\",\"DeviceType\":\"Focuser\",\"DeviceNumber\":0,\"UniqueID\":\"pi-foc-01\"},{\"DeviceName\":\"Dual Controller Rotator\",\"DeviceType\":\"Rotator\",\"DeviceNumber\":0,\"UniqueID\":\"pi-rot-01\"}],\"ClientTransactionID\":0,\"ServerTransactionID\":1,\"ErrorNumber\":0,\"ErrorMessage\":\"\"}", "application/json");
        });

        // --- FOCUSER MANDATORY ROUTES ---
        svr.Get("/api/v1/focuser/0/connected", [&](const httplib::Request& req, httplib::Response& res) {
            res.set_content(formatBoolResponse(true, req), "application/json");
        });

        svr.Get("/api/v1/focuser/0/absolute", [&](const httplib::Request& req, httplib::Response& res) {
            res.set_content(formatBoolResponse(true, req), "application/json"); // Yes, we are absolute
        });

        svr.Get("/api/v1/focuser/0/maxstep", [&](const httplib::Request& req, httplib::Response& res) {
            res.set_content(formatResponse(FOC_LIMIT_MAX, req), "application/json");
        });

        svr.Get("/api/v1/focuser/0/position", [&](const httplib::Request& req, httplib::Response& res) {
            res.set_content(formatResponse(focuser->getCurrentPosition(), req), "application/json");
        });

        svr.Get("/api/v1/focuser/0/ismoving", [&](const httplib::Request& req, httplib::Response& res) {
            res.set_content(formatBoolResponse(false, req), "application/json"); // Simplification: assume moves are instant for now
        });

        svr.Put("/api/v1/focuser/0/move", [&](const httplib::Request& req, httplib::Response& res) {
            if (req.has_param("Position")) {
                long long target = std::stoll(req.get_param_value("Position"));
                std::cout << "[NET] Focuser Move -> " << target << std::endl;
                focuser->moveTo(target, FOC_SPEED_MED);
            }
            res.set_content(formatResponse(0, req), "application/json");
        });

        // --- ROTATOR MANDATORY ROUTES ---
        svr.Get("/api/v1/rotator/0/connected", [&](const httplib::Request& req, httplib::Response& res) {
            res.set_content(formatBoolResponse(true, req), "application/json");
        });

        svr.Get("/api/v1/rotator/0/position", [&](const httplib::Request& req, httplib::Response& res) {
            res.set_content(formatResponse(rotator->getCurrentPosition(), req), "application/json");
        });

        svr.Put("/api/v1/rotator/0/moveabsolute", [&](const httplib::Request& req, httplib::Response& res) {
            if (req.has_param("Position")) {
                long long target = std::stoll(req.get_param_value("Position"));
                rotator->moveTo(target, ROT_SPEED_MED);
            }
            res.set_content(formatResponse(0, req), "application/json");
        });

        std::cout << "\n[SUCCESS] Alpaca Server active on port " << port << std::endl;
        
        if (!svr.listen("0.0.0.0", port)) {
            std::cerr << "[ERROR] Port " << port << " binding failed!" << std::endl;
        }
    }

    // Helper for Numbers
    std::string formatResponse(long long val, const httplib::Request& req) {
        std::string clientID = req.has_param("ClientTransactionID") ? req.get_param_value("ClientTransactionID") : "0";
        return "{\"Value\":" + std::to_string(val) + ",\"ClientTransactionID\":" + clientID + ",\"ErrorNumber\":0,\"ErrorMessage\":\"\"}";
    }

    // Helper for Booleans (True/False)
    std::string formatBoolResponse(bool val, const httplib::Request& req) {
        std::string clientID = req.has_param("ClientTransactionID") ? req.get_param_value("ClientTransactionID") : "0";
        return "{\"Value\":" + std::string(val ? "true" : "false") + ",\"ClientTransactionID\":" + clientID + ",\"ErrorNumber\":0,\"ErrorMessage\":\"\"}";
    }
};

#endif
