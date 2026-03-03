/*
 * Project:    LX200 Focuser Automation
 * Component:  WaveShare Stepper Driver Header
 * File:       AlpacaServer.hpp
 * Author:     Robert D. Steele
 * Date:       2026-03-02
 * Version:    3.0 (Full ASCOM Handshake Support)
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
#include <ifaddrs.h>
#include <arpa/inet.h>
#include <sys/socket.h>

class AlpacaServer {
public:
    AlpacaServer(WaveShareStepper* foc, WaveShareStepper* rot) 
        : focuser(foc), rotator(rot), running(false) {}

    void start(int port = 8080) {
        running = true;
        printNetworkInfo(port);

        std::cout << "[SYSTEM] Launching Alpaca Service on port " << port << "..." << std::endl;
        server_thread = std::thread(&AlpacaServer::run, this, port);

        // Give the thread a moment to initialize
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    void stop() {
        running = false;
        svr.stop();
        if (server_thread.joinable()) server_thread.join();
    }

    void printNetworkInfo(int port) {
        struct ifaddrs *ifaddr, *ifa;
        char host[NI_MAXHOST];
        
        if (getifaddrs(&ifaddr) == -1) {
            std::cerr << "[NET] Error getting network interfaces" << std::endl;
            return;
        }
        
        std::cout << "\n--- Alpaca Server Network Monitor ---" << std::endl;
        for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
            if (ifa->ifa_addr == NULL || ifa->ifa_addr->sa_family != AF_INET) continue;
            
            if (getnameinfo(ifa->ifa_addr, sizeof(struct sockaddr_in), host, NI_MAXHOST, NULL, 0, NI_NUMERICHOST) == 0) {
                std::string ifName(ifa->ifa_name);
                if (ifName != "lo") {
                    std::cout << "  > " << ifName << ": http://" << host << ":" << port << std::endl;
                }
            }
        }
        std::cout << "-------------------------------------\n" << std::endl;
        freeifaddrs(ifaddr);
    }

private:
    WaveShareStepper* focuser;
    WaveShareStepper* rotator;
    httplib::Server svr;
    std::thread server_thread;
    bool running;

    void run(int port) {
        // ====================================================================
        // --- MANAGEMENT ROUTES ---
        // ====================================================================
        
        svr.Get("/management/v1/description", [&](const httplib::Request& req, httplib::Response& res) {
            res.set_content(R"({"Value":{"ServerName":"Pi-LX200-Focuser","Manufacturer":"Steele-Astronomy","ManufacturerVersion":"1.1","Location":"Observatory"},"ClientTransactionID":0,"ServerTransactionID":1,"ErrorNumber":0,"ErrorMessage":""})", "application/json");
        });

        svr.Get("/management/v1/configureddevices", [&](const httplib::Request& req, httplib::Response& res) {
            std::string clientID = req.has_param("ClientTransactionID") ? req.get_param_value("ClientTransactionID") : "0";
            std::string json = R"({
                "Value": [
                {
                    "DeviceName": "Dual Controller Focuser",
                    "DeviceType": "Focuser",
                    "DeviceNumber": 0,
                    "UniqueID": "Steele-Focuser-01"
                },
                {
                    "DeviceName": "Dual Controller Rotator",
                    "DeviceType": "Rotator",
                    "DeviceNumber": 0,
                    "UniqueID": "Steele-Rotator-01"
                }],
                "ClientTransactionID": )" + clientID + R"(,
                "ServerTransactionID": 1,
                "ErrorNumber": 0,
                "ErrorMessage": ""
            })";
            res.set_content(json, "application/json");
        });

        // ====================================================================
        // --- FOCUSER ROUTES (Device 0) ---
        // ====================================================================

        // Handshake: Connected
        auto foc_con = [&](const httplib::Request& req, httplib::Response& res) {
            res.set_content(formatBoolResponse(true, req), "application/json");
        };
        svr.Get("/api/v1/focuser/0/connected", foc_con);
        svr.Get("/api/v1/focuser/0/Connected", foc_con);
        svr.Put("/api/v1/focuser/0/connected", foc_con);
        svr.Put("/api/v1/focuser/0/Connected", foc_con);

        // Info: Name
        auto foc_name = [&](const httplib::Request& req, httplib::Response& res) {
            res.set_content(formatStringResponse("Dual-Controller Focuser", req), "application/json");
        };
        svr.Get("/api/v1/focuser/0/name", foc_name);
        svr.Get("/api/v1/focuser/0/Name", foc_name);

        // State: Position
        auto foc_pos = [&](const httplib::Request& req, httplib::Response& res) {
            res.set_content(formatResponse(focuser->getCurrentPosition(), req), "application/json");
        };
        svr.Get("/api/v1/focuser/0/position", foc_pos);
        svr.Get("/api/v1/focuser/0/Position", foc_pos);

        // State: IsMoving
        auto foc_mov = [&](const httplib::Request& req, httplib::Response& res) {
            res.set_content(formatBoolResponse(focuser->isMoving(), req), "application/json");
        };
        svr.Get("/api/v1/focuser/0/ismoving", foc_mov);
        svr.Get("/api/v1/focuser/0/IsMoving", foc_mov);

        // Capability: Absolute & MaxIncrement
        svr.Get("/api/v1/focuser/0/absolute", [&](const httplib::Request& req, httplib::Response& res) {
            res.set_content(formatBoolResponse(true, req), "application/json");
        });
        svr.Get("/api/v1/focuser/0/maxincrement", [&](const httplib::Request& req, httplib::Response& res) {
            res.set_content(formatResponse(10000, req), "application/json"); 
        });

        // Command: Move
        auto foc_move_cmd = [&](const httplib::Request& req, httplib::Response& res) {
            if (req.has_param("Position")) {
                long long target = std::stoll(req.get_param_value("Position"));
                std::thread([this, target]() {
                    this->focuser->moveTo(target, FOC_SPEED_MED);
                }).detach();
            }
            res.set_content(formatVoidResponse(req), "application/json");
        };
        svr.Put("/api/v1/focuser/0/move", foc_move_cmd);
        svr.Put("/api/v1/focuser/0/Move", foc_move_cmd);

        // Command: Halt
        auto foc_halt = [&](const httplib::Request& req, httplib::Response& res) {
            focuser->halt(); 
            res.set_content(formatVoidResponse(req), "application/json");
        };
        svr.Put("/api/v1/focuser/0/halt", foc_halt);
        svr.Put("/api/v1/focuser/0/Halt", foc_halt);


        // ====================================================================
        // --- ROTATOR ROUTES (Device 0) ---
        // ====================================================================

        // Handshake: Connected
        auto rot_con = [&](const httplib::Request& req, httplib::Response& res) {
            res.set_content(formatBoolResponse(true, req), "application/json");
        };
        svr.Get("/api/v1/rotator/0/connected", rot_con);
        svr.Get("/api/v1/rotator/0/Connected", rot_con);
        svr.Put("/api/v1/rotator/0/connected", rot_con);
        svr.Put("/api/v1/rotator/0/Connected", rot_con);

        // Info: Name
        auto rot_name = [&](const httplib::Request& req, httplib::Response& res) {
            res.set_content(formatStringResponse("Dual-Controller Rotator", req), "application/json");
        };
        svr.Get("/api/v1/rotator/0/name", rot_name);
        svr.Get("/api/v1/rotator/0/Name", rot_name);

        // State: Position
        auto rot_pos = [&](const httplib::Request& req, httplib::Response& res) {
            res.set_content(formatResponse(rotator->getCurrentPosition(), req), "application/json");
        };
        svr.Get("/api/v1/rotator/0/position", rot_pos);
        svr.Get("/api/v1/rotator/0/Position", rot_pos);

        // State: IsMoving
        auto rot_mov = [&](const httplib::Request& req, httplib::Response& res) {
            res.set_content(formatBoolResponse(rotator->isMoving(), req), "application/json");
        };
        svr.Get("/api/v1/rotator/0/ismoving", rot_mov);
        svr.Get("/api/v1/rotator/0/IsMoving", rot_mov);

        // Capability: CanHalt
        svr.Get("/api/v1/rotator/0/canhalt", [&](const httplib::Request& req, httplib::Response& res) {
            res.set_content(formatBoolResponse(true, req), "application/json");
        });

        // Command: MoveAbsolute
        auto rot_move_cmd = [&](const httplib::Request& req, httplib::Response& res) {
            if (req.has_param("Position")) {
                long long target = std::stoll(req.get_param_value("Position"));
                std::cout << "[NET] Rotator Move Threaded -> " << target << std::endl;
                std::thread([this, target]() {
                    this->rotator->moveTo(target, ROT_SPEED_MED);
                }).detach();
            }
            res.set_content(formatVoidResponse(req), "application/json");
        };
        svr.Put("/api/v1/rotator/0/moveabsolute", rot_move_cmd);
        svr.Put("/api/v1/rotator/0/MoveAbsolute", rot_move_cmd);

        // Command: Halt
        auto rot_halt = [&](const httplib::Request& req, httplib::Response& res) {
            std::cout << "[NET] Rotator HALT triggered!" << std::endl;
            rotator->halt(); 
            res.set_content(formatVoidResponse(req), "application/json");
        };
        svr.Put("/api/v1/rotator/0/halt", rot_halt);
        svr.Put("/api/v1/rotator/0/Halt", rot_halt);

        // Start Listening
        std::cout << "[NET] Thread: Routes defined. Calling listen()..." << std::endl;
        if (!svr.listen("0.0.0.0", port)) {
            std::cerr << "[ERROR] Alpaca Server failed to bind to port " << port << "!" << std::endl;
        }
        std::cout << "[NET] Thread: Server has stopped." << std::endl;
    }

    // --- HELPERS ---
    std::string formatResponse(long long val, const httplib::Request& req) {
        std::string clientID = req.has_param("ClientTransactionID") ? req.get_param_value("ClientTransactionID") : "0";
        return "{\"Value\":" + std::to_string(val) + ",\"ClientTransactionID\":" + clientID + ",\"ErrorNumber\":0,\"ErrorMessage\":\"\"}";
    }

    std::string formatBoolResponse(bool val, const httplib::Request& req) {
        std::string clientID = req.has_param("ClientTransactionID") ? req.get_param_value("ClientTransactionID") : "0";
        return "{\"Value\":" + std::string(val ? "true" : "false") + ",\"ClientTransactionID\":" + clientID + ",\"ErrorNumber\":0,\"ErrorMessage\":\"\"}";
    }

    std::string formatStringResponse(std::string val, const httplib::Request& req) {
        std::string clientID = req.has_param("ClientTransactionID") ? req.get_param_value("ClientTransactionID") : "0";
        return "{\"Value\":\"" + val + "\",\"ClientTransactionID\":" + clientID + ",\"ErrorNumber\":0,\"ErrorMessage\":\"\"}";
    }

    std::string formatVoidResponse(const httplib::Request& req) {
        std::string clientID = req.has_param("ClientTransactionID") ? req.get_param_value("ClientTransactionID") : "0";
        return "{\"ClientTransactionID\":" + clientID + ",\"ErrorNumber\":0,\"ErrorMessage\":\"\"}";
    }
};

#endif