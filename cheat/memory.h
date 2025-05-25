#ifndef MEMORY_MEMORY_H
#define MEMORY_MEMORY_H

#include <Windows.h>
#include <vector>
#include <memory>
#include <thread>
#include <mutex>
#include <atomic>
#include <string>

class MemoryScanner {
public:
    explicit MemoryScanner(HANDLE hProcess);

    std::vector<ULONG_PTR> Search(const char *pattern);

private:
    static constexpr size_t BLOCK_SIZE = 409600;

    struct MemoryRegion {
        ULONG_PTR base;
        SIZE_T size;
    };

    HANDLE m_hProcess;
    short m_badChar[256]{}; // BM算法的坏字符表

    static void GetPatternArray(
            const char *pattern,
            std::unique_ptr<WORD[]> &patternArray,
            WORD &length
    );

    void BuildBadCharTable(
            const WORD *patternArray,
            WORD patternLength
    );

    void SearchBlock(
            const WORD *pattern,
            WORD patternLength,
            ULONG_PTR base,
            SIZE_T size,
            std::vector<ULONG_PTR> &results,
            std::mutex &mutex
    );

    void CollectRegions(std::vector<MemoryRegion> &regions);
};

class Memory {
public:
    // 构造函数（需传入有效的进程句柄）
    explicit Memory(HANDLE hProcess, uintptr_t baseAddress = 0) : m_hProcess(hProcess), m_baseAddress(baseAddress) {
        if (hProcess == nullptr) throw std::invalid_argument("Invalid process handle");
    }

    // 基础地址操作
    [[nodiscard]] uintptr_t GetBaseAddress() const { return m_baseAddress; }

    void SetBaseAddress(uintptr_t address) { m_baseAddress = address; }

    // 模板方法：读取指定类型数据
    template<typename T>
    [[nodiscard]] T Read(uintptr_t offset = 0) const {
        T value;
        if (!ReadProcessMemory(m_hProcess,
                               reinterpret_cast<LPCVOID>(m_baseAddress + offset),
                               &value,
                               sizeof(T),
                               nullptr)) {
            throw std::runtime_error("Read memory failed. Error: " + std::to_string(GetLastError()));
        }
        return value;
    }

    // 模板方法：写入指定类型数据
    template<typename T>
    void Write(uintptr_t offset, const T &value) const {
        if (!WriteProcessMemory(m_hProcess,
                                reinterpret_cast<LPVOID>(m_baseAddress + offset),
                                &value,
                                sizeof(T),
                                nullptr)) {
            throw std::runtime_error("Write memory failed. Error: " + std::to_string(GetLastError()));
        }
    }

    // 字节数组操作
    [[nodiscard]] std::vector<BYTE> ReadBytes(uintptr_t offset, size_t size) const {
        std::vector<BYTE> buffer(size);
        if (!ReadProcessMemory(m_hProcess,
                               reinterpret_cast<LPCVOID>(m_baseAddress + offset),
                               buffer.data(),
                               size,
                               nullptr)) {
            throw std::runtime_error("Read bytes failed. Error: " + std::to_string(GetLastError()));
        }
        return buffer;
    }

    [[nodiscard]] std::string ReadString(uintptr_t offset = 0, size_t maxLength = 4096) const {
        // 参数校验
        if (maxLength == 0) {
            throw std::invalid_argument("maxLength must be greater than 0");
        }

        // 读取原始字节数据
        std::vector<BYTE> buffer = ReadBytes(offset, maxLength);

        // 查找第一个空字符（0x00）的位置
        auto endPos = std::find(buffer.begin(), buffer.end(), 0x00);

        // 截断到空字符或最大长度
        return std::string(buffer.begin(), endPos);
    }

    void WriteBytes(uintptr_t offset, const std::vector<BYTE> &data) const {
        if (!WriteProcessMemory(m_hProcess,
                                reinterpret_cast<LPVOID>(m_baseAddress + offset),
                                data.data(),
                                data.size(),
                                nullptr)) {
            throw std::runtime_error("Write bytes failed. Error: " + std::to_string(GetLastError()));
        }
    }

    // 指针链解析（支持多级偏移）
    template<typename... Offsets>
    uintptr_t ResolvePointerChain(Offsets... offsets) const {
        uintptr_t current = m_baseAddress;
        for (auto offset: {offsets...}) current = Read<uintptr_t>(current) + offset;
        return current;
    }

private:
    HANDLE m_hProcess;    // 进程句柄（由调用者管理生命周期）
    uintptr_t m_baseAddress; // 当前基地址
};

#endif //MEMORY_MEMORY_H
