// src/core/string_instances.h
#pragma once
#include <string>
#include <istream>

extern template class std::basic_string<char>;

extern template std::basic_string<char>::basic_string(const char*);
extern template std::basic_string<char>& std::basic_string<char>::operator=(const char*);
extern template std::basic_string<char>& std::basic_string<char>::assign(const char*);
extern template std::basic_string<char>& std::basic_string<char>::append(const char*);
extern template std::basic_string<char>& std::basic_string<char>::append(const char*, size_t);
extern template std::basic_string<char>& std::basic_string<char>::append(size_t, char);

extern template std::istream& std::getline<char>(std::istream&, std::basic_string<char>&);
extern template std::istream& std::getline<char>(std::istream&, std::basic_string<char>&, char);

extern template class std::basic_string<wchar_t>;
extern template std::basic_string<wchar_t>::basic_string(const wchar_t*);
extern template std::basic_string<wchar_t>& std::basic_string<wchar_t>::operator=(const wchar_t*);
extern template std::basic_string<wchar_t>& std::basic_string<wchar_t>::assign(const wchar_t*);
extern template std::basic_string<wchar_t>& std::basic_string<wchar_t>::append(const wchar_t*);
extern template std::basic_string<wchar_t>& std::basic_string<wchar_t>::append(const wchar_t*, size_t);
extern template std::basic_string<wchar_t>& std::basic_string<wchar_t>::append(size_t, wchar_t);

extern template std::wistream& std::getline<wchar_t>(std::wistream&, std::basic_string<wchar_t>&);
extern template std::wistream& std::getline<wchar_t>(std::wistream&, std::basic_string<wchar_t>&, wchar_t);