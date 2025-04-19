#include "memory.h"

MemoryScanner::MemoryScanner(HANDLE hProcess) : m_hProcess(hProcess) {
    memset(m_badChar, -1, sizeof(m_badChar));
}

void MemoryScanner::GetPatternArray(
        const char *pattern,
        std::unique_ptr<WORD[]> &patternArray,
        WORD &length
) {
    length = static_cast<WORD>(strlen(pattern) / 3 + 1);
    patternArray = std::make_unique<WORD[]>(length);

    for (int i = 0, idx = 0; i < strlen(pattern);) {
        char bytes[3] = {pattern[i++], pattern[i++], pattern[i++]};

        if (bytes[0] == '?' && bytes[1] == '?') {
            patternArray[idx++] = 256;  // 通配符
        } else {
            WORD value = 0;
            for (int j = 0; j < 2; ++j) {
                value <<= 4;
                if (bytes[j] >= '0' && bytes[j] <= '9') {
                    value |= bytes[j] - '0';
                } else if (bytes[j] >= 'A' && bytes[j] <= 'F') {
                    value |= bytes[j] - 'A' + 10;
                } else if (bytes[j] >= 'a' && bytes[j] <= 'f') {
                    value |= bytes[j] - 'a' + 10;
                }
            }
            patternArray[idx++] = value;
        }
    }
}

void MemoryScanner::BuildBadCharTable(
        const WORD *patternArray,
        WORD patternLength
) {
    memset(m_badChar, -1, sizeof(m_badChar));
    for (WORD i = 0; i < patternLength; ++i) {
        WORD c = patternArray[i];
        if (c != 256) { // 忽略通配符
            m_badChar[c] = static_cast<short>(i);
        }
    }
}

void MemoryScanner::SearchBlock(
        const WORD *pattern,
        WORD patternLength,
        ULONG_PTR base,
        SIZE_T size,
        std::vector<ULONG_PTR> &results,
        std::mutex &mutex
) {
    std::unique_ptr<BYTE[]> buffer(new BYTE[size]);

    if (!ReadProcessMemory(m_hProcess,
                           reinterpret_cast<LPCVOID>(base),
                           buffer.get(),
                           size,
                           nullptr)) {
        return;
    }

    const SIZE_T bufferSize = size;
    const WORD m = patternLength;

    for (SIZE_T i = 0; i <= bufferSize - m;) {
        int j = m - 1;

        // 从右到左匹配
        while (j >= 0 && (pattern[j] == 256 || buffer[i + j] == pattern[j])) {
            j--;
        }

        if (j < 0) {
            // 找到匹配
            std::lock_guard<std::mutex> lock(mutex);
            results.push_back(base + i);
            i++; // 移动到下一个位置
        } else {
            // 计算坏字符规则的移动量
            BYTE c = buffer[i + j];
            short badCharPos = m_badChar[c];
            int shift = j - badCharPos;

            // 如果坏字符不在模式中，移动 j + 1 位
            if (badCharPos == -1) {
                shift = j + 1;
            }

            // 确保至少移动1位
            shift = (std::max)(1, shift);
            i += shift;
        }
    }
}

std::vector<ULONG_PTR> MemoryScanner::Search(const char *pattern) {
    std::unique_ptr<WORD[]> patternArray;
    WORD patternLength;
    GetPatternArray(pattern, patternArray, patternLength);
    BuildBadCharTable(patternArray.get(), patternLength);

    std::vector<MemoryRegion> regions;
    CollectRegions(regions);

    std::atomic_size_t regionIndex(0);
    std::mutex resultMutex;
    std::vector<ULONG_PTR> results;

    unsigned threadCount = (std::max)(std::thread::hardware_concurrency(), 1u);
    std::vector<std::thread> workers;
    workers.reserve(threadCount);

    auto workerTask = [&]() {
        while (true) {
            size_t idx = regionIndex.fetch_add(1);
            if (idx >= regions.size()) break;

            ULONG_PTR base = regions[idx].base;
            SIZE_T remaining = regions[idx].size;

            while (remaining > 0) {
                SIZE_T chunk = (std::min)(remaining, BLOCK_SIZE);
                SearchBlock(patternArray.get(), patternLength, base, chunk, results, resultMutex);
                base += chunk;
                remaining -= chunk;
            }
        }
    };

    for (unsigned i = 0; i < threadCount; ++i) workers.emplace_back(workerTask);
    for (auto &t: workers) if (t.joinable()) t.join();

    return results;
}

void MemoryScanner::CollectRegions(std::vector<MemoryRegion> &regions) {
    ULONG_PTR address = 0;
    MEMORY_BASIC_INFORMATION mbi;

    while (VirtualQueryEx(m_hProcess, reinterpret_cast<LPCVOID>(address), &mbi, sizeof(mbi))) {
        if (mbi.State == MEM_COMMIT && (mbi.Protect == PAGE_READWRITE || mbi.Protect == PAGE_EXECUTE_READWRITE)) {
            regions.push_back({reinterpret_cast<ULONG_PTR>(mbi.BaseAddress), mbi.RegionSize});
        }
        address = reinterpret_cast<ULONG_PTR>(mbi.BaseAddress) + mbi.RegionSize;
    }
}
