#ifndef CSV_H
#define CSV_H

#include <cstdio>
#include <vector>
#include <string>
#include <cstring>

#include <algorithm>
#include <utility>
#include <exception>

#ifndef CSV_IO_NO_THREAD
#include <mutex>
#include <thread>
#include <condition_variable>
#endif

#include <memory>
#include <cassert>
#include <cerrno>
#include <istream>

namespace io {

    /* ============================== LineReader ============================== */

    namespace error {
        struct base : std::exception {
            virtual void format_error_message()const = 0;

            const char*what()const noexcept override {
                format_error_message();
                return error_message_buffer;
            }

            mutable char error_message_buffer[512];
        };

        const int max_file_name_length = 255;

        struct with_file_name {
            with_file_name() {
                std::memset(file_name, 0, sizeof(file_name));
            }

            void set_file_name(const char* file_name) {
                if (file_name != nullptr) {
                    // This call to strncpy has parenthesis arount it to slience the GCC -Wstringop-truncation warning
                    (strncpy(this->file_name, file_name, sizeof(this->file_name)));
                    this->file_name[sizeof(this->file_name) - 1] = '\0';
                } else {
                    this->file_name[0] = '\0';
                }
            }

            char file_name[max_file_name_length + 1];
        };

        struct with_file_line {
            with_file_line() {
                file_line = -1;
            }
            
            void set_file_line(int file_line) {
                this->file_line = file_line;
            }

            int file_line;
        };

        struct with_errno {
            with_errno() {
                errno_value = 0;
            }

            void set_errno(int errno_value) {
                this->errno_value = errno_value;
            }

            int errno_value;
        };

        struct can_not_open_file:
               base,
               with_file_name,
               with_errno {
            void format_error_message()const override {
                if (errno_value != 0) {
                    std::snprintf(error_message_buffer, sizeof(error_message_buffer),
                                  "Cannot open file\"%s\" because \"%s\".",
                                  file_name, std::strerror(errno_value));
                } else {
                    std::snprintf(error_message_buffer, sizeof(error_message_buffer),
                                  "Line number %d in file \"%s\" exceeds the maximum length of 2^24 - 1.",
                                  file_name, file_name);
                }
            }
        };
    } // namespace error

    class ByteSourceBase {
    public:
        virtual int read(char* buffer, int size) = 0;
        virtual ~ByteSourceBase(){}
    }; // class ByteSourceBase

    namespace detail {

        class OwningStdIOByteSourceBase : public ByteSourceBase {
        public:
            explicit OwningStdIOByteSourceBase(FILE* file):file(file) {
                // Tell the std library that we want to do the buffering ourself.
                std::setvbuf(file, 0, _IONBF, 0);
            }

            int read(char* buffer, int size) {
                return std::fread(buffer, 1, size, file);
            }

            ~OwningStdIOByteSourceBase() {
                std::fclose(file);
            }
        private:
            FILE* file;
        }; // class OwningStdIOByteSourceBase

        class NonOwningIStreamByteSource: public ByteSourceBase {
        public:
            explicit NonOwningIStreamByteSource(std::istream &in): in(in) {}

            int read(char *buffer, int size) {
                in.read(buffer, size);
                return in.gcount();
            }

            ~NonOwningIStreamByteSource() {}
        private:
            std::istream &in;
        };

    } // namespace detail

}


#endif