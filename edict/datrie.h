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
  int32_t size_;
  uint32_t tailsize_;
  int32_t pos_;
  int32_t* base_;
  int32_t* check_;
  CHAR* tail_;
  // return value must greater than 0
  int32_t getIndex(CHAR c) const {
    assert(c == 0 || Alphabet::toIndex(c) < (uint32_t)-2);
    assert(c == 0 || Alphabet::toIndex(c) < Alphabet::end());
    assert(c == 0 || Alphabet::toIndex(c) >= 0);
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
    DATrieBase<CHAR, Alphabet>::size_ = 1024;
    DATrieBase<CHAR, Alphabet>::tailsize_ = 1024;
    DATrieBase<CHAR, Alphabet>::base_ =
        new int[DATrieBase<CHAR, Alphabet>::size_];
    DATrieBase<CHAR, Alphabet>::check_ =
        new int[DATrieBase<CHAR, Alphabet>::size_];
    DATrieBase<CHAR, Alphabet>::base_[0] =
        -(DATrieBase<CHAR, Alphabet>::size_ - 1);
    DATrieBase<CHAR, Alphabet>::base_[1] = 1;
    DATrieBase<CHAR, Alphabet>::base_[2] = 0;
    for (int32_t i = 3; i < DATrieBase<CHAR, Alphabet>::size_; ++i) {
      DATrieBase<CHAR, Alphabet>::base_[i] = -(i - 1);
    }
    DATrieBase<CHAR, Alphabet>::check_[0] = -2;
    DATrieBase<CHAR, Alphabet>::check_[1] = 0;
    for (int32_t i = 2; i < DATrieBase<CHAR, Alphabet>::size_ - 1; ++i) {
      DATrieBase<CHAR, Alphabet>::check_[i] = -(i + 1);
    }
    DATrieBase<CHAR, Alphabet>::check_[DATrieBase<CHAR, Alphabet>::size_ - 1] =
        0;
    DATrieBase<CHAR, Alphabet>::tail_ =
        new CHAR[DATrieBase<CHAR, Alphabet>::tailsize_];
  }
  ~DATrieWriter() {
    if (DATrieBase<CHAR, Alphabet>::base_)
      delete DATrieBase<CHAR, Alphabet>::base_;
    if (DATrieBase<CHAR, Alphabet>::check_)
      delete DATrieBase<CHAR, Alphabet>::check_;
    if (DATrieBase<CHAR, Alphabet>::tail_)
      delete DATrieBase<CHAR, Alphabet>::tail_;
  }
  // word ends with a null terminator.
  void insert(const CHAR* word, uint32_t data = 0) {
    const CHAR* p = word - 1;
    int32_t s = 1;
    int32_t t;
    do {
      ++p;
      t = base(s) + DATrieBase<CHAR, Alphabet>::getIndex(*p);
      if (check(t) != s) {
        if (check(t) > 0) {
          // resolve collision
          std::vector<int32_t> list0;
          std::vector<int32_t> list1;
          get_list(s, list0);
          get_list(check(t), list1);
          if (list0.size() + 1 < list1.size()) {
            int32_t offset = DATrieBase<CHAR, Alphabet>::getIndex(*p);
            auto itr = std::lower_bound(list0.begin(), list0.end(), offset);
            list0.insert(itr, offset);
            s = relocate(s, s, list0, &offset);
          } else {
            s = relocate(s, check(t), list1, nullptr);
          }
        }
        t = base(s) + DATrieBase<CHAR, Alphabet>::getIndex(*p);
        if(check(t) > 0) throw __LINE__;
        alloc_node(t);
        set_check(t, s);
        set_base(t, -DATrieBase<CHAR, Alphabet>::pos_ - 1);
        write_tail((*p ? p + 1 : nullptr), data);
        return;
      }
      if (base(t) < 0) {
        if(check(t) != s) throw __LINE__;
        break;
      }
      s = t;
    } while (*p);

    int32_t old_base_value = base(t);
    CHAR* q = nullptr;
    if (*p) {
      //old_base_value = base(t);
      q = DATrieBase<CHAR, Alphabet>::tail_ - old_base_value - 1;
    }
    if (*p == 0 || strcmp(p + 1, q) == 0) return;
    if (base(t) != 0) {
      if(check(t) != s) throw __LINE__;
      // tail insert
      s = t;
      p = p + 1;
      std::vector<int32_t> list(1);
      while (*p == *q) {
        list[0] = (DATrieBase<CHAR, Alphabet>::getIndex(*p));
        set_base(s, x_check(list));
        t = base(s) + list[0];
        alloc_node(t);
        set_check(t, s);
        s = t;
        ++p;
        ++q;
      }
      list[0] = DATrieBase<CHAR, Alphabet>::getIndex(*p);
      list.push_back(DATrieBase<CHAR, Alphabet>::getIndex(*q));
      if (list[0] > list[1]) {
        std::swap(list[0], list[1]);
      }

      int32_t base_value = x_check(list);
      set_base(s, base_value);

      // move the remaining string in tail_.
      {
        int32_t t = base(s) + DATrieBase<CHAR, Alphabet>::getIndex(*q);
        alloc_node(t);
        set_check(t, s);
        set_base(t, old_base_value);
        CHAR* src = q + 1;
        CHAR* dst = DATrieBase<CHAR, Alphabet>::tail_ - old_base_value - 1;
        if (*q) {
          while (*src) *dst++ = *src++;
          *dst++ = *src++;
        }
        *reinterpret_cast<uint32_t*>(dst) = *reinterpret_cast<uint32_t*>(src);
      }

      // for the remaining part of the word.
      {
        int32_t t = base(s) + DATrieBase<CHAR, Alphabet>::getIndex(*p);
        alloc_node(t);
        set_check(t, s);
        set_base(t, -DATrieBase<CHAR, Alphabet>::pos_ - 1);
        write_tail((*p ? p + 1 : nullptr), data);
      }
    }
  }

  int dumpToFile(const char* filename, const char* extraData = nullptr,
                 uint64_t extraSize = 0) const {
    FILE* pFile = fopen(filename, "wb");
    if (!pFile) return -1;

    int32_t size = getCompressedSizeOfArray();
    const_cast<DATrieWriter*>(this)->compress_tail();

    fwrite(&size, sizeof(size), 1, pFile);
    fwrite(&(DATrieBase<CHAR, Alphabet>::pos_),
           sizeof(DATrieBase<CHAR, Alphabet>::tailsize_), 1, pFile);
    fwrite(&(DATrieBase<CHAR, Alphabet>::pos_),
           sizeof(DATrieBase<CHAR, Alphabet>::pos_), 1, pFile);
    fwrite(DATrieBase<CHAR, Alphabet>::base_,
           sizeof(*DATrieBase<CHAR, Alphabet>::base_), size, pFile);
    fwrite(DATrieBase<CHAR, Alphabet>::check_,
           sizeof(*DATrieBase<CHAR, Alphabet>::check_), size, pFile);
    fwrite(DATrieBase<CHAR, Alphabet>::tail_,
           sizeof(*DATrieBase<CHAR, Alphabet>::tail_),
           DATrieBase<CHAR, Alphabet>::pos_, pFile);
    if (extraData) {
      fwrite(&extraSize, sizeof(extraSize), 1, pFile);
      fwrite(extraData, sizeof(*extraData), extraSize, pFile);
    }
    fclose(pFile);
    return 0;
  }
  int32_t getCompressedSizeOfArray() const {
    int32_t index = -DATrieBase<CHAR, Alphabet>::base_[0];
    if(index == 0 || index != DATrieBase<CHAR, Alphabet>::size_ - 1) return DATrieBase<CHAR, Alphabet>::size_;
    for(;;) {
      int32_t prev = -DATrieBase<CHAR, Alphabet>::base_[index];
      if(prev + 1 == index)
        index = prev;
      else
        break;
    }
    return index + 1;
  }
  void compress_tail() {
    struct Info {
      int32_t index;
      bool hasString;
    };
    std::map<int32_t, Info> tails;
    for(int32_t i = 0; i < DATrieBase<CHAR, Alphabet>::size_; ++i) {
      if(DATrieBase<CHAR, Alphabet>::base_[i] < 0 && DATrieBase<CHAR, Alphabet>::check_[i] > 0) {
        if(DATrieBase<CHAR, Alphabet>::base_[DATrieBase<CHAR, Alphabet>::check_[i]] + 1 == i) {
          tails[-DATrieBase<CHAR, Alphabet>::base_[i] - 1] = {i, false};
        } else {
          tails[-DATrieBase<CHAR, Alphabet>::base_[i] - 1] = {i, true};
        }
      }
    }
    int32_t new_pos = 0;
    for(const auto& kv : tails) {
      int32_t index = kv.second.index;
      uint32_t old_pos = - DATrieBase<CHAR, Alphabet>::base_[index] - 1;
      DATrieBase<CHAR, Alphabet>::base_[index] = - new_pos - 1;
      uint32_t old_end_pos = old_pos;
      if(kv.second.hasString) {
        while(DATrieBase<CHAR, Alphabet>::tail_[old_end_pos++] != 0);
      }
      old_end_pos += sizeof(uint32_t);
      if(old_pos != new_pos)
        memcpy(DATrieBase<CHAR, Alphabet>::tail_ + new_pos, DATrieBase<CHAR, Alphabet>::tail_ + old_pos, old_end_pos - old_pos);
      new_pos += old_end_pos - old_pos;
    }
    DATrieBase<CHAR, Alphabet>::pos_ = new_pos;
  }
  void resize(int32_t size) {
    assert(size > 0);

    int32_t new_size = DATrieBase<CHAR, Alphabet>::size_ * 2;
    while (size > new_size) new_size *= 2;

    int32_t* temp_base = new int32_t[new_size];
    memcpy(temp_base, DATrieBase<CHAR, Alphabet>::base_,
           DATrieBase<CHAR, Alphabet>::size_ *
               sizeof(*DATrieBase<CHAR, Alphabet>::base_));
    temp_base[0] = -(new_size - 1);
    for (int32_t i = DATrieBase<CHAR, Alphabet>::size_; i < new_size; ++i) {
      temp_base[i] = -(i - 1);
    }
    delete DATrieBase<CHAR, Alphabet>::base_;
    DATrieBase<CHAR, Alphabet>::base_ = temp_base;

    int32_t* temp_check = new int32_t[new_size];
    memcpy(temp_check, DATrieBase<CHAR, Alphabet>::check_,
           DATrieBase<CHAR, Alphabet>::size_ *
               sizeof(*DATrieBase<CHAR, Alphabet>::check_));
    for (int32_t i = DATrieBase<CHAR, Alphabet>::size_ - 1; i < new_size - 1; ++i) {
      temp_check[i] = -(i + 1);
    }
    temp_check[new_size - 1] = 0;

    delete DATrieBase<CHAR, Alphabet>::check_;
    DATrieBase<CHAR, Alphabet>::check_ = temp_check;
    DATrieBase<CHAR, Alphabet>::size_ = new_size;
  }

 protected:
  friend class DATrieReader<CHAR, Alphabet>;
  void get_list(int32_t s, std::vector<int32_t>& list) {
    int32_t base_value = base(s);
    if (check(base_value + 1) == s) list.push_back(1);  // add '\0' to the list
    for (int32_t index = Alphabet::begin(); index != Alphabet::end(); ++index) {
      if (check(base_value + index + 2) == s) list.push_back(index + 2);
    }
  }
  void alloc_node(int32_t s) {
    assert(s > 0);

    int32_t prev = base(s);
    int32_t next = check(s);
    DATrieBase<CHAR, Alphabet>::base_[-next] = prev;
    DATrieBase<CHAR, Alphabet>::check_[-prev] = next;
  }
  void free_node(int32_t s) {
    assert(s > 0);

    int32_t next = s + 1;
    while (check(next) >= 0) ++next;
    int32_t prev = DATrieBase<CHAR, Alphabet>::base_[next];
    DATrieBase<CHAR, Alphabet>::base_[s] = prev;
    DATrieBase<CHAR, Alphabet>::base_[next] = -s;
    DATrieBase<CHAR, Alphabet>::check_[-prev] = -s;
    DATrieBase<CHAR, Alphabet>::check_[s] = -next;
  }
  int relocate(int32_t current, int32_t s, std::vector<int32_t>& list,
               int32_t *t) {
    int32_t old_base = base(s);
    set_base(s, x_check(list));
    if (t) {
      list.erase(std::remove(list.begin(), list.end(), *t), list.end());
    };
    for (const auto offset : list) {
      int32_t old_node = old_base + offset;
      int32_t new_node = base(s) + offset;
      alloc_node(new_node);
      set_base(new_node, base(old_node));
      set_check(new_node, s);
      if (base(old_node) > 0) {
        int32_t base_value = base(old_node);
        if (check(base_value + 1) == old_node) 
          set_check(base_value + 1, new_node);
        for (int32_t index = Alphabet::begin(); index != Alphabet::end();
             ++index) {
          if(check(base_value + index + 2) == old_node) {
            set_check(base_value + index + 2, new_node);
          }
        }
      }
      if (current != s && old_node == current) current = new_node;
      free_node(old_node);
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
    uint32_t needed_size =
        DATrieBase<CHAR, Alphabet>::pos_ + word_size + sizeof(data);
    if (needed_size > DATrieBase<CHAR, Alphabet>::tailsize_) {
      uint32_t new_size = DATrieBase<CHAR, Alphabet>::tailsize_ * 2;
      while (needed_size > new_size) new_size *= 2;
      CHAR* temp = new CHAR[new_size];
      memcpy(temp, DATrieBase<CHAR, Alphabet>::tail_,
             DATrieBase<CHAR, Alphabet>::tailsize_);
      delete DATrieBase<CHAR, Alphabet>::tail_;
      DATrieBase<CHAR, Alphabet>::tail_ = temp;
      DATrieBase<CHAR, Alphabet>::tailsize_ = new_size;
    }
    memcpy(DATrieBase<CHAR, Alphabet>::tail_ + DATrieBase<CHAR, Alphabet>::pos_,
           word, (word_size) * sizeof(CHAR));
    DATrieBase<CHAR, Alphabet>::pos_ += word_size;
    *reinterpret_cast<uint32_t*>(DATrieBase<CHAR, Alphabet>::tail_ +
                                 DATrieBase<CHAR, Alphabet>::pos_) = data;
    DATrieBase<CHAR, Alphabet>::pos_ += sizeof(data);
  }
  // index = 1...size_-1
  int32_t base(int32_t index) {
    assert(index >= 0);
    if (index >= DATrieBase<CHAR, Alphabet>::size_) resize(index + 1);
    return DATrieBase<CHAR, Alphabet>::base_[index];
  }
  // index = 1...size_-1
  int32_t check(int32_t index) {
    assert(index >= 0);
    if (index >= DATrieBase<CHAR, Alphabet>::size_) resize(index + 1);
    return DATrieBase<CHAR, Alphabet>::check_[index];
  }

  void set_base(int32_t index, int32_t value) {
    assert(index >= 0);
    if (index >= DATrieBase<CHAR, Alphabet>::size_) resize(index + 1);
    DATrieBase<CHAR, Alphabet>::base_[index] = value;
  }

  void set_check(int32_t index, int32_t value) {
    assert(index >= 0);
    if (index >= DATrieBase<CHAR, Alphabet>::size_) resize(index + 1);
    DATrieBase<CHAR, Alphabet>::check_[index] = value;
  }
  int32_t x_check(const std::vector<int32_t>& list) {
    assert(!list.empty());
    assert(std::is_sorted(list.begin(), list.end()));
    assert(std::all_of(list.begin(), list.end(), [](int32_t x){
      return x > 0;
    }));

    int32_t offset0 = list[0];
    int32_t s = -DATrieBase<CHAR, Alphabet>::check_[0];
    // find least free cell s such that s > c
    for (;;) {
      while (s != 0 && s <= offset0) {
        assert(s < (DATrieBase<CHAR, Alphabet>::size_));
        s = -DATrieBase<CHAR, Alphabet>::check_[s];
      }
      if (s == 0)
        resize(DATrieBase<CHAR, Alphabet>::size_ * 2);
      else
        break;
    }

    for (;;) {
      while (s != 0) {
        int32_t i = 1;
        while (i != list.size() && check(s + list[i] - offset0) < 0) ++i;
        if (i == list.size()) {
          return s - offset0;
        }
        assert(s < (DATrieBase<CHAR, Alphabet>::size_));
        s = -DATrieBase<CHAR, Alphabet>::check_[s];
      }
      resize(DATrieBase<CHAR, Alphabet>::size_ * 2);
    }
  }
};

