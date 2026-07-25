#pragma once
#include <string>

// Starts the HTTP server on the given port, serving webRoot as static files.
// Blocks until the server is stopped (via the /api/shutdown route).
void runServer(int port, const std::string &webRoot);
