/*
 * MIT License
 *
 * Copyright (c) 2025 Filipp Andjelo
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

#include <stdexcept>
#include <string>

namespace sqlite3pp {

class SQLITE3PP_EXPORT Error : public std::runtime_error {
public:
    Error(const std::string& what) noexcept : std::runtime_error(what) {}
};

class SQLITE3PP_EXPORT OpenDatabaseError : public Error {
public:
    OpenDatabaseError(std::string fileName, std::string reason)
    : Error{"Failed to open database: " + fileName + ": " + reason}, m_fileName{std::move(fileName)} {}

    const std::string& getFileName() const { return m_fileName; }

    const std::string& getReason() const { return m_reason; }

private:
    std::string m_fileName;
    std::string m_reason;
};

class SQLITE3PP_EXPORT PrepareStatementError : public Error {
public:
    PrepareStatementError(const std::string& what, std::string sql)
    : Error{"Failed to prepare statement: " + what}, m_sql{std::move(sql)} {}

    const std::string& getSql() const { return m_sql; }

private:
    std::string m_sql;
};

class SQLITE3PP_EXPORT BindParameterError : public Error {
public:
    BindParameterError(const std::string& what, size_t index)
    : Error{"Failed to bind paramter " + std::to_string(index) + ": " + what}, m_index{index} {}

    size_t getIndex() const { return m_index; }

private:
    size_t m_index;
};

class SQLITE3PP_EXPORT TypeMismatchError : public Error {
public:
    TypeMismatchError(size_t index)
    : Error{"Result type mismatch in column " + std::to_string(index)}, m_index{index} {}

    size_t getIndex() const { return m_index; }

private:
    size_t m_index;
};

} // namespace sqlite3pp
