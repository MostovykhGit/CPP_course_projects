#include <algorithm>
#include <cstring>
#include <iostream>

class String {
public:
  String(const char* ch) : str_(new char[strlen(ch) + 1]), size_(strlen(ch)), capacity_(strlen(ch) + 1) {
    memcpy(str_, ch, strlen(ch));
    str_[size_] = '\0';
  }

  String(int n, char ch) : str_(new char[n + 1]), size_(n), capacity_(n + 1) {
    memset(str_, ch, n);
    str_[size_] = '\0';
  }

  String(char* str_new, int sz_new, int cap_new): str_(str_new), size_(sz_new), capacity_(cap_new) {}

  String(): str_(new char[2]), size_(0), capacity_(2) {
    str_[0] = '\0';
  }

  String(const String& s): String(s.size_, '0') {
    memcpy(str_, s.str_, s.size_);
  }

  ~String() {
    delete[] str_;
  }

  String& operator=(const String& s) &{
    String copy = s;
    Swap_(copy);
    str_[size_] = '\0';
    return *this;
  }

  char& operator[](int n) {
    return *(str_ + n);
  }

  const char& operator[](int n) const {
    return *(str_ + n);
  }

  String substr(int start, int count) const {
    int new_cap = 1;
    while (capacity_ < count) {
      new_cap *= 2;
    }
    char* new_str = new char[new_cap];
    memcpy(new_str, str_ + start, count);
    String result(new_str, count, new_cap);
    return result;
  }

  String& operator+=(const String& s) {
    int new_size = size_ + s.size();
    while(capacity_ < new_size + 1) {
      capacity_ *= 2;
    }
    char* new_str = new char[capacity_];
    memcpy(new_str, str_, size_);
    memcpy(new_str + size_, s.str_, s.size());
    new_str[new_size] = '\0';
    delete[] str_;
    str_ = new_str;
    size_ = new_size;
    return *this;
  }
  
  String& operator+=(const char ch) {
    this->push_back(ch);
    return *this;
  }

  size_t length() const {
    return size_;
  }

  size_t size() const {
    return size_;
  }

  size_t capacity() const {
    return capacity_;
  }

  void push_back(char ch) {
    if (size_ + 1 < capacity_) {
      str_[size_] = ch;
      str_[size_ + 1] = '\0';
    } else {
      capacity_ *= 2;
      char* temp_str = new char[capacity_];
      memcpy(temp_str, str_, size_);
      temp_str[size_] = ch;
      delete[] str_;
      str_ = temp_str;
      str_[size_ + 1] = '\0';
    }
    ++size_;   
  }

  void pop_back() {
    str_[size_ - 1] = '\0';
    --size_;
  }

  char& front() {
    return str_[0];
  }

  const char& front() const {
    return str_[0];
  }

  char& back() {
    if (size_ == 0) {
      return str_[0];
    }
    return str_[size_ - 1];
  }

  const char& back() const {
    if (size_ == 0) {
      return str_[0];
    }
    return str_[size_ - 1];
  }

  size_t find(const String& substr) const {
    for (size_t i = 0; i <= static_cast<size_t>(size_) - substr.size(); ++i) {
      if (str_[i] == substr[0]) {
        size_t compare_index = 0;
        bool is_substr = true;
        while (compare_index < substr.size()) {
          if (str_[i + compare_index] != substr[compare_index]) {
            is_substr = false;
            break;
          }
          ++compare_index;
        }
        if (is_substr) {
          return i;
        }
      }
    }
    return size_;
  }

  size_t rfind(const String& substr) const {
    int ans = size_;
    for (size_t i = 0; i <= static_cast<size_t>(size_) - substr.size(); ++i) {
      if (str_[i] == substr[0]) {
        size_t compare_index = 0;
        bool is_substr = true;
        while (compare_index < substr.size()) {
          if (str_[i + compare_index] != substr[compare_index]) {
            is_substr = false;
            break;
          }
          ++compare_index;
        }
        if (is_substr) {
          ans = static_cast<int>(i);
        }
      }
    }
    return ans;
  }
  
  bool empty() const {
    return (size_ == 0);
  }

  void clear() {
    size_ = 0;
    str_[0] = '\0';
  }

  void shrink_to_fit() {
    char* new_str = new char[size_];
    memcpy(new_str, str_, size_);
    delete[] str_;
    str_ = new_str;
    capacity_ = size_;
  }

  char* data() const {
    return str_;
  }

private:
  void Swap_(String& s) {
    std::swap(capacity_, s.capacity_);
    std::swap(size_, s.size_);
    std::swap(str_, s.str_);
  }
  char* str_;
  int size_;
  int capacity_;
};

bool operator==(const String& s1, const String& s2) {
  if (s1.size() != s2.size()) {
    return false;
  }
  if (memcmp(&(s1[0]), &(s2[0]), s1.size()) == 0) { return true; }
  return false;
}
  
bool operator!=(const String& s1, const String& s2) {
  return !(s1 == s2);
}

bool operator<(const String& s1, const String& s2) {
  if (s1.size() != s2.size()) { return s1.size() < s2.size(); }
  if (memcmp(&(s1[0]), &(s2[0]), s1.size()) < 0) { return true; }
  return false;
}

bool operator>(const String& s1, const String& s2) {
  return (s2 < s1);
}

bool operator<=(const String& s1, const String& s2) {
  return !(s1 > s2);
}

bool operator>=(const String& s1, const String& s2) {
  return !(s1 < s2);
}

String operator+(const String& s1, const String& s2) {
  int new_cap = std::max(s1.capacity(), s2.capacity());
  int new_size = s1.size() + s2.size();
  while (new_cap < new_size + 1) {
    new_cap *= 2;
  }
  char* new_str = new char[new_cap];
  memcpy(new_str, s1.data(), s1.size());
  memcpy(new_str + s1.size(), s2.data(), s2.size());
  new_str[new_size] = '\0';
  String result(new_str, new_size, new_cap);
  return result;
}

String operator+(const String& s1, char ch2) {
  String result = s1;
  result.push_back(ch2);
  return result;
}

String operator+(char ch1, const String& s2) {
  String result;
  result.push_back(ch1);
  result += s2;
  return result;
}

std::ostream& operator<<(std::ostream& out, const String& s) {
  for (size_t i = 0; i < s.size(); ++i) {
    out << s[i];
  }
  return out;
}

std::istream& operator>>(std::istream& in, String& s) {
  s.clear();
  char ch;
  while(in.get(ch) && !std::isspace(ch)) {
    s.push_back(ch);
  }
  return in;
}
