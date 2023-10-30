#include <algorithm>
#include <iostream>
#include <vector>
#include <string>

const long long BASE = 1000 * 1000 * 1000;

class BigInteger;
bool operator<(const BigInteger& num1, const BigInteger& num2);
bool operator>(const BigInteger& num1, const BigInteger& num2);
bool operator<=(const BigInteger& num1, const BigInteger& num2);
bool operator>=(const BigInteger& num1, const BigInteger& num2);
bool operator==(const BigInteger& num1, const BigInteger& num2);
bool operator!=(const BigInteger& num1, const BigInteger& num2);
BigInteger operator+(const BigInteger& num1, const BigInteger& num2);
BigInteger operator-(const BigInteger& num1, const BigInteger& num2);
BigInteger operator*(const BigInteger& num1, const BigInteger& num2);
BigInteger operator/(const BigInteger& num1, const BigInteger& num2);
BigInteger operator%(const BigInteger& num1, const BigInteger& num2);
BigInteger operator""_bi(const char* x, size_t sz);
BigInteger operator""_bi(unsigned long long);
std::ostream& operator<<(std::ostream& out, const BigInteger& num);
std::istream& operator>>(std::istream& in, BigInteger& num);

class BigInteger {
 private:
  void remove_lead_zeros() {
    while (digits_.size() > 1 && digits_.back() == 0) {
      digits_.pop_back();
    }
  }

  bool absolutely_geqslant(const BigInteger& num) {
    if (digits_.size() < num.digits_.size()) return false;
    if (digits_.size() > num.digits_.size()) return true;
    for (long long i = digits_.size() - 1; i >= 0; --i) {
      if (digits_[i] < num.digits_[i]) return false;
      if (digits_[i] > num.digits_[i]) return true;
    }
    return true;
  }

  bool is_positive_;
  std::vector<long long> digits_;
  friend bool operator<(const BigInteger&, const BigInteger&);
  friend std::istream& operator>>(std::istream&, BigInteger&);

 public:
  BigInteger(int x) {
    is_positive_ = (x >= 0);
    x = std::abs(x);
    if (x == 0) digits_.push_back(0);
    while (x > 0) {
      digits_.push_back(x % BASE);
      x /= BASE;
    }
  }

  BigInteger(const BigInteger& num): is_positive_(num.is_positive_), digits_(num.digits_) {};

  BigInteger(const std::string& s) {
    bool have_to_change = false;
    if (s[0] == '-') {
      is_positive_ = false;
      have_to_change = true;
    } else {
      is_positive_ = true;
    }
    digits_ = {};
    long long i = s.size() - 1;
    while (i >= static_cast<long long>(have_to_change)) {
      std::string block;
      while (i >= static_cast<long long>(have_to_change) && block.size() < 9) {
        block.push_back(s[i]);
        --i;
      }
      reverse(block.begin(), block.end());
      digits_.push_back(std::stoi(block));
    }
    remove_lead_zeros();
  }

  BigInteger() : is_positive_(true), digits_({0}) {};

  BigInteger(bool new_is_positive, std::vector<long long> new_digits) : is_positive_(new_is_positive), digits_(new_digits) {}

  BigInteger& operator=(const BigInteger& num) {
    BigInteger copy(num);
    swap(copy);
    return *this;
  }

  void swap(BigInteger& num) {
    std::swap(is_positive_, num.is_positive_);
    std::swap(digits_, num.digits_);
  }

  explicit operator bool() const {
    return !(digits_.size() == 1 && digits_[0] == 0);
  }

  explicit operator int() const {
    long long x;
    if (digits_.size() > 1) {
      x = digits_[1] * BASE + digits_[0];
    } else {
      x = digits_[0];
    }
    if (!is_positive_) x = -x;
    return static_cast<int>(x);
  }
  
  std::string toString() const {
    std::string result;
    result.reserve(digits_.size() * 10);
    if (!is_positive_) result += '-';
    for (long long i = digits_.size() - 1; i >= 0; --i) {
      std::string temp = std::to_string(digits_[i]);
      if (i != static_cast<long long>(digits_.size() - 1)) temp = std::string(9 - temp.size(), '0') + temp;
      result += temp;
    }
    return result;
  }

  BigInteger operator-() const {
    BigInteger copy = *this;
    if (digits_.size() == 1 && digits_[0] == 0) {
      return copy;
    }
    copy.is_positive_ = !(copy.is_positive_);
    return copy;
  }

