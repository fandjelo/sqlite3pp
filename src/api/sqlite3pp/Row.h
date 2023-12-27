/*
 * MIT License
 *
 * Copyright (c) 2023 Filipp Andjelo
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#pragma once

#include "BaseDefs.h"

#include <string>
#include <utility>
#include <vector>

namespace sqlite3pp {

class SQLITE3PP_EXPORT Row {
public:
    explicit Row(sqlite3_stmt* stmt) : m_stmt{stmt} {}

    template <typename T>
    T get(std::size_t index) const {
        T result{};
        get(index, result);
        return result;
    }

private:
    sqlite3_stmt* m_stmt;

    template <typename K, typename V>
    std::size_t get(std::size_t index, std::pair<K, V>& result) const {
        return get(get(index, result.first), result.second);
    }

    std::size_t get(std::size_t index, int& value) const;
    std::size_t get(std::size_t index, double& value) const;
    std::size_t get(std::size_t index, std::string& value) const;
    std::size_t get(std::size_t index, std::vector<char>& value) const;
};

} // namespace sqlite3pp
