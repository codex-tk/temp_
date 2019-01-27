/**
 * @file file_output.cpp
 * @author ghtak
 * @brief
 * @version 0.1
 * @date 2019-01-27
 *
 * @copyright Copyright (c) 2019
 *
 */

#include <iomanip>
#include <tlab/log/outputs/file_output.hpp>

#if defined(_WIN32) || defined(__WIN32__)
#else
#include <dirent.h>
#include <errno.h>
#include <pwd.h>
#include <sys/param.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#endif

namespace tlab::log {
namespace {

bool dir_exist(const std::string &path) {
#if defined(_WIN32) || defined(__WIN32__)
    DWORD dwAttrib = GetFileAttributesA(path.c_str());

    return (dwAttrib != INVALID_FILE_ATTRIBUTES &&
            (dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
#else
    struct stat info;
    if (stat(path.c_str(), &info) != 0)
        return false;
    else if (info.st_mode & S_IFDIR)
        return true;
    else
        return false;
#endif
}

bool mkdir(const std::string &path) {
#if defined(_WIN32) || defined(__WIN32__)
    LPSECURITY_ATTRIBUTES attr = nullptr;
    return CreateDirectoryA(path.c_str(), attr) == TRUE;
#else
    return ::mkdir(path.c_str(), 0755) == 0;
#endif
}

void list_files(const std::string &path, std::vector<std::string> &files) {
#if defined(_WIN32) || defined(__WIN32__)
    WIN32_FIND_DATAA find_data;
    HANDLE h = FindFirstFileA((path + "\\*.*").c_str(), &find_data);
    if (h != INVALID_HANDLE_VALUE) {
        do {
            if (!(find_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
                files.emplace_back(find_data.cFileName);
            }
        } while (FindNextFileA(h, &find_data) == TRUE);
        FindClose(h);
    }
#else
    struct stat statinfo;
    DIR *dir = opendir(path.c_str());
    if (dir == nullptr)
        return;

    char full_path[4096];
    struct dirent *ent = nullptr;
    while ((ent = readdir(dir)) != nullptr) {
        sprintf(full_path, "%s/%s", path.c_str(), ent->d_name);
        if (stat(full_path, &statinfo) == 0) {
            if (!S_ISDIR(statinfo.st_mode)) {
                files.emplace_back(ent->d_name);
            }
        }
    }
    closedir(dir);
#endif
}

} // namespace

file_output::file_output(const std::string &path, const int max_days,
                         const int max_lines)
    : _path(path), _max_lines(max_lines), _lines(0), _max_days(max_days),
      _seq(0) {
    if (dir_exist(_path) == false) {
        mkdir(_path);
    }
}

void file_output::put(const char *ptr, const std::size_t,
                      const tlab::log::record &) {
    if (_ofstream.is_open() == false) {
        std::ostringstream oss;
        int64_t now_ms =
            std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::system_clock::now().time_since_epoch())
                .count();
        std::time_t now = now_ms / 1000;
        struct std::tm tm;
#if defined(_WIN32) || defined(__WIN32__)
        localtime_s(&tm, &now);
#else
        localtime_r(&now, &tm);
#endif
        oss << _path << '/' << std::setfill('0') << std::setw(4)
            << tm.tm_year + 1900 << std::setw(2) << tm.tm_mon + 1
            << std::setw(2) << tm.tm_mday << '_' << std::setw(2) << tm.tm_hour
            << std::setw(2) << tm.tm_min << std::setw(2) << tm.tm_sec << '_'
            << std::setw(4) << ++_seq;
        _ofstream.open(oss.str());
    }

    if (_ofstream.good()) {
        _ofstream << ptr << '\n';
        ++_lines;
    }
    if (_lines >= _max_lines) {
        _ofstream.close();
        _lines = 0;
        delete_old_logs();
    }
}

void file_output::delete_old_logs(void) {
    std::time_t now;
    struct std::tm tm;

    std::time(&now);
    now -= (60 * 60 * 24 * _max_days);
#if defined(_WIN32) || defined(__WIN32__)
    localtime_s(&tm, &now);
#else
    localtime_r(&now, &tm);
#endif
    std::ostringstream oss;
    oss << std::setfill('0') << std::setw(4) << tm.tm_year + 1900
        << std::setw(2) << tm.tm_mon + 1 << std::setw(2) << tm.tm_mday << '_'
        << std::setw(2) << tm.tm_hour << std::setw(2) << tm.tm_min
        << std::setw(2) << tm.tm_sec;
    std::string base = oss.str();
    const std::size_t base_len = base.length();
    std::vector<std::string> files;
    list_files(_path, files);
    for (std::string &file : files) {
        if (file.length() > base_len) {
            if (file.substr(0, base_len) < base) {
#if defined(_WIN32) || defined(__WIN32__)
                std::string full_path = _path + '\\' + file;
                DeleteFileA(full_path.c_str());
#else
                std::string full_path = _path + '/' + file;
                remove(full_path.c_str());
#endif
            }
        }
    }
}

} // namespace tlab::log