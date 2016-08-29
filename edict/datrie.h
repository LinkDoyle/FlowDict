#ifndef _DATRIE_HPP_
#define _DATRIE_HPP_

#include <cstdio>
#include <cassert>
#include <cstring>
#include <vector>
#include <string>
/*
template<typename CHAR>
class IAlphabet
{
public:
    //returns value must less than (uint32_t)-2;
    static uint32_t toIndex(CHAR c);
    static CHAR toChar(uint32_t index);
    static uint32_t begin();
    static uint32_t end();
};
*/

template <typename CHAR, class Alphabet>
class DATrieBase {
 public:
  DATrieBase()
      : size_(0),
        tailsize_(0),
        pos_(0),
        base_(nullptr),
        check_(nullptr),
        tail_(nullptr) {}
  ~DATrieBase() {}
  size_t getDataSize() const { return size_ * 2 + tailsize_; }
  void print_memory() {
    printf("index\tbase\tcheck\tch\ttail\n");
    for (int i = size_; i >= 0; --i) {
      if (base_[i - 1] != 0 || check_[i - 1] != 0) {
        for (size_t j = 1; j <= i; ++j) {
          if (base_[j - 1] || check_[j - 1]) {
            int x = j - 1 - base_[check_[j - 1] - 1];
            printf("%zd\t%d\t%d\t%c", j, base_[j - 1], check_[j - 1],
                   x == 0 ? '\2' : Alphabet::toChar(x - 1));
            if (base_[j - 1] < 0) printf("\t%s", tail_ - base_[j - 1] - 1);
            putchar('\n');
          }
        }
        break;
      }
    }
  }

 protected:
  uint32_t size_;
  uint32_t tailsize_;
  int32_t pos_;
  int32_t* base_;
  int32_t* check_;
  CHAR* tail_;
  // return value must greater than 0
  uint32_t getIndex(CHAR c) const {
    assert(c == 0 || Alphabet::toIndex(c) < (uint32_t)-2);
    assert(c == 0 || Alphabet::toIndex(c) < Alphabet::end());
    return c == 0 ? 1 : Alphabet::toIndex(c) + 2;
  }
};

template <typename CHAR, class Alphabet>
class DATrieReader;
template <typename CHAR, class Alphabet>
class DATrieWriter;

template <typename CHAR, class Alphabet>
class DATrieWriter : public DATrieBase<CHAR, Alphabet> {
 public:
  DATrieWriter() {
    size_ = 1024;
    tailsize_ = 1024;
    base_ = new int[size_];
    memset(base_, 0, sizeof(*base_) * size_);
    check_ = new int[size_];
    memset(check_, 0, sizeof(*check_) * size_);
    tail_ = new CHAR[tailsize_];
    base_[0] = 1;
  }
  ~DATrieWriter() {
    if (base_) delete base_;
    if (check_) delete check_;
    if (tail_) delete tail_;
  }
  // word ends with a null terminator.
  void insert(const CHAR* word, uint32_t data = 0) {
    const CHAR* p = word - 1;
    int s = 1;
    int t;
    do {
      ++p;
      t = base(s) + getIndex(*p);
      if (check(t) != s) {
        if (check(t) != 0) {
          // resolve collision
          std::vector<CHAR> list0;
          std::vector<CHAR> list1;
          get_list(s, list0);
          get_list(check(t), list1);
          if (list0.size() + 1 < list1.size()) {
            list0.push_back(*p);
            s = relocate(s, s, list0, true);
          } else {
            s = relocate(s, check(t), list1, false);
          }
        }
        t = base(s) + getIndex(*p);
        check(t) = s;
        base(t) = -pos_ - 1;
        write_tail((*p ? p + 1 : nullptr), data);
        return;
      }
      if (base(t) < 0) {
        break;
      }
      s = t;
    } while (*p);

    int old_base_value;
    CHAR* q = nullptr;
    if (*p) {
      old_base_value = base(t);
      q = tail_ - old_base_value - 1;
    }
    if (*p == 0 || strcmp(p + 1, q) == 0) return;
    if (base(t) != 0) {
      // tail insert
      s = t;
      p = p + 1;
      std::vector<CHAR> list{0, 0};

      while (*p == *q) {
        list[0] = *p;
        base(s) = x_check(list);
        t = base(s) + getIndex(*p);
        check(t) = s;
        s = t;
        ++p;
        ++q;
      }
      list[0] = *p;
      list[1] = *q;
      base(s) = x_check(list);

      // move the remaining string in tail_.
      {
        int t = base(s) + getIndex(*q);
        check(t) = s;
        base(t) = old_base_value;
        CHAR* src = q + 1;
        CHAR* dst = tail_ - old_base_value - 1;
        if (*q) {
          while (*src) *dst++ = *src++;
          *dst++ = *src++;
        }
        *reinterpret_cast<uint32_t*>(dst) = *reinterpret_cast<uint32_t*>(src);
      }

      // for the remaining part of the word.
      {
        int t = base(s) + getIndex(*p);
        check(t) = s;
        base(t) = -pos_ - 1;
        write_tail((*p ? p + 1 : nullptr), data);
      }
    }
  }

