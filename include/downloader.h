#ifndef SK_UTILS_DOWNLOADER_H
#define SK_UTILS_DOWNLOADER_H

#pragma once

#include <cstddef>
#include <memory>
#include <string>

#include "curl/curl.h"
#include "tl/expected.hpp"

using CurlHandle = std::unique_ptr<CURL, decltype(&curl_easy_cleanup)>;
using FileHandle = std::unique_ptr<FILE, decltype(&fclose)>;

tl::expected<std::string, std::string> FetchUrl(std::string url) {
    CurlHandle curl(curl_easy_init(), &curl_easy_cleanup);
    if (!curl) {
        return tl::unexpected<std::string>("Init curl failed");
    }
    std::string page;
    curl_easy_setopt(curl.get(), CURLOPT_URL, url.c_str());
    curl_easy_setopt(
        curl.get(), CURLOPT_WRITEFUNCTION, +[](char *content, size_t size, size_t mennb, void *user_data) -> size_t {
            size_t totalsize = size * mennb;
            static_cast<std::string *>(user_data)->append(content, totalsize);
            return totalsize;
        });
    curl_easy_setopt(curl.get(), CURLOPT_WRITEDATA, &page);
    CURLcode response = curl_easy_perform(curl.get());
    if (response != CURLE_OK) {
        auto err = curl_easy_strerror(response);
        return tl::unexpected<std::string>(err);
    }
    return page;
}

tl::expected<size_t, std::string> FetchToFile(const std::string &url, const std::string &output_path) {
    FileHandle file(fopen(output_path.c_str(), "wb"), &fclose);
    if (!file) {
        return tl::unexpected("Failed to open output file: " + output_path);
    }
    CurlHandle curl(curl_easy_init(), &curl_easy_cleanup);
    if (!curl) {
        return tl::unexpected("Init curl failed");
    }
    curl_easy_setopt(curl.get(), CURLOPT_URL, url.c_str());
    //    - CURLOPT_WRITEFUNCTION: 不设置，使用 libcurl 默认的 fwrite 实现
    curl_easy_setopt(curl.get(), CURLOPT_WRITEDATA, file.get());
    curl_easy_setopt(curl.get(), CURLOPT_FOLLOWLOCATION, 1L);

    CURLcode response = curl_easy_perform(curl.get());
    if (response != CURLE_OK) {
        return tl::unexpected(curl_easy_strerror(response));
    }
    curl_off_t bytes_downloaded = 0;
    curl_easy_getinfo(curl.get(), CURLINFO_SIZE_DOWNLOAD_T, &bytes_downloaded);
    return static_cast<size_t>(bytes_downloaded);
}

#endif  // SK_UTILS_DOWNLOADER_H
