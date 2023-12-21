#pragma once

#include <stdexcept>
#include <string>

class DatabaseError : public std::runtime_error {
public:
    DatabaseError(const std::string& what) noexcept : std::runtime_error(what) {}
};

class OpenDatabaseError : public DatabaseError {
public:
    OpenDatabaseError(std::string fileName)
    : DatabaseError{"Failed to open database: " + fileName}, m_fileName{std::move(fileName)} {}

    const std::string getFileName() const { return m_fileName; }

private:
    std::string m_fileName;
};

class PrepareStatementError : public DatabaseError {
public:
    PrepareStatementError(const std::string& what, std::string sql)
    : DatabaseError{"Failed to prepare statement: " + what}, m_sql{std::move(sql)} {}

    const std::string& getSql() const { return m_sql; }

private:
    std::string m_sql;
};

class BindParameterError : public DatabaseError {
public:
    BindParameterError(const std::string& what, size_t index)
    : DatabaseError{"Failed to bind paramter " + std::to_string(index) + ": " + what}, m_index{index} {}

    size_t getIndex() const { return m_index; }

private:
    size_t m_index;
};

class TypeMismatchError : public DatabaseError {
public:
    TypeMismatchError(size_t index)
    : DatabaseError("Result type mismatch in column " + std::to_string(index)), m_index{index} {}

    size_t getIndex() const { return m_index; }

private:
    size_t m_index;
};

