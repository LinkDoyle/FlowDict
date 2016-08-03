#ifndef DARTRS_H
#define DARTRS_H

#include <vector>
#include <cassert>
#include <cstring>

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
class DATrie {
 public:
  DATrie() : size_(16), tailsize_(16), pos_(0) {
    base_ = new int[size_];
    memset(base_, 0, sizeof(*base_) * size_);
    check_ = new int[size_];
    memset(check_, 0, sizeof(*check_) * size_);
    tail_ = new CHAR[tailsize_];

    base_[0] = 1;
  }
  ~DATrie() {
    if (base_) delete base_;
    if (check_) delete check_;
    if (tail_) delete tail_;
  }
  int retrieve(const CHAR *word) {
    uint32_t unused;
    return retrieve(word, unused);
  }
  // return the position to data,
  // word ends with a null terminator,
  // return the data using the parameter data.
  int retrieve(const CHAR *word, uint32_t &data) {
    const CHAR *p = word;
    int state = 1;
    while (base(state) > 0) {
      int next = base(state) + getIndex(*p++);
      if (check(next) != state) return -1;
      state = next;
    }
    int pos = -base(state) - 1;

    // there is no remaining string in the tail_.
    if (*(p - 1) == 0) return pos;

    // check the remaining string.
    for (;;) {
      if (tail_[pos] != *p) return -1;
      if (tail_[pos] == 0 && *p == 0) {
        data = *reinterpret_cast<uint32_t *>(tail_ + pos + 1);
        return pos + 1;
      }
      ++pos;
      ++p;
    }
  }

  // word ends with a null terminator.
  void insert(const CHAR *word, uint32_t data = 0) {
    const CHAR *p = word;
    int state = 1;
    for (;;) {
      int base_value = base(state);

      if (base_value < 0) {
        // Case 3: insertion, when a collision occurs
        int temp = -base_value;
        CHAR *q = tail_ + temp - 1;

        // collect the common prefix
        while (*p == *q) {
          list_.clear();
          list_.push_back(*p);
          base(state) = x_check(list_);
          int next_state = base(state) + getIndex(*p);
          check(next_state) = state;
          state = next_state;
          ++p, ++q;
        }
        list_.clear();
        list_.push_back(*p);
        list_.push_back(*q);
        int candidate = x_check(list_);
        base(state) = candidate;

        // move the remaining string in tail_.
        {
          int next_state = candidate + getIndex(*q++);
          base(next_state) = -temp;
          check(next_state) = state;
          CHAR *src = q;
          CHAR *dst = tail_ + temp - 1;
          if (*(q - 1)) {
            while (*src) *dst++ = *src++;
            *dst++ = *src++;
          }
          *reinterpret_cast<uint32_t *>(dst++) =
              *reinterpret_cast<uint32_t *>(src++);
        }

        // for the remaining string of the word
        {
          int next_state = candidate + getIndex(*p++);
          base(next_state) = -pos_ - 1;
          check(next_state) = state;
          write_tail(p, data);
        }
        return;
      } else {
        int next = base_value + getIndex(*p);
        int check_value = check(next);

        if (check_value == 0) {
          // Case 1 and 2: insert without any collisions.
          base_[next - 1] = -(pos_ + 1);
          check_[next - 1] = state;
          write_tail((*p ? p + 1 : nullptr), data);
          return;
        } else if (check_value != state) {
          // Case 4: insertion, when a new word is inserted with a collision

          list_.clear();
          if (check(base_value + 1) == state)
            list_.push_back(0);  // add '\0' to the list
          for (uint32_t index = Alphabet::begin(); index != Alphabet::end();
               ++index) {
            if (check(base_value + index + 2) == state)
              list_.push_back(Alphabet::toChar(index));
          }

          list1_.clear();
          if (check(base(check_value) + 1) == check_value)
            list1_.push_back(0);  // add '\0' to the list
          for (uint32_t index = Alphabet::begin(); index != Alphabet::end();
               ++index) {
            if (check(base(check_value) + index + 2) == check_value)
              list1_.push_back(Alphabet::toChar(index));
          }

          int temp_base, candidate;
          if (list_.size() + 1 < list1_.size()) {
            temp_base = base_value;
            candidate = x_check(list_);
          } else {
            temp_base = base(check_value);
            candidate = x_check(list1_);
          }
          const auto &list =
              (list_.size() + 1 < list1_.size()) ? list_ : list1_;
          base(check_value) = candidate;
          for (const auto c : list) {
            int temp_node1 = temp_base + getIndex(c);
            int temp_node2 = base(check_value) + getIndex(c);
            base(temp_node2) = base(temp_node1);
            check(temp_node2) = check(temp_node1);
            if (base(temp_node1) > 0) {
              for (uint32_t index = Alphabet::begin(); index != Alphabet::end();
                   ++index) {
                int &check_value = check(base(temp_node1) + index + 2);
                if (check_value == temp_node1) {
                  check_value = temp_node2;
                }
              }
            }
            base(temp_node1) = 0;
            check(temp_node1) = 0;
          }

          // insert the remaining part of new string into tail_.
          int temp_node = base(state) + getIndex(*p);
          base(temp_node) = -pos_ - 1;
          check(temp_node) = state;
          write_tail((*p ? p + 1 : nullptr), data);
          return;
        }

        ++p;
        state = next;
      }
    }
  }
  void print_memory() {
    printf("index\tbase\tcheck\n");
    for (size_t i = 1; i <= size_; ++i) {
      printf("%d\t%d\t%d\n", i, base_[i - 1], check_[i - 1]);
    }
    printf("index\ttail\n");
    for (int i = 0; i < pos_; ++i) {
      printf("%d\t%c\n", i + 1, tail_[i]);
    }
  }

