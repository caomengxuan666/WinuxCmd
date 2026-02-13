/*
 *  Copyright © 2026 [caomengxuan666]
 *  
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the “Software”), to deal
 *  in the Software without restriction, including without limitation the rights
 *  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *  copies of the Software, and to permit persons to whom the Software is furnished
 *  to do so, subject to the following conditions:
 *  
 *  The above copyright notice and this permission notice shall be included in all
 *  copies or substantial portions of the Software.
 *  
 *  THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 *  SOFTWARE.
 *  
 *  - File: string_instances.h
 *  - Username: Administrator
 *  - CopyrightYear: 2026
 */
// src/core/string_instances.h
#pragma once
#include <istream>
#include <string>

extern template class std::basic_string<char>;

extern template std::basic_string<char>::basic_string(const char*);
extern template std::basic_string<char>& std::basic_string<char>::operator=(
    const char*);
extern template std::basic_string<char>& std::basic_string<char>::assign(
    const char*);
extern template std::basic_string<char>& std::basic_string<char>::append(
    const char*);
extern template std::basic_string<char>& std::basic_string<char>::append(
    const char*, size_t);
extern template std::basic_string<char>& std::basic_string<char>::append(size_t,
                                                                         char);

extern template std::istream& std::getline<char>(std::istream&,
                                                 std::basic_string<char>&);
extern template std::istream& std::getline<char>(std::istream&,
                                                 std::basic_string<char>&,
                                                 char);

extern template class std::basic_string<wchar_t>;
extern template std::basic_string<wchar_t>::basic_string(const wchar_t*);
extern template std::basic_string<wchar_t>&
std::basic_string<wchar_t>::operator=(const wchar_t*);
extern template std::basic_string<wchar_t>& std::basic_string<wchar_t>::assign(
    const wchar_t*);
extern template std::basic_string<wchar_t>& std::basic_string<wchar_t>::append(
    const wchar_t*);
extern template std::basic_string<wchar_t>& std::basic_string<wchar_t>::append(
    const wchar_t*, size_t);
extern template std::basic_string<wchar_t>& std::basic_string<wchar_t>::append(
    size_t, wchar_t);

extern template std::wistream& std::getline<wchar_t>(
    std::wistream&, std::basic_string<wchar_t>&);
extern template std::wistream& std::getline<wchar_t>(
    std::wistream&, std::basic_string<wchar_t>&, wchar_t);
