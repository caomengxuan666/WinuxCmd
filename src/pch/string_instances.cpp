#include "pch/string_instances.h"

template class std::basic_string<char>;

template std::basic_string<char>::basic_string(const char*);
template std::basic_string<char>& std::basic_string<char>::operator=(const char*);
template std::basic_string<char>& std::basic_string<char>::assign(const char*);
template std::basic_string<char>& std::basic_string<char>::append(const char*);
template std::basic_string<char>& std::basic_string<char>::append(const char*, size_t);
template std::basic_string<char>& std::basic_string<char>::append(size_t, char);

template std::istream& std::getline<char>(std::istream&, std::basic_string<char>&);
template std::istream& std::getline<char>(std::istream&, std::basic_string<char>&, char);

template class std::basic_string<wchar_t>;
template std::basic_string<wchar_t>::basic_string(const wchar_t*);
template std::basic_string<wchar_t>& std::basic_string<wchar_t>::operator=(const wchar_t*);
template std::basic_string<wchar_t>& std::basic_string<wchar_t>::assign(const wchar_t*);
template std::basic_string<wchar_t>& std::basic_string<wchar_t>::append(const wchar_t*);
template std::basic_string<wchar_t>& std::basic_string<wchar_t>::append(const wchar_t*, size_t);
template std::basic_string<wchar_t>& std::basic_string<wchar_t>::append(size_t, wchar_t);

template std::wistream& std::getline<wchar_t>(std::wistream&, std::basic_string<wchar_t>&);
template std::wistream& std::getline<wchar_t>(std::wistream&, std::basic_string<wchar_t>&, wchar_t);