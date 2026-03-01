/*
 * Project:    LX200 Focuser Automation
 * Component:  WaveShare Stepper Driver Header
 * File:       AlpacaServer.hpp
 * Author:     Robert D. Steele
 * Date:       2026-02-23
 * Version:    2.4 (Fix halting issue)
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

	// Name Property
        auto foc_name = [&](const httplib::Request& req, httplib::Response& res) {
            res.set_content("{\"Value\":\"Dual-Controller Focuser\",\"ClientTransactionID\":0,\"ErrorNumber\":0,\"ErrorMessage\":\"\"}", "application/json");
        };
        svr.Get("/api/v1/focuser/0/name", foc_name);
        svr.Get("/api/v1/focuser/0/Name", foc_name);

        // Description Property
        auto foc_desc = [&](const httplib::Request& req, httplib::Response& res) {
            res.set_content("{\"Value\":\"LX200 Stepper Controller via Raspberry Pi\",\"ClientTransactionID\":0,\"ErrorNumber\":0,\"ErrorMessage\":\"\"}", "application/json");
        };
        svr.Get("/api/v1/focuser/0/description", foc_desc);
        svr.Get("/api/v1/focuser/0/Description", foc_desc);

        // DriverInfo Property
        auto foc_info = [&](const httplib::Request& req, httplib::Response& res) {
            res.set_content("{\"Value\":\"Built by Robert Steele - v2.1\",\"ClientTransactionID\":0,\"ErrorNumber\":0,\"ErrorMessage\":\"\"}", "application/json");
        };
        svr.Get("/api/v1/focuser/0/driverinfo", foc_info);
        svr.Get("/api/v1/focuser/0/DriverInfo", foc_info);

        // DriverVersion Property
        auto foc_ver = [&](const httplib::Request& req, httplib::Response& res) {
            res.set_content("{\"Value\":\"2.1\",\"ClientTransactionID\":0,\"ErrorNumber\":0,\"ErrorMessage\":\"\"}", "application/json");
        };
        svr.Get("/api/v1/focuser/0/driverversion", foc_ver);
        svr.Get("/api/v1/focuser/0/DriverVersion", foc_ver);

        // InterfaceVersion (Should return 3 for modern Alpaca)
        auto foc_iface = [&](const httplib::Request& req, httplib::Response& res) {
            res.set_content("{\"Value\":3,\"ClientTransactionID\":0,\"ErrorNumber\":0,\"ErrorMessage\":\"\"}", "application/json");
        };
        svr.Get("/api/v1/focuser/0/interfaceversion", foc_iface);
        svr.Get("/api/v1/focuser/0/InterfaceVersion", foc_iface);
	
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

        // NEW: StepSize Property
        auto foc_stepsize = [&](const httplib::Request& req, httplib::Response& res) {
            // 1.0 tells ASCOM 1 step = 1 unit. No weird scaling.
            res.set_content(formatResponse(1, req), "application/json");
        };
        svr.Get("/api/v1/focuser/0/stepsize", foc_stepsize);
        svr.Get("/api/v1/focuser/0/StepSize", foc_stepsize);

        // NEW: MaxIncrement Property
        auto foc_maxinc = [&](const httplib::Request& req, httplib::Response& res) {
            // This tells ASCOM "Don't ever try to move more than 10k steps in one jump"
            res.set_content(formatResponse(10000, req), "application/json");
        };
        svr.Get("/api/v1/focuser/0/maxincrement", foc_maxinc);
        svr.Get("/api/v1/focuser/0/MaxIncrement", foc_maxinc);

        // ... (existing position, ismoving routes) ...
	
        // IsMoving Property
	auto foc_mov = [&](const httplib::Request& req, httplib::Response& res) {
            res.set_content(formatBoolResponse(focuser->isMoving(), req), "application/json");
        };
	
        svr.Get("/api/v1/focuser/0/ismoving", foc_mov);
        svr.Get("/api/v1/focuser/0/IsMoving", foc_mov);

        // Move Command
        // Update this in Focuser Move and Rotator Move absolute

	// Updated Move Command (Threaded)
	auto foc_move_cmd = [&](const httplib::Request& req, httplib::Response& res) {
	  if (req.has_param("Position")) {
	    long long target = std::stoll(req.get_param_value("Position"));
        
	    // LAUNCH IN BACKGROUND THREAD
	    std::thread([this, target]() {
	      this->focuser->moveTo(target, FOC_SPEED_MED);
	    }).detach();
	  }
	  res.set_content(formatResponse(0, req), "application/json");
	};
	
        svr.Put("/api/v1/focuser/0/move", foc_move_cmd);
        svr.Put("/api/v1/focuser/0/Move", foc_move_cmd);


	// New Halt Command
	auto foc_halt = [&](const httplib::Request& req, httplib::Response& res) {
	  std::cout << "[NET] Focuser HALT triggered!" << std::endl;
	  focuser->halt(); 
	  res.set_content(formatResponse(0, req), "application/json");
	};
	svr.Put("/api/v1/focuser/0/halt", foc_halt);
	svr.Put("/api/v1/focuser/0/Halt", foc_halt);
	
        // --- ROTATOR MANDATORY ROUTES (With Case-Insensitivity Aliases) ---

	// --- ROTATOR IDENTITY ROUTES ---

        // Name
        auto rot_name = [&](const httplib::Request& req, httplib::Response& res) {
            res.set_content(formatStringResponse("Dual-Controller Rotator", req), "application/json");
        };
        svr.Get("/api/v1/rotator/0/name", rot_name);
        svr.Get("/api/v1/rotator/0/Name", rot_name);

        // Description
        auto rot_desc = [&](const httplib::Request& req, httplib::Response& res) {
            res.set_content(formatStringResponse("LX200 Field Rotator via Raspberry Pi", req), "application/json");
        };
        svr.Get("/api/v1/rotator/0/description", rot_desc);
        svr.Get("/api/v1/rotator/0/Description", rot_desc);

        // DriverInfo
        auto rot_info = [&](const httplib::Request& req, httplib::Response& res) {
            res.set_content(formatStringResponse("Built by Robert Steele - v2.1", req), "application/json");
        };
        svr.Get("/api/v1/rotator/0/driverinfo", rot_info);
        svr.Get("/api/v1/rotator/0/DriverInfo", rot_info);

        // DriverVersion
        auto rot_ver = [&](const httplib::Request& req, httplib::Response& res) {
            res.set_content(formatStringResponse("2.1", req), "application/json");
        };
        svr.Get("/api/v1/rotator/0/driverversion", rot_ver);
        svr.Get("/api/v1/rotator/0/DriverVersion", rot_ver);

        // InterfaceVersion
        auto rot_iface = [&](const httplib::Request& req, httplib::Response& res) {
            res.set_content(formatResponse(3, req), "application/json");
        };
        svr.Get("/api/v1/rotator/0/interfaceversion", rot_iface);
        svr.Get("/api/v1/rotator/0/InterfaceVersion", rot_iface);
	
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

    // Helper for String Responses
    std::string formatStringResponse(std::string val, const httplib::Request& req) {
        std::string clientID = req.has_param("ClientTransactionID") ? req.get_param_value("ClientTransactionID") : "0";
        return "{\"Value\":\"" + val + "\",\"ClientTransactionID\":" + clientID + ",\"ErrorNumber\":0,\"ErrorMessage\":\"\"}";
    }

    // NEW: Helper for "Void" Responses (Commands that don't return a value)
    std::string formatVoidResponse(const httplib::Request& req) {
        std::string clientID = req.has_param("ClientTransactionID") ? req.get_param_value("ClientTransactionID") : "0";
        static uint32_t serverID = 1;
        return "{\"ClientTransactionID\":" + clientID + ",\"ServerTransactionID\":" + std::to_string(serverID++) + ",\"ErrorNumber\":0,\"ErrorMessage\":\"\"}";
    }
};

#endif