  BigInteger& operator+=(const BigInteger& num) {
    if (is_positive_ == num.is_positive_) {
      digits_.resize(std::max(digits_.size(), num.digits_.size()) + 1);
      long long carry = 0;
      for (size_t i = 0; i < digits_.size(); ++i) {
        long long prev_carry = carry;
        if (i < num.digits_.size()) {
          carry = (digits_[i] + num.digits_[i] + carry) / BASE;
          digits_[i] = (digits_[i] + num.digits_[i] + prev_carry) % BASE;
        } else {
          carry = (digits_[i] + carry) / BASE;
          digits_[i] = (digits_[i] + prev_carry) % BASE;    
        }
      }
      remove_lead_zeros();
      return *this;
    } else {
      if (is_positive_) {
        if (absolutely_geqslant(num)) {
          return *this -= (-num);
        } else {
          BigInteger copy = -num;
          copy -= *this;
          copy.is_positive_ = false;
          *this = copy;
        }
      } else {
        if (absolutely_geqslant(num)) {
          is_positive_ = true;
          *this -= num;
          is_positive_ = false;
        } else {
          BigInteger copy = num;
          copy -= (-*this);
          *this = copy;
        }
      }
    }
    return *this;
  }

  BigInteger& operator-=(const BigInteger& num) {
    if (is_positive_ == num.is_positive_) {
      long long type = 0;
      if (is_positive_) {
        if (absolutely_geqslant(num)) {
          type = 1;
          is_positive_ = true;
        } else {
          type = 2;
          is_positive_ = false;
        }
      } else {
        if (absolutely_geqslant(num)) {
          type = 1;
          is_positive_ = false;
        } else {
          type = 2;
          is_positive_ = true;
        }
      }
      if (type == 1) {
        long long carry = 0;
        for (size_t i = 0; i < num.digits_.size(); ++i) {
          if (digits_[i] - carry >= num.digits_[i]) {
            digits_[i] -= (carry + num.digits_[i]);
            carry = 0;
          } else {
            digits_[i] = BASE + digits_[i] - carry - num.digits_[i];
            carry = 1;
          }
        }
        for (size_t i = num.digits_.size(); i < digits_.size(); ++i) {
          if (digits_[i] >= carry) {
            digits_[i] -= carry;
            carry = 0;
          } else {
            digits_[i] = BASE - carry;
            carry = 1;
          }
        }
      }
      if (type == 2) {
        long long carry = 0;
        std::vector<long long> less_num = digits_;
        digits_ = num.digits_;
        for (size_t i = 0; i < less_num.size(); ++i) {
          if (digits_[i] - carry >= less_num[i]) {
            digits_[i] -= (carry + less_num[i]);
            carry = 0;
          } else {
            digits_[i] = BASE + digits_[i] - carry - less_num[i];
            carry = 1;
          }
        }
        for (size_t i = less_num.size(); i < digits_.size(); ++i) {
          if (digits_[i] >= carry) {
            digits_[i] -= carry;
            carry = 0;
          } else {
            digits_[i] = BASE - carry;
            carry = 1;
          }
        }
      }
    } else {
      *this += (-num);
    }
    remove_lead_zeros();
    return *this;
  }

  BigInteger& operator*=(const BigInteger& num) {
    if (digits_.size() == 1 && digits_[0] == 0) return *this;
    if (num.digits_.size() == 1 && num.digits_[0] == 0) {
      *this = num;
      return *this;
    }
    BigInteger result = 0;
    result.is_positive_ = (is_positive_ == num.is_positive_);
    result.digits_.resize(digits_.size() + num.digits_.size());
    long long carry = 0;
    for (size_t i = 0; i < digits_.size(); ++i) {
      carry = 0;
      for (size_t j = 0; j < num.digits_.size() || carry; ++j) {
        long long mult = result.digits_[i + j] + 1ll * digits_[i] * (j < num.digits_.size() ? num.digits_[j] : 0) + carry;
        result.digits_[i + j] = static_cast<long long>(mult % BASE);
        carry = static_cast<long long>(mult / BASE);
      }
    }
    *this = result;
    remove_lead_zeros();
    return *this;
  }

  BigInteger& operator/=(const BigInteger& input_num) {
    BigInteger abs_num = (input_num.is_positive_ ? input_num : -input_num);
    if (digits_.size() == 1 && digits_[0] == 0) return *this;
    if ((is_positive_ ? *this : -(*this)) < abs_num) {
      *this = 0_bi;
      is_positive_ = true;
      return *this;
    }
    BigInteger cur = 0;
    std::vector<long long> res_digits;
    BigInteger carry = 0;
    for (long long i = digits_.size() - 1; i >= 0; i--) {
      cur = BigInteger(digits_[i]) + carry * BigInteger(BASE);
      if (cur < abs_num) {
        if (res_digits.size() == 0) {
          carry = cur;
          continue;
        }
        res_digits.push_back(0);
        carry = cur;
        continue;
      }
      long long left = 0;
      long long right = BASE;
      while (right - left > 1) {
        long long mid = (right + left) / 2;
        if (cur >= BigInteger(mid) * abs_num) {
          left = mid;
        } else {
          right = mid;
        }
      }
      res_digits.push_back(left);
      carry = cur - BigInteger(left) * abs_num;
    }
    reverse(res_digits.begin(), res_digits.end());
    is_positive_ = (is_positive_ == input_num.is_positive_);
    while (res_digits.size() > 1 && res_digits.back() == 0) {
      res_digits.pop_back();
    }
    digits_ = res_digits;
    return *this;
  }

