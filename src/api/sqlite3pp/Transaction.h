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

namespace sqlite3pp {

class SQLITE3PP_EXPORT Transaction {
public:
    Transaction(const Transaction&) = delete;
    Transaction(Transaction&&) noexcept = default;
    Transaction& operator=(const Transaction&) = delete;
    Transaction& operator=(Transaction&&) noexcept = default;
    ~Transaction();

    explicit Transaction(std::shared_ptr<sqlite3> db);

    void commit() const;

    Statement prepare(const std::string& sql) const { return {m_db, sql}; }

    void execute(const std::string& sql) const { prepare(sql).execute(); }

private:
    std::shared_ptr<sqlite3> m_db;
};

} // namespace sqlite3pp
