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

#include "BaseDefs.hpp"
#include "Row.hpp"

#include <map>
#include <memory>
#include <set>
#include <string>
#include <vector>

namespace sqlite3pp {

class SQLITE3PP_EXPORT Statement {
public:
    Statement(std::shared_ptr<sqlite3> db, const std::string& sql);

    void bind(size_t index, int value) const;
    void bind(size_t index, double value) const;
    void bind(size_t index, const std::string& value) const;
    void bind(size_t index, const Blob& value) const;

    template <typename Handler>
    void execute(const Handler& handler) const {
        while (hasNext()) {
            handler(Row{m_stmt.get()});
        }
    }

    void execute() const {
        execute([](const auto&) {});
    }

    template <typename T>
    T execute() const {
        T result{};
        execute([&result](const auto& row) { get(row, result); });
        return result;
    }

private:
    static void get(const Row& row, Blob& result) { result = row.get<Blob>(0); }

    template <typename T>
    static void get(const Row& row, T& result) {
        result = row.get<T>(0);
    }

    template <typename T>
    static void get(const Row& row, std::vector<T>& results) {
        results.push_back(row.get<T>(0));
    }

    template <typename T>
    static void get(const Row& row, std::set<T>& results) {
        results.insert(row.get<T>(0));
    }

    template <typename K, typename V>
    static void get(const Row& row, std::map<K, V>& results) {
        results.insert(row.get<std::pair<K, V>>(0));
    }

    std::shared_ptr<sqlite3> m_db;
    std::unique_ptr<sqlite3_stmt, void (*)(sqlite3_stmt*)> m_stmt{nullptr, nullptr};

    bool hasNext() const;
};

} // namespace sqlite3pp
