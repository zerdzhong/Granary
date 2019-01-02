//
// Created by zhongzhendong on 2018-12-23.
//

#ifndef GRANARY_CHECKS_HPP
#define GRANARY_CHECKS_HPP


#include <string>

namespace granary {
    namespace granary_checks_impl {

        enum class CheckArgType : int {
            kEnd = 0,
            kInt,
            kLong,
            kLongLong,
            kUInt,
            kULong,
            kULongLong,
            kDouble,
            kLongDouble,
            kCharP,
            kStdString,
            kVoidP,
            kCheckOp
        };

        void FatalLog(const char* file,
                      int line,
                      const char* message,
                      const CheckArgType* fmt,
                      ...);

        template <CheckArgType A, typename T>
        struct Val {
            CheckArgType Type() { return A; }
            T GetValue() const {return val; }
            T val;
        };

        inline Val<CheckArgType::kInt, int> MakeVal(int x) {
            return {x};
        }
        inline Val<CheckArgType::kLong, long> MakeVal(long x) {
            return {x};
        }
        inline Val<CheckArgType::kLongLong, long long> MakeVal(long long x) {
            return {x};
        }
        inline Val<CheckArgType::kUInt, unsigned int> MakeVal(unsigned int x) {
            return {x};
        }
        inline Val<CheckArgType::kULong, unsigned long> MakeVal(unsigned long x) {
            return {x};
        }
        inline Val<CheckArgType::kULongLong, unsigned long long> MakeVal(
                unsigned long long x) {
            return {x};
        }

        inline Val<CheckArgType::kDouble, double> MakeVal(double x) {
            return {x};
        }
        inline Val<CheckArgType::kLongDouble, long double> MakeVal(long double x) {
            return {x};
        }

        inline Val<CheckArgType::kCharP, const char*> MakeVal(const char* x) {
            return {x};
        }
        inline Val<CheckArgType::kStdString, const std::string*> MakeVal(
                const std::string& x) {
            return {&x};
        }

        inline Val<CheckArgType::kVoidP, const void*> MakeVal(const void* x) {
            return {x};
        }
    }
}

void FatalMessage(const char* file, int line, const char* msg);

#define D_CHECK(condition)                                             \
  do {                                                                   \
    if (!(condition)) {                                                  \
      FatalMessage(__FILE__, __LINE__, "CHECK failed: " #condition); \
    }                                                                    \
  } while (0)

#endif //GRANARY_CHECKS_HPP