 private:
  size_t size_;
  size_t tailsize_;
  int *base_;
  int *check_;
  int pos_;
  CHAR *tail_;
  std::vector<CHAR> list_;
  std::vector<CHAR> list1_;
  // return value must greater than 0
  uint32_t getIndex(CHAR c) {
    assert(Alphabet::toIndex(c) < (uint32_t)-2);
    return c == 0 ? 1 : Alphabet::toIndex(c) + 2;
  }
  void write_tail(const CHAR *word, uint32_t data) {
    int word_size = 0;
    if (word) {
      const CHAR *p = word;
      while (*p++) ++word_size;
      word_size += 1;  // includes the null terminator
    }
    size_t needed_size = pos_ + word_size + sizeof(data);
    if (needed_size > tailsize_) {
      size_t new_size = tailsize_ * 2;
      while (needed_size > new_size) new_size *= 2;
      CHAR *temp = new CHAR[new_size];
      memcpy(temp, tail_, tailsize_);
      delete tail_;
      tail_ = temp;
      tailsize_ = new_size;
    }
    memcpy(tail_ + pos_, word, (word_size) * sizeof(CHAR));
    pos_ += word_size;
    *reinterpret_cast<uint32_t *>(tail_ + pos_) = data;
    pos_ += sizeof(data);
  }
  void resize(size_t size) {
    size_t new_size = size_ * 2;
    while (size > new_size) new_size *= 2;

    int *temp_base = new int[new_size];
    memcpy(temp_base, base_, size_ * sizeof(*base_));
    memset(temp_base + size_, 0, (new_size - size_) * sizeof(*base_));
    delete base_;
    base_ = temp_base;

    int *temp_check = new int[new_size];
    memcpy(temp_check, check_, size_ * sizeof(*check_));
    memset(temp_check + size_, 0, (new_size - size_) * sizeof(*check_));
    delete check_;
    check_ = temp_check;

    size_ = new_size;
  }
  // index = 1...size_
  int &base(size_t index) {
    if (index > size_) resize(index);
    return base_[index - 1];
  }
  // index = 1...size_
  int &check(size_t index) {
    if (index > size_) resize(index);
    return check_[index - 1];
  }
  size_t x_check(const std::vector<CHAR> &list) {
    int q = 1;
    for (const auto a : list) {
      while (check(q + getIndex(a)) != 0) ++q;
    }
    return q;
  }
};
#endif
