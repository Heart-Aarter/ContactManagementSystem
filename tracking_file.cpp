#include "tracking_file.h"

#include <filesystem>

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

int saveTracking(const Tracking* pTracking, const char* pPath) {
    FILE* fp = nullptr;

    if ((fp = openResolvedFile(pPath, "ab")) == nullptr) {
        if ((fp = openResolvedFile(pPath, "wb")) == nullptr) {
            return FALSE;
        }
    }

    fwrite(pTracking, sizeof(Tracking), 1, fp);
    fclose(fp);
    return TRUE;
}

int updateTracking(const Tracking *pTracking, const char *pPath, int nIndex) {
    FILE *fp = nullptr;
    int nCount = getTrackingCount(pPath);
    const long lPosition = (long) nIndex * (long) sizeof(Tracking);

    if (pTracking == nullptr || pPath == nullptr || nCount == 0 || nIndex < 0 || nIndex >= nCount) {
        return FALSE;
    }

    if ((fp = openResolvedFile(pPath, "rb+")) == nullptr) {
        return FALSE;
    }

    if (fseek(fp, lPosition, SEEK_SET) != 0) {
        fclose(fp);
        return FALSE;
    }

    if (fwrite(pTracking, sizeof(Tracking), 1, fp) != 1) {
        fclose(fp);
        return FALSE;
    }

    fclose(fp);
    return TRUE;
}

int getTrackingCount(const char *pPath) {
    FILE *fp = nullptr;
    long lSize = 0;

    if (pPath == nullptr) {
        return 0;
    }

    if ((fp = openResolvedFile(pPath, "rb")) == nullptr) {
        return 0;
    }

    if (fseek(fp, 0, SEEK_END) != 0) {
        fclose(fp);
        return 0;
    }

    lSize = ftell(fp);
    if (lSize < 0) {
        fclose(fp);
        return 0;
    }

    fclose(fp);
    return (int) (lSize / (long) sizeof(Tracking));
}

int readTracking(Tracking *pTracking, const char *pPath) {
    FILE *fp = nullptr;
    int i = 0;
    int nCount = getTrackingCount(pPath);

    if (nCount == 0) {
        return FALSE;
    }

    if ((fp = openResolvedFile(pPath, "rb")) == nullptr) {
        return FALSE;
    }

    for (i = 0; i < nCount; i++) {
        if (fread(&pTracking[i], sizeof(Tracking), 1, fp) != 1) {
            fclose(fp);
            return FALSE;
        }
    }

    fclose(fp);
    return nCount;
}
