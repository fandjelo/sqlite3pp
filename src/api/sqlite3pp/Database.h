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
#include "Statement.h"

#include <memory>
#include <string>

namespace sqlite3pp {

class SQLITE3PP_EXPORT Database {
public:
    explicit Database(const std::string& uri);

    Statement prepare(const std::string& sql) const { return Statement{m_db, sql}; }

    void execute(const std::string& sql) const { prepare(sql).execute(); }

    template <typename T>
    void execute(const std::string& sql, T&& action) const {
        prepare(sql).execute(std::forward<T>(action));
    }

    template <typename T>
    T execute(const std::string& sql) const {
        return prepare(sql).execute<T>();
    }

    template <typename Action>
    void transaction(const Action& action) const {
        execute("BEGIN");
        // Misuse unique_ptr as a scope guard to do rollback on error
        using Guard = std::unique_ptr<const Database, void (*)(const Database*)>;
        auto guard = Guard{this, [](const auto* db) { db->execute("ROLLBACK"); }};
        action(*this);
        guard.release()->execute("COMMIT");
    }

private:
    std::shared_ptr<sqlite3> m_db;
};

} // namespace sqlite3pp
