#include "http_server.hpp"
#include <cstdlib>
#include <filesystem>
#include <random>
#include <thread>

#if defined(_WIN32)
#include <windows.h>
#elif defined(__APPLE__)
#include <mach-o/dyld.h>
#else
#include <unistd.h>
#endif

namespace fs = std::filesystem;

static fs::path exeDir() {
#if defined(_WIN32)
  char buf[MAX_PATH];
  GetModuleFileNameA(nullptr, buf, MAX_PATH);
  return fs::path(buf).parent_path();
#elif defined(__APPLE__)
  char buf[1024];
  uint32_t size = sizeof(buf);
  _NSGetExecutablePath(buf, &size);
  return fs::canonical(buf).parent_path();
#else
  return fs::canonical("/proc/self/exe").parent_path();
#endif
}

static void openBrowser(const std::string &url) {
#if defined(_WIN32)
  std::string cmd = "start " + url;
#elif defined(__APPLE__)
  std::string cmd = "open " + url;
#else
  std::string cmd = "xdg-open " + url;
#endif
  std::system(cmd.c_str());
}

int main() {
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<int> dist(20000, 59999);
  const int port = dist(gen);

  const std::string url = "http://localhost:" + std::to_string(port);
  const std::string webRoot = (exeDir() / "web").string();

  std::thread launcher([url]() {
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    openBrowser(url);
  });
  launcher.detach();

  runServer(port, webRoot);
  return 0;
}