template <typename CHAR, class Alphabet>
class DATrieReader : public DATrieBase<CHAR, Alphabet> {
 public:
  DATrieReader() {}
  int loadFromMemory(CHAR* data) {
    CHAR* pos = data;
    DATrieBase<CHAR, Alphabet>::size_ = *reinterpret_cast<uint32_t*>(pos);
    pos += sizeof(uint32_t);
    DATrieBase<CHAR, Alphabet>::tailsize_ = *reinterpret_cast<uint32_t*>(pos);
    pos += sizeof(uint32_t);
    DATrieBase<CHAR, Alphabet>::pos_ = *reinterpret_cast<uint32_t*>(pos);
    pos += sizeof(uint32_t);

    DATrieBase<CHAR, Alphabet>::base_ = reinterpret_cast<int32_t*>(pos);
    pos += DATrieBase<CHAR, Alphabet>::size_ * sizeof(int32_t);
    DATrieBase<CHAR, Alphabet>::check_ = reinterpret_cast<int32_t*>(pos);
    pos += DATrieBase<CHAR, Alphabet>::size_ * sizeof(int32_t);
    DATrieBase<CHAR, Alphabet>::tail_ = pos;
    pos += DATrieBase<CHAR, Alphabet>::tailsize_ * sizeof(CHAR);

    extraSize_ = *reinterpret_cast<uint64_t*>(pos);
    pos += sizeof(extraSize_);
    extraData_ = pos;
    pos += extraSize_;
    return 0;
  }
  int loadFromWriter(const DATrieWriter<CHAR, Alphabet>& writer) {
    DATrieBase<CHAR, Alphabet>::size_ = writer.size_;
    DATrieBase<CHAR, Alphabet>::tailsize_ = writer.tailsize_;
    DATrieBase<CHAR, Alphabet>::pos_ = writer.pos_;
    DATrieBase<CHAR, Alphabet>::base_ = writer.base_;
    DATrieBase<CHAR, Alphabet>::check_ = writer.check_;
    DATrieBase<CHAR, Alphabet>::tail_ = writer.tail_;

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
      t = base(s) + DATrieBase<CHAR, Alphabet>::getIndex(*p);
      if (check(t) != s) return -1;
      if (base(t) < 0) break;
      s = t;
    } while (*p);