  int dumpToFile(const char* filename, const char* extraData,
                 uint64_t extraSize) const {
    FILE* pFile = fopen(filename, "wb");
    if (!pFile) return -1;
    fwrite(&size_, sizeof(size_), 1, pFile);
    fwrite(&pos_, sizeof(tailsize_), 1, pFile);
    fwrite(&pos_, sizeof(pos_), 1, pFile);
    fwrite(base_, sizeof(*base_), size_, pFile);
    fwrite(check_, sizeof(*check_), size_, pFile);
    fwrite(tail_, sizeof(*tail_), pos_, pFile);
    fwrite(&extraSize, sizeof(extraSize), 1, pFile);
    fwrite(extraData, sizeof(*extraData), extraSize, pFile);
    fclose(pFile);
    return 0;
  }

 protected:
  friend class DATrieReader<CHAR, Alphabet>;
  void get_list(int s, std::vector<CHAR>& list) {
    if (check(base(s) + 1) == s) list.push_back(0);  // add '\0' to the list
    for (uint32_t index = Alphabet::begin(); index != Alphabet::end();
         ++index) {
      if (check(base(s) + index + 2) == s)
        list.push_back(Alphabet::toChar(index));
    }
  }
  int relocate(int current, int s, std::vector<CHAR>& list, bool isPushed) {
    int old_base = base(s);
    base(s) = x_check(list);
    if (isPushed) list.pop_back();
    for (const auto ch : list) {
      int old_node = old_base + getIndex(ch);
      int new_node = base(s) + getIndex(ch);
      base(new_node) = base(old_node);
      check(new_node) = s;
      if (base(old_node) > 0) {
        int& check_value = check(base(old_node) + 1);
        if (check_value == old_node) check_value = new_node;  // copy for '\0'
        for (uint32_t index = Alphabet::begin(); index != Alphabet::end();
             ++index) {
          int& check_value = check(base(old_node) + index + 2);
          if (check_value == old_node) {
            check_value = new_node;
          }
        }
      }
      if (current != s && old_node == current) current = new_node;
      base(old_node) = 0;
      check(old_node) = 0;
    }
    return current;
  }
  void write_tail(const CHAR* word, uint32_t data) {
    uint32_t word_size = 0;
    if (word) {
      const CHAR* p = word;
      while (*p++) ++word_size;
      word_size += 1;  // includes the null terminator
    }
    uint32_t needed_size = pos_ + word_size + sizeof(data);
    if (needed_size > tailsize_) {
      uint32_t new_size = tailsize_ * 2;
      while (needed_size > new_size) new_size *= 2;
      CHAR* temp = new CHAR[new_size];
      memcpy(temp, tail_, tailsize_);
      delete tail_;
      tail_ = temp;
      tailsize_ = new_size;
    }
    memcpy(tail_ + pos_, word, (word_size) * sizeof(CHAR));
    pos_ += word_size;
    *reinterpret_cast<uint32_t*>(tail_ + pos_) = data;
    pos_ += sizeof(data);
  }
  void resize(uint32_t size) {
    uint32_t new_size = size_ * 2;
    while (size > new_size) new_size *= 2;

    int* temp_base = new int[new_size];
    memcpy(temp_base, base_, size_ * sizeof(*base_));
    memset(temp_base + size_, 0, (new_size - size_) * sizeof(*base_));
    delete base_;
    base_ = temp_base;

    int* temp_check = new int[new_size];
    memcpy(temp_check, check_, size_ * sizeof(*check_));
    memset(temp_check + size_, 0, (new_size - size_) * sizeof(*check_));
    delete check_;
    check_ = temp_check;

    size_ = new_size;
  }
  // index = 1...size_
  int32_t& base(uint32_t index) {
    if (index > size_) resize(index);
    return base_[index - 1];
  }
  // index = 1...size_
  int32_t& check(uint32_t index) {
    if (index > size_) resize(index);
    return check_[index - 1];
  }
  int x_check(const std::vector<CHAR>& list) {
    assert(!list.empty());
    int q = 1;
    int i = 0;
    do {
      if (check(q + getIndex(list[i++])) != 0) {
        ++q;
        i = 0;
        continue;
      }
    } while (i != list.size());
    return q;
  }
};

