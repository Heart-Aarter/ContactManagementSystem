#include "points_file.h"

#include <cstdio>
#include <filesystem>
#include <string>

namespace {

std::string resolveDataPath(const char* pPath) {
    const std::filesystem::path directPath(pPath);
    if (std::filesystem::exists(directPath) || std::filesystem::exists(directPath.parent_path())) {
        return directPath.string();
    }

    const std::filesystem::path parentPath = std::filesystem::path("..") / directPath;
    if (std::filesystem::exists(parentPath) || std::filesystem::exists(parentPath.parent_path())) {
        return parentPath.string();
    }

    const std::filesystem::path grandParentPath = std::filesystem::path("..") / ".." / directPath;
    if (std::filesystem::exists(grandParentPath) || std::filesystem::exists(grandParentPath.parent_path())) {
        return grandParentPath.string();
    }

    return directPath.string();
}

FILE* openResolvedFile(const char* pPath, const char* pMode) {
    const std::string resolvedPath = resolveDataPath(pPath);
    return fopen(resolvedPath.c_str(), pMode);
}

}

int savePoint(const PointChange* pPoint, const char* pPath) {
    if (pPoint == nullptr || pPath == nullptr) {
        return FALSE;
    }

    FILE* fp = openResolvedFile(pPath, "ab");
    if (fp == nullptr) {
        fp = openResolvedFile(pPath, "wb");
        if (fp == nullptr) {
            return FALSE;
        }
    }

    if (fwrite(pPoint, sizeof(PointChange), 1, fp) != 1) {
        fclose(fp);
        return FALSE;
    }

    fclose(fp);
    return TRUE;
}

int getPointCount(const char* pPath) {
    if (pPath == nullptr) {
        return 0;
    }

    FILE* fp = openResolvedFile(pPath, "rb");
    if (fp == nullptr) {
        return 0;
    }

    int nCount = 0;
    PointChange point{};
    while (fread(&point, sizeof(PointChange), 1, fp) == 1) {
        nCount++;
    }

    fclose(fp);
    return nCount;
}

int readPoint(PointChange* pPoint, const char* pPath) {
    if (pPoint == nullptr || pPath == nullptr) {
        return FALSE;
    }

    FILE* fp = openResolvedFile(pPath, "rb");
    if (fp == nullptr) {
        return FALSE;
    }

    int nCount = 0;
    while (fread(&pPoint[nCount], sizeof(PointChange), 1, fp) == 1) {
        nCount++;
    }

    fclose(fp);
    return nCount;
}
