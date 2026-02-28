/*
 * Project:    LX200 Focuser Automation
 * Component:  WaveShare Stepper Driver Header
 * File:       AlpacaServer.hpp
 * Author:     Robert D. Steele
 * Date:       2026-02-23
 * Version:    2.1 (Update 2/28/26 from Gemini - Case Insensitivity Fix)
 * Copyright (c) 2026 Robert D. Steele. All Rights Reserved.
 */

#ifndef ALPACA_SERVER_HPP
#define ALPACA_SERVER_HPP

#include "httplib.h"
#include "WaveShareStepper.hpp"
#include "config.h"
#include <thread>
#include <iostream>
#include <string>

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

        // --- FOCUSER MANDATORY ROUTES (With Case-Insensitivity Aliases) ---
        
        // Connected Property
        auto foc_conn = [&](const httplib::Request& req, httplib::Response& res) {
            res.set_content(formatBoolResponse(true, req), "application/json");
        };
        svr.Get("/api/v1/focuser/0/connected", foc_conn);
        svr.Get("/api/v1/focuser/0/Connected", foc_conn);
        svr.Put("/api/v1/focuser/0/connected", foc_conn);
        svr.Put("/api/v1/focuser/0/Connected", foc_conn);

        // Absolute Property
        auto foc_abs = [&](const httplib::Request& req, httplib::Response& res) {
            res.set_content(formatBoolResponse(true, req), "application/json");
        };
        svr.Get("/api/v1/focuser/0/absolute", foc_abs);
        svr.Get("/api/v1/focuser/0/Absolute", foc_abs);

        // Position Property
        auto foc_pos = [&](const httplib::Request& req, httplib::Response& res) {
            res.set_content(formatResponse(focuser->getCurrentPosition(), req), "application/json");
        };
        svr.Get("/api/v1/focuser/0/position", foc_pos);
        svr.Get("/api/v1/focuser/0/Position", foc_pos);

        // MaxStep Property
        auto foc_max = [&](const httplib::Request& req, httplib::Response& res) {
            res.set_content(formatResponse(FOC_LIMIT_MAX, req), "application/json");
        };
        svr.Get("/api/v1/focuser/0/maxstep", foc_max);
        svr.Get("/api/v1/focuser/0/MaxStep", foc_max);

        // IsMoving Property
        auto foc_mov = [&](const httplib::Request& req, httplib::Response& res) {
            res.set_content(formatBoolResponse(false, req), "application/json");
        };
        svr.Get("/api/v1/focuser/0/ismoving", foc_mov);
        svr.Get("/api/v1/focuser/0/IsMoving", foc_mov);

        // Move Command
        auto foc_move_cmd = [&](const httplib::Request& req, httplib::Response& res) {
            if (req.has_param("Position")) {
                long long target = std::stoll(req.get_param_value("Position"));
                std::cout << "[NET] Focuser Move -> " << target << std::endl;
                focuser->moveTo(target, FOC_SPEED_MED);
            }
            res.set_content(formatResponse(0, req), "application/json");
        };
        svr.Put("/api/v1/focuser/0/move", foc_move_cmd);
        svr.Put("/api/v1/focuser/0/Move", foc_move_cmd);


        // --- ROTATOR MANDATORY ROUTES (With Case-Insensitivity Aliases) ---

        // Connected Property
        auto rot_conn = [&](const httplib::Request& req, httplib::Response& res) {
            res.set_content(formatBoolResponse(true, req), "application/json");
        };
        svr.Get("/api/v1/rotator/0/connected", rot_conn);
        svr.Get("/api/v1/rotator/0/Connected", rot_conn);
        svr.Put("/api/v1/rotator/0/connected", rot_conn);
        svr.Put("/api/v1/rotator/0/Connected", rot_conn);

        // Position Property
        auto rot_pos = [&](const httplib::Request& req, httplib::Response& res) {
            res.set_content(formatResponse(rotator->getCurrentPosition(), req), "application/json");
        };
        svr.Get("/api/v1/rotator/0/position", rot_pos);
        svr.Get("/api/v1/rotator/0/Position", rot_pos);

        // MoveAbsolute Command
        auto rot_move_cmd = [&](const httplib::Request& req, httplib::Response& res) {
            if (req.has_param("Position")) {
                long long target = std::stoll(req.get_param_value("Position"));
                std::cout << "[NET] Rotator Move -> " << target << std::endl;
                rotator->moveTo(target, ROT_SPEED_MED);
            }
            res.set_content(formatResponse(0, req), "application/json");
        };
        svr.Put("/api/v1/rotator/0/moveabsolute", rot_move_cmd);
        svr.Put("/api/v1/rotator/0/MoveAbsolute", rot_move_cmd);

        std::cout << "\n[SUCCESS] Alpaca Server active on http://192.168.5.11:" << port << std::endl;
        
        if (!svr.listen("0.0.0.0", port)) {
            std::cerr << "[ERROR] Alpaca Server failed to bind to port " << port << "!" << std::endl;
        }
    }

    // Helper for Numbers
    std::string formatResponse(long long val, const httplib::Request& req) {
        std::string clientID = req.has_param("ClientTransactionID") ? req.get_param_value("ClientTransactionID") : "0";
        return "{\"Value\":" + std::to_string(val) + ",\"ClientTransactionID\":" + clientID + ",\"ErrorNumber\":0,\"ErrorMessage\":\"\"}";
    }

    // Helper for Booleans
    std::string formatBoolResponse(bool val, const httplib::Request& req) {
        std::string clientID = req.has_param("ClientTransactionID") ? req.get_param_value("ClientTransactionID") : "0";
        return "{\"Value\":" + std::string(val ? "true" : "false") + ",\"ClientTransactionID\":" + clientID + ",\"ErrorNumber\":0,\"ErrorMessage\":\"\"}";
    }
};

#endif