template <typename CHAR, class Alphabet>
class DATrieReader : public DATrieBase<CHAR, Alphabet> {
 public:
  DATrieReader() {}
  int loadFromMemory(CHAR* data) {
    CHAR* pos = data;
    size_ = *reinterpret_cast<uint32_t*>(pos);
    pos += sizeof(uint32_t);
    tailsize_ = *reinterpret_cast<uint32_t*>(pos);
    pos += sizeof(uint32_t);
    pos_ = *reinterpret_cast<uint32_t*>(pos);
    pos += sizeof(uint32_t);

    base_ = reinterpret_cast<int32_t*>(pos);
    pos += size_ * sizeof(int32_t);
    check_ = reinterpret_cast<int32_t*>(pos);
    pos += size_ * sizeof(int32_t);
    tail_ = pos;
    pos += tailsize_ * sizeof(CHAR);

    extraSize_ = *reinterpret_cast<uint64_t*>(pos);
    pos += sizeof(extraSize_);
    extraData_ = pos;
    pos += extraSize_;
    return 0;
  }
  int loadFromWriter(const DATrieWriter<CHAR, Alphabet>& writer) {
    size_ = writer.size_;
    tailsize_ = writer.tailsize_;
    pos_ = writer.pos_;
    base_ = writer.base_;
    check_ = writer.check_;
    tail_ = writer.tail_;

    extraSize_ = 0;
    extraData_ = 0;
    return 1;
  }
  int retrieve(const CHAR* word) const {
    uint32_t unused;
    return retrieve(word, unused);
  }
  // return the position to data,
  // word ends with a null terminator,
  // return the data using the parameter data.
  int retrieve(const CHAR* word, uint32_t& data) const {
    const CHAR* p = word - 1;
    int32_t s = 1;
    int32_t t;
    do {
      ++p;
      t = base(s) + getIndex(*p);
      if (check(t) != s) return -1;
      if (base(t) < 0) break;
      s = t;
    } while (*p);

    CHAR* q = nullptr;
    int pos = -base(t) - 1;
    if (*p) {
      q = tail_ + pos;
      if (strcmp(p + 1, q) == 0) {
        int offset = pos + strlen(q) + 1;
        data = *reinterpret_cast<uint32_t*>(tail_ + offset);
        return offset;
      }
    } else {
      data = *reinterpret_cast<uint32_t*>(tail_ + pos);
      return pos;
    }
    return -1;
  }
  uint64_t getExtraSize() const { return extraSize_; }
  const char* getExtraData() const { return extraData_; }

