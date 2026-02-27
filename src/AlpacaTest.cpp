#include "httplib.h"
#include <iostream>

int main() {
    httplib::Server svr;

    // This handles a request to http://192.168.5.11/hello
    svr.Get("/hello", [](const httplib::Request &, httplib::Response &res) {
        res.set_content("Hello from the Raspberry Pi Focuser!", "text/plain");
        std::cout << "[NET] Windows machine just said hello!" << std::endl;
    });

    std::cout << "Alpaca Test Server started on http://192.168.5.11:8080" << std::endl;
    svr.listen("0.0.0.0", 8080);
    return 0;
}