#include "http_server.hpp"
#include "achats_locaux.hpp"
#include "httplib.h"
#include "json.hpp"
#include <fstream>
#include <iostream>
#include <mutex>

using json = nlohmann::json;

namespace {
std::mutex g_mutex;
bool g_hasResult = false;
AchatsLocauxResult g_lastResult;
} // namespace

void runServer(int port, const std::string &webRoot) {
  httplib::Server svr;

  // Serve web/index.html, style.css, app.js as static files at "/"
  svr.set_mount_point("/", webRoot);

  svr.set_post_routing_handler(
      [](const httplib::Request &, httplib::Response &res) {
        res.set_header("Cache-Control", "no-store, no-cache, must-revalidate");
      });

  // POST /api/achats-locaux/upload  (multipart form, field name "file")
  svr.Post("/api/achats-locaux/upload",
           [](const httplib::Request &req, httplib::Response &res) {
             try {
               if (!req.form.has_file("file")) {
                 res.status = 400;
                 res.set_content(R"({"error":"missing 'file' field"})",
                                 "application/json");
                 return;
               }
               const auto &file = req.form.get_file("file");

               std::string tmpPath = "achats_locaux_upload.xlsm";
               std::ofstream(tmpPath, std::ios::binary) << file.content;

               AchatsLocauxResult result = computeAchatsLocaux(tmpPath);

               {
                 std::lock_guard<std::mutex> lock(g_mutex);
                 g_lastResult = result;
                 g_hasResult = true;
               }

               res.set_content(R"({"status":"ok"})", "application/json");
             } catch (const std::exception &ex) {
               res.status = 500;
               json j = {{"error", ex.what()}};
               res.set_content(j.dump(), "application/json");
             } catch (...) {
               res.status = 500;
               json j = {{"error", "unknown non-standard exception"}};
               res.set_content(j.dump(), "application/json");
             }
           });

  // GET /api/achats-locaux  -> latest computed result
  svr.Get("/api/achats-locaux", [](const httplib::Request &,
                                   httplib::Response &res) {
    std::lock_guard<std::mutex> lock(g_mutex);

    if (!g_hasResult) {
      res.status = 404;
      res.set_content(R"({"error":"no result yet"})", "application/json");
      return;
    }

    json j = {
        {"LOCAL_OVERDUE_INVOICES", g_lastResult.overdueInvoices},
        {"LOCAL_PENDING_INVOICES", g_lastResult.pendingInvoices},
        {"LOCAL_TOTAL", g_lastResult.total},
    };
    res.set_content(j.dump(), "application/json");
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