  std::vector<std::basic_string<CHAR>> keysWithPrefix(const CHAR* prefix,
                                                      int max = -1) const {
    std::vector<std::basic_string<CHAR>> v;
    if (max == 0) return v;
    if (max > 0) v.reserve(max);
    const CHAR* p = prefix;
    int32_t s = 1;
    int32_t t;
    while (*p) {
      t = base(s) + getIndex(*p);
      if (check(t) != s) return v;
      s = t;
      ++p;
      if (base(s) < 0) {
        const CHAR* tail = tail_ - base(s) - 1;
        while (*p != NULL && *p == *tail) {
          ++p;
          ++tail;
        };
        if (*p == NULL) v.push_back(std::basic_string<CHAR>{prefix} + tail);
        return v;
        break;
      }
    }
    collect(s, std::basic_string<CHAR>(prefix), v, max);
    return v;
  }
  std::vector<std::basic_string<CHAR>> keysThatMatch(const CHAR* pattern,
                                                     int max = -1) const {
    std::vector<std::basic_string<CHAR>> v;
    if (max == 0) return v;
    if (max > 0) v.reserve(max);
    std::basic_string<CHAR> prefix;
    collect(1, prefix, pattern, v, max);
    return v;
  }

 protected:
  void collect(int32_t node, std::basic_string<CHAR>& String,
               std::vector<std::basic_string<CHAR>>& v, int count) const {
    if (count == v.size()) return;
    if (base(node) < 0) {
      const CHAR* q = tail_ - base(node) - 1;
      v.push_back(String + q);
      return;
    }
    if (check(base(node) + 1) == node) {
      v.push_back(String);
    }
    for (uint32_t index = Alphabet::begin(); index != Alphabet::end();
         ++index) {
      int32_t next = base(node) + index + 2;
      if (check(next) == node) {
        String.push_back(Alphabet::toChar(index));
        collect(next, String, v, count);
        String.pop_back();
      }
    }
  }

  CHAR getWildcard() const {
    if (std::is_same_v<CHAR, char>)
      return '?';
    else if (std::is_same_v<CHAR, wchar_t>)
      return L'?';
    else if (std::is_same_v<CHAR, char16_t>)
      return u'?';
    else if (std::is_same_v<CHAR, char32_t>)
      return U'?';
    else
      assert(false);
    return 0;
  }

  void collect(int32_t node, std::basic_string<CHAR>& prefix,
               const CHAR* pattern, std::vector<std::basic_string<CHAR>>& v,
               int count) const {
    if (count == v.size()) return;
    if (base(node) < 0) {
      const CHAR* tail = tail_ - base(node) - 1;
      const CHAR* q = tail;
      CHAR nextChar;
      while ((nextChar = *pattern) && (*q != 0) &&
             (nextChar == getWildcard() || nextChar == *q)) {
        ++pattern;
        ++q;
      }
      if (nextChar == 0 && *q == 0) v.push_back(prefix + tail);
      return;
    }
    CHAR nextChar = *pattern;
    if (nextChar == 0) {
      if (check(base(node) + 1) == node) {
        v.push_back(prefix);
      }
    } else if (nextChar == getWildcard()) {
      for (uint32_t index = Alphabet::begin(); index != Alphabet::end();
           ++index) {
        int next = base(node) + index + 2;
        if (check(next) == node) {
          prefix.push_back(Alphabet::toChar(index));
          collect(next, prefix, pattern + 1, v, count);
          prefix.pop_back();
        }
      }
    } else {
      int32_t next = base(node) + getIndex(nextChar);
      if (check(next) == node) {
        prefix.push_back(nextChar);
        collect(next, prefix, pattern + 1, v, count);
        prefix.pop_back();
      }
    }
  }
  int32_t base(uint32_t index) const {
    if (index > size_) return 0;
    return base_[index - 1];
  }
  int32_t check(uint32_t index) const {
    if (index > size_) return 0;
    return check_[index - 1];
  }
  uint64_t extraSize_;
  const char* extraData_;
};

#endif