  BigInteger& operator%=(const BigInteger& num) {
    *this = *this - (*this / num) * num;
    return *this;
  }

  BigInteger& operator++() {
    *this += 1;
    return *this;
  }

  BigInteger operator++(int) {
    BigInteger copy = *this;
    *this += 1;
    return copy;
  }

  BigInteger& operator--() {
    *this -= 1;
    return *this;
  }

  BigInteger operator--(int) {
    BigInteger copy = *this;
    *this -= 1;
    return copy;
  }

  BigInteger gcd(BigInteger num2) {
    BigInteger num1 = *this;
    if (num1 < BigInteger(0)) num1 = (-num1);
    if (num2 < BigInteger(0)) num2 = (-num2);
    while (num2 != BigInteger(0)) {
      BigInteger temp = num1 % num2;
      num1 = num2;
      num2 = temp;
    }
    return num1;
  }

  ~BigInteger() {};
};

bool operator<(const BigInteger& num1, const BigInteger& num2) {
  if (num1.is_positive_ != num2.is_positive_) return num2.is_positive_;
  if (num1.digits_.size() < num2.digits_.size()) return num1.is_positive_;
  if (num1.digits_.size() > num2.digits_.size()) return !num1.is_positive_;
  for (long long i = num1.digits_.size() - 1; i >= 0; --i) {
    if (num1.digits_[i] < num2.digits_[i]) return num1.is_positive_;
    if (num1.digits_[i] > num2.digits_[i]) return !num1.is_positive_;
  }
  return false;
}

bool operator>(const BigInteger& num1, const BigInteger& num2) {
  return (num2 < num1);
}

bool operator<=(const BigInteger& num1, const BigInteger& num2) {
  return !(num1 > num2);
}

bool operator>=(const BigInteger& num1, const BigInteger& num2) {
  return !(num1 < num2);
}

bool operator==(const BigInteger& num1, const BigInteger& num2) {
  return !(num1 < num2 || num1 > num2);
}

bool operator!= (const BigInteger& num1, const BigInteger& num2) {
  return (num1 > num2 || num1 < num2);
}

BigInteger operator+(const BigInteger& num1, const BigInteger& num2) {
  BigInteger copy = num1;
  copy += num2;
  return copy;
}

BigInteger operator-(const BigInteger& num1, const BigInteger& num2) {
  BigInteger copy = num1;
  copy -= num2;
  return copy;
}

BigInteger operator*(const BigInteger& num1, const BigInteger& num2) {
  BigInteger copy = num1;
  copy *= num2;
  return copy;
}

BigInteger operator/(const BigInteger& num1, const BigInteger& num2) {
  BigInteger copy = num1;
  copy /= num2;
  return copy;
}

BigInteger operator%(const BigInteger& num1, const BigInteger& num2) {
  BigInteger copy = num1;
  copy %= num2;
  return copy;
}

std::ostream& operator<<(std::ostream& out, const BigInteger& num) {
  std::string s = num.toString();
  for (size_t i = 0; i < s.size(); ++i) {
    out << s[i];
  }
  return out;
}

std::istream& operator>>(std::istream& in, BigInteger& num) {
  std::string s;
  in >> s;
  num = BigInteger(s);
  return in;
}

BigInteger operator""_bi(const char* x, size_t sz) {
  std::string s(x);
  reverse(s.begin(), s.end());
  bool res_is_positive;
  std::vector<long long> res_digits;
  if (s[sz - 1] == '-') {
    res_is_positive = false;
    s.pop_back();
  } else {
    res_is_positive = true;
  }
  size_t i = 0;
  while (i < s.size()) {
    std::string block; 
    while (i < s.size() && block.size() < 9) {
      block.push_back(s[i]);
      ++i;
    }
    reverse(block.begin(), block.end());
    res_digits.push_back(std::stoi(block));
  }
  BigInteger result = BigInteger(res_is_positive, res_digits);
  return result;
}

BigInteger operator""_bi(unsigned long long x) {
  std::vector<long long> res_digits;
  if (x == 0) res_digits.push_back(0);
  while (x > 0) {
    res_digits.push_back(x % BASE);
    x /= BASE;
  }
  BigInteger result = BigInteger(true, res_digits);
  return result;
}

class Rational {
 private:
  BigInteger numerator;
  BigInteger denumerator = 1;
  friend bool operator<(Rational, Rational);
 public:
  Rational(const BigInteger& x) : numerator(x), denumerator(1) {}