    CHAR* q = nullptr;
    int pos = -base(t) - 1;
    if (*p) {
      q = DATrieBase<CHAR, Alphabet>::tail_ + pos;
      if (strcmp(p + 1, q) == 0) {
        int32_t len = static_cast<int32_t>(strlen(q));
        if (len < 0) throw;
        int32_t offset = pos + len + 1;
        data = *reinterpret_cast<uint32_t*>(DATrieBase<CHAR, Alphabet>::tail_ +
                                            offset);
        return offset;
      }
    } else {
      data =
          *reinterpret_cast<uint32_t*>(DATrieBase<CHAR, Alphabet>::tail_ + pos);
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
      t = base(s) + DATrieBase<CHAR, Alphabet>::getIndex(*p);
      if (check(t) != s) return v;
      s = t;
      ++p;
      if (base(s) < 0) {
        const CHAR* tail = DATrieBase<CHAR, Alphabet>::tail_ - base(s) - 1;
        while (*p != NULL && *p == *tail) {
          ++p;
          ++tail;
        };
        if (*p == NULL) v.push_back(std::basic_string<CHAR>{prefix} + tail);
        return v;
        break;
      }
    }
    std::basic_string<CHAR> temp(prefix);
    collect(s, temp, v, max);
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
      const CHAR* q = DATrieBase<CHAR, Alphabet>::tail_ - base(node) - 1;
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
    if (std::is_same<CHAR, char>::value)
      return '?';
    else if (std::is_same<CHAR, wchar_t>::value)
      return L'?';
    else if (std::is_same<CHAR, char16_t>::value)
      return u'?';
    else if (std::is_same<CHAR, char32_t>::value)
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
      const CHAR* tail = DATrieBase<CHAR, Alphabet>::tail_ - base(node) - 1;
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
      int32_t next =
          base(node) + DATrieBase<CHAR, Alphabet>::getIndex(nextChar);
      if (check(next) == node) {
        prefix.push_back(nextChar);
        collect(next, prefix, pattern + 1, v, count);
        prefix.pop_back();
      }
    }
  }
  int32_t base(int32_t index) const {
    assert(index > 0);
    if (index >= DATrieBase<CHAR, Alphabet>::size_) return 0;
    return DATrieBase<CHAR, Alphabet>::base_[index];
  }
  int32_t check(int32_t index) const {
    assert(index > 0);
    if (index >= DATrieBase<CHAR, Alphabet>::size_) return 0;
    return DATrieBase<CHAR, Alphabet>::check_[index];
  }
  uint64_t extraSize_;
  const char* extraData_;
};

#endif
