#include "memory.h"

MemoryScanner::MemoryScanner(HANDLE hProcess) : m_hProcess(hProcess) {
    memset(m_next, -1, sizeof(m_next));
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

void MemoryScanner::BuildNextArray(const WORD *patternArray, WORD patternLength) {
    memset(m_next, -1, sizeof(m_next));
    for (WORD i = 0; i < patternLength; ++i) m_next[patternArray[i]] = static_cast<short>(i);
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

    for (size_t i = 0; i < size;) {
        size_t j = i, k = 0;
        for (; k < patternLength && j < size; ++k, ++j) {
            if (pattern[k] != 256 && buffer[j] != pattern[k]) break;
        }

        if (k == patternLength) {
            std::lock_guard<std::mutex> lock(mutex);
            results.push_back(base + i);
        }

        if (i + patternLength >= size) break;

        int shift = m_next[buffer[i + patternLength]];
        i += (shift == -1) ? (patternLength - m_next[256]) : (patternLength - shift);
    }
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

std::vector<ULONG_PTR>  MemoryScanner::Search(const char *pattern) {
    // 转换特征码
    std::unique_ptr<WORD[]> patternArray;
    WORD patternLength;
    GetPatternArray(pattern, patternArray, patternLength);
    BuildNextArray(patternArray.get(), patternLength);

    // 收集内存区域
    std::vector<MemoryRegion> regions;
    CollectRegions(regions);

    // 多线程参数
    std::atomic_size_t regionIndex(0);
    std::mutex resultMutex;
    std::vector<ULONG_PTR> results;

    // 创建线程池
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