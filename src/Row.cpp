#include <sqlite3pp/Error.h>
#include <sqlite3pp/Row.h>
#include <sqlite3.h>

namespace sqlite3pp {

std::size_t Row::get(std::size_t index, int& value) const {
    if (SQLITE_INTEGER != sqlite3_column_type(m_stmt, index)) {
        throw TypeMismatchError{index};
    }
    value = sqlite3_column_int(m_stmt, index);
    return index + 1;
}

std::size_t Row::get(std::size_t index, double& value) const {
    if (SQLITE_FLOAT != sqlite3_column_type(m_stmt, index)) {
        throw TypeMismatchError{index};
    }
    value = sqlite3_column_double(m_stmt, index);
    return index + 1;
}

std::size_t Row::get(std::size_t index, std::string& value) const {
    if (SQLITE_TEXT != sqlite3_column_type(m_stmt, index)) {
        throw TypeMismatchError{index};
    }
    const auto length = sqlite3_column_bytes(m_stmt, index);
    const auto* text = sqlite3_column_text(m_stmt, index);
    value.assign(text, text + length);
    return index + 1;
}

std::size_t Row::get(std::size_t index, std::vector<char>& value) const {
    if (SQLITE_BLOB != sqlite3_column_type(m_stmt, index)) {
        throw TypeMismatchError{index};
    }
    const auto length = sqlite3_column_bytes(m_stmt, index);
    const auto* data = static_cast<const char*>(sqlite3_column_blob(m_stmt, index));
    value.assign(data, data + length);
    return index + 1;
}

} // namespace sqlite3pp
