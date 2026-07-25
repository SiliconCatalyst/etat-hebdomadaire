#include "http_server.hpp"
#include "httplib.h"
#include <iostream>

void runServer(int port, const std::string &webRoot) {
  httplib::Server svr;

  // Serve web/index.html, style.css, app.js as static files at "/"
  svr.set_mount_point("/", webRoot);

  svr.set_post_routing_handler(
      [](const httplib::Request &, httplib::Response &res) {
        res.set_header("Cache-Control", "no-store, no-cache, must-revalidate");
      });

  // POST /api/shutdown
  svr.Post("/api/shutdown",
           [&svr](const httplib::Request &, httplib::Response &res) {
             res.set_content("stopping", "text/plain");
             svr.stop();
           });

  std::cout << "Serving on http://localhost:" << port << "\n";
  svr.listen("0.0.0.0", port);
}
