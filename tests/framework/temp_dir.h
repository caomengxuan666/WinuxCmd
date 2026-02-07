#pragma once
#include <windows.h>

#include <filesystem>
#include <fstream>
#include <string>

struct TempDir {
  std::filesystem::path path;

  TempDir() {
    wchar_t base[MAX_PATH];
    GetTempPathW(MAX_PATH, base);

    wchar_t name[MAX_PATH];
    GetTempFileNameW(base, L"wct", 0, name);
    DeleteFileW(name);

    path = name;
    std::filesystem::create_directory(path);
  }

  ~TempDir() {
    std::error_code ec;
    std::filesystem::remove_all(path, ec);
  }

  [[nodiscard]]
  std::wstring wpath() const {
    return path.wstring();
  }

  void write(const std::string &rel, const std::string &content) const {
    auto p = path / rel;
    std::filesystem::create_directories(p.parent_path());

    std::ofstream ofs(p, std::ios::binary);
    ofs.write(content.data(), content.size());
  }

  void write_bytes(const std::string &rel,
                   const std::vector<char> &data) const {
    auto p = path / rel;
    std::filesystem::create_directories(p.parent_path());

    std::ofstream ofs(p, std::ios::binary);
    ofs.write(data.data(), data.size());
  }

  std::string read(const std::string &rel) const {
    auto p = path / rel;
    std::ifstream ifs(p, std::ios::binary);
    return std::string((std::istreambuf_iterator<char>(ifs)),
                       std::istreambuf_iterator<char>());
  }
};
