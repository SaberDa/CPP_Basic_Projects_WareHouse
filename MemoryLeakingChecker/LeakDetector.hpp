#ifndef __LEAK_DETECTOR__
#define __LEAK_DETECTOR__

void* operator new(size_t _size, char *_file, unsigned int _line);
void* operator new[](size_t _size, char *_file, unsigned int _line);

#ifndef __NEW_OVERLOAD_IMPLEMENTATION__
#define new    new(__FILE__, __LINE__)
#endif

class _leak_detector {
public:
    // callCount to make sure we only call it once
    static unsigned int callCount;

    _leak_detector() noexcept {
        ++callCount;
    }

    ~_leak_detector() noexcept {
        if (--callCount == 0) LeakDetector();
    }

private:
    static unsigned int LeakDetector() noexcept;
};

static _leak_detector _exit_counter;

#endif // !__LEAK_DETECTOR__