  Rational(const BigInteger& x, const BigInteger& y) {
    if (x >= 0 == y >= 0) {
      numerator = (x >= 0 ? x : -x);
      denumerator = (y >= 0 ? y : -y);
    } else {
      numerator = (x >= 0 ? -x : x);
      denumerator = (y >= 0 ? y : -y);
    }
  }

  Rational(int x) : numerator(x), denumerator(1) {}

  Rational() : numerator(0), denumerator(1) {}

  Rational(const Rational& num) : numerator(num.numerator), denumerator(num.denumerator) {}

  explicit operator double() const {
    return std::stod(asDecimal(50));
  }

  Rational& operator=(const Rational& num) {
    numerator = num.numerator;
    denumerator = num.denumerator;
    return *this;
  }

  Rational operator-() const {
    Rational copy = *this;
    copy.numerator = (-copy.numerator);
    return copy;
  }

  Rational& operator+=(const Rational& num) {
    Rational res;
    res.denumerator = denumerator * num.denumerator;
    res.numerator = numerator * num.denumerator + num.numerator * denumerator;
    BigInteger gcd = res.numerator.gcd(res.denumerator);
    res.numerator /= gcd;
    res.denumerator /= gcd;
    *this = res;
    return *this;
  }

  Rational& operator-=(const Rational& num) {
    *this += (-num);
    return *this;
  }

  Rational& operator*=(const Rational& num) {
    Rational res;
    res.numerator = numerator * num.numerator;
    res.denumerator = denumerator * num.denumerator;
    BigInteger gcd = res.numerator.gcd(res.denumerator);
    res.numerator /= gcd;
    res.denumerator /= gcd;
    *this = res;
    return *this;
  }

  Rational& operator/=(const Rational& num) { 
    Rational res;
    res.numerator = numerator * num.denumerator;
    res.denumerator = denumerator * num.numerator;
    if ((res.numerator > BigInteger(0)) == (res.denumerator > BigInteger(0))) {
      if (res.numerator < BigInteger(0)) {
        res.numerator = (-res.numerator);
        res.denumerator = (-res.denumerator);
      }
    } else {
      if (res.numerator > BigInteger(0)) {
        res.numerator = (-res.numerator);
        res.denumerator = (-res.denumerator);
      }
    }
    BigInteger gcd = res.numerator.gcd(res.denumerator);
    res.numerator /= gcd;
    res.denumerator /= gcd;
    *this = res;
    return *this;
  }

  std::string toString() {
    std::string res;
    res += numerator.toString();
    if (denumerator == BigInteger(1)) {
      return res;
    }
    res += '/';
    res += denumerator.toString();
    return res;

    
  }

  std::string asDecimal(int precision = 0) const {
    if (numerator == 0) {
      return "0";
    }
    std::string s;
    if (numerator < 0_bi) s += '-';
    BigInteger a = numerator / denumerator;
    if (a < 0) a = -a; 
    BigInteger temp_num = (numerator > 0_bi ? numerator : -numerator);
    BigInteger temp_den = denumerator;
    temp_num %= temp_den;
    s += a.toString();
    std::string b;
    for (long long i = 0; i < precision; ++i) {
      temp_num *= 10;
      b += (temp_num / temp_den).toString();
      if (temp_num / temp_den != 0_bi) {
        temp_num %= temp_den;
      }
    }
    if (precision > 0) {
      s += ".";
      s += b;
    }
    return s;
  }

  ~Rational() {}

};

Rational operator+(Rational num1, const Rational& num2) {
  num1 += num2;
  return num1;
}
  
Rational operator-(Rational num1, const Rational& num2) {
  num1 -= num2;
  return num1;
}

Rational operator*(Rational num1, const Rational& num2) {
  num1 *= num2;
  return num1;
}

Rational operator/(Rational num1, const Rational& num2) {
  num1 /= num2;
  return num1;
}

bool operator<(Rational num1, Rational num2) {
  if ((num1.numerator >= BigInteger(0)) != (num2.numerator >= BigInteger(0))) {
    return num2.numerator >= BigInteger(0);
  }
  num1.numerator *= num2.denumerator;
  num2.numerator *= num1.denumerator;
  return (num1.numerator < num2.numerator);
}

bool operator>(const Rational& num1, const Rational& num2) {
  return (num2 < num1);
}

bool operator<=(const Rational& num1, const Rational& num2) {
  return !(num1 > num2);
}

bool operator>=(const Rational& num1, const Rational& num2) {
  return !(num1 < num2);
}

bool operator==(const Rational& num1, const Rational& num2) {
  return !(num1 > num2 || num1 < num2);
}

bool operator!=(const Rational& num1, const Rational& num2) {
  return !(num1 == num2);
}
