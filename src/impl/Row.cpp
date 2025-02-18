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
#include <sqlite3.h>
#include <sqlite3pp/Error.hpp>
#include <sqlite3pp/Row.hpp>

namespace sqlite3pp {

std::size_t Row::get(std::size_t index, int& value) const {
    if (SQLITE_INTEGER != sqlite3_column_type(m_stmt, static_cast<int>(index))) {
        throw TypeMismatchError{index};
    }
    value = sqlite3_column_int(m_stmt, static_cast<int>(index));
    return index + 1;
}

std::size_t Row::get(std::size_t index, double& value) const {
    if (SQLITE_FLOAT != sqlite3_column_type(m_stmt, static_cast<int>(index))) {
        throw TypeMismatchError{index};
    }
    value = sqlite3_column_double(m_stmt, static_cast<int>(index));
    return index + 1;
}

std::size_t Row::get(std::size_t index, std::string& value) const {
    if (SQLITE_TEXT != sqlite3_column_type(m_stmt, static_cast<int>(index))) {
        throw TypeMismatchError{index};
    }
    const auto* text = sqlite3_column_text(m_stmt, static_cast<int>(index));
    const auto length = sqlite3_column_bytes(m_stmt, static_cast<int>(index));
    value.assign(text, text + length); // NOLINT: bridge to C-code
    return index + 1;
}

std::size_t Row::get(std::size_t index, Blob& value) const {
    if (SQLITE_BLOB != sqlite3_column_type(m_stmt, static_cast<int>(index))) {
        throw TypeMismatchError{index};
    }
    const auto* data = static_cast<Blob::const_pointer>(sqlite3_column_blob(m_stmt, static_cast<int>(index)));
    const auto length = sqlite3_column_bytes(m_stmt, static_cast<int>(index));
    value.assign(data, data + length); // NOLINT: bridge to C-code
    return index + 1;
}

} // namespace sqlite3pp
