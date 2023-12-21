#pragma once

#include <stdexcept>
#include <string>

namespace SQLite3pp {

class Error : public std::runtime_error {
public:
    Error(const std::string& what) noexcept : std::runtime_error(what) {}
};

class OpenDatabaseError : public Error {
public:
    OpenDatabaseError(std::string fileName, std::string reason)
    : Error{"Failed to open database: " + fileName + ": " + reason}, m_fileName{std::move(fileName)} {}

    const std::string& getFileName() const { return m_fileName; }

    const std::string& getReason() const { return m_reason; }

private:
    std::string m_fileName;
    std::string m_reason;
};

class PrepareStatementError : public Error {
public:
    PrepareStatementError(const std::string& what, std::string sql)
    : Error{"Failed to prepare statement: " + what}, m_sql{std::move(sql)} {}

    const std::string& getSql() const { return m_sql; }

private:
    std::string m_sql;
};

class BindParameterError : public Error {
public:
    BindParameterError(const std::string& what, size_t index)
    : Error{"Failed to bind paramter " + std::to_string(index) + ": " + what}, m_index{index} {}

    size_t getIndex() const { return m_index; }

private:
    size_t m_index;
};

class TypeMismatchError : public Error {
public:
    TypeMismatchError(size_t index)
    : Error{"Result type mismatch in column " + std::to_string(index)}, m_index{index} {}

    size_t getIndex() const { return m_index; }

private:
    size_t m_index;
};

} // namespace SQLite3pp
