#pragma once

#include <unordered_map>
#include <list>

#include <formats/common/marco.hpp>
#include <formats/jsoncpp/fwd.hpp>

FORMATS_NAMESPACE_BEGIN

template <typename Key,
          typename T,
          typename Hash     = std::hash<Key>,
          typename KeyEqual = std::equal_to<Key>>
class ordered_map
{
public:
  using value_type = std::pair<const Key, T>;
  using link_type  = std::list<value_type>;
  using map_type   = std::unordered_map<Key, typename link_type::iterator, Hash, KeyEqual>;

  using refex_wrapper   = value_type&;
  using const_reference = const value_type&;
  using pointer         = value_type*;
  using const_pointer   = const value_type*;

  using hasher      = typename map_type::hasher;
  using key_type    = typename map_type::key_type;
  using mapped_type = typename map_type::mapped_type;
  using key_equal   = typename map_type::key_equal;

  using size_type       = typename link_type::size_type;
  using difference_type = typename link_type::difference_type;

  using iterator               = typename link_type::iterator;
  using const_iterator         = typename link_type::const_iterator;
  using reverse_iterator       = typename link_type::reverse_iterator;
  using const_reverse_iterator = typename link_type::const_reverse_iterator;

  /*using local_iterator       = typename map_type::iterator;
  using const_local_iterator = typename map_type::const_iterator;*/

  using insert_return_type = std::pair<iterator, bool>;

public:
  ordered_map() {}

  ordered_map(size_type bucket_count)
      : mapped_container_(bucket_count)
  {}

  ordered_map(size_type bucket_count, const hasher& hash)
      : mapped_container_(bucket_count, hash)
  {}

  template <typename Iter>
  ordered_map(Iter first, Iter last)
      : mapped_container_(first, last)
      , linked_container_(first, last)
  {}

  ordered_map(std::initializer_list<value_type> ilist)
      : mapped_container_(ilist)
      , linked_container_(ilist)
  {}

  ordered_map(const ordered_map& other)
  {
    mapped_container_.reserve(other.linked_container_.size());
    insert(other.begin(), other.end());
  }

  ordered_map(ordered_map&& other)
      : mapped_container_(std::move(other.mapped_container_))
      , linked_container_(std::move(other.linked_container_))
  {}

  inline ordered_map& operator=(const ordered_map& other)
  {
    mapped_container_ = other.mapped_container_;
    linked_container_ = other.linked_container_;

    return *this;
  }

  inline ordered_map& operator=(ordered_map&& other)
  {
    mapped_container_ = other.mapped_container_;
    linked_container_ = other.linked_container_;

    other.clear();

    return *this;
  }

  ~ordered_map() { clear(); }

public:
  inline T& operator[](const key_type& key) noexcept
  {
    auto iter = find(key);

    if (iter != end()) { return iter->second; }
    else
    {
      auto ret = emplace_hint(linked_container_.end(), key, T());
      return ret.first->second;
    }
  }

  inline T& operator[](key_type&& key) noexcept
  {
    auto iter = find(key);

    if (iter != end()) { return iter->second; }
    else
    {
      auto ret = emplace_hint(linked_container_.end(), std::move(key), T());
      return ret.first->second;
    }
  }

public:  // Capacity
  inline bool empty() const noexcept { return linked_container_.empty(); }

  inline size_type size() const noexcept { return linked_container_.size(); }

  inline iterator find(const key_type& key) noexcept
  {
    auto iter = mapped_container_.find(key);
    return iter != mapped_container_.end() ? iterator(iter->second) : linked_container_.end();
  }

  inline const_iterator find(const key_type& key) const noexcept
  {
    auto iter = mapped_container_.find(key);
    return iter != mapped_container_.end() ? const_iterator(iter->second) : linked_container_.end();
  }

  inline bool contains(const key_type& key) const noexcept
  {
    return mapped_container_.find(key) != mapped_container_.end();
  }

public:  // Modifiers
  inline void clear() noexcept
  {
    mapped_container_.clear();
    linked_container_.clear();
  }

  inline void swap(ordered_map& other) noexcept
  {
    mapped_container_.swap(other.mapped_container_);
    linked_container_.swap(other.linked_container_);
  }

  inline insert_return_type insert(const value_type& value)
  {
    if (mapped_container_.find(value->first) == mapped_container_.end())
    {
      auto iter = linked_container_.insert(end(), value);
      mapped_container_.emplace(iter->first, iter);

      return insert_return_type{iter, true};
    }

    return insert_return_type{find(value->first), false};
  }

  inline insert_return_type insert(value_type&& value)
  {
    if (mapped_container_.find(value.first) == mapped_container_.end())
    {
      auto iter = linked_container_.insert(linked_container_.end(), std::move(value));
      mapped_container_.emplace(iter->first, iter);

      return insert_return_type{iter, true};
    }

    return insert_return_type{find(value.first), false};
  }

  inline iterator insert(const_iterator pos, const value_type& value)
  {
    iterator iter = find(value.first);

    if (mapped_container_.find(value.first) == mapped_container_.end())
    {
      iter = linked_container_.insert(pos, value);
      mapped_container_.emplace(iter->first, iter);
    }

    return iter;
  }

  inline iterator insert(const_iterator pos, value_type&& value)
  {
    iterator iter = find(value.first);

    if (mapped_container_.find(value.first) == mapped_container_.end())
    {
      iter = linked_container_.insert(pos, std::move(value));
      mapped_container_.emplace(iter->first, iter);
    }

    return iter;
  }

  template <class InputIt>
  inline void insert(InputIt first, InputIt last)
  {
    while (first != last)
    {
      insert(end(), *first);
      ++first;
    }
  }

  inline void insert(std::initializer_list<value_type> ilist)
  {
    insert(ilist.begin(), ilist.end());
  }

  template <class... Args>
  inline insert_return_type emplace(Args&&... args)
  {
    return emplace_hint(linked_container_.end(), std::forward<Args&&>(args)...);
  }

  template <class... Args>
  inline insert_return_type emplace_hint(const_iterator pos, Args&&... args)
  {
    auto iter       = linked_container_.emplace(pos, std::forward<Args&&>(args)...);
    auto insert_ret = mapped_container_.emplace(iter->first, iter);

    if (!insert_ret.second) { linked_container_.erase(iter); }

    return {insert_ret.first->second, insert_ret.second};
  }

  inline size_type erase(const key_type& key)
  {
    auto iter = mapped_container_.find(key);
    if (iter != mapped_container_.end())
    {
      linked_container_.erase(iter->second);
      mapped_container_.erase(iter);

      return 1;
    }

    return 0;
  }

  inline iterator erase(iterator pos)
  {
    mapped_container_.erase(pos->first);
    return linked_container_.erase(pos);
  }

  inline iterator erase(const_iterator pos)
  {
    mapped_container_.erase(pos->first);
    return linked_container_.erase(pos);
  }

  void merge(ordered_map& source) noexcept
  {
    auto iter = source.begin();
    while (iter != source.end())
    {
      if (!contains(iter->first))
      {
        insert(end(), *iter);
        iter = source.erase(iter);
        continue;
      }

      ++iter;
    }
  }

  void merge(ordered_map&& source) noexcept
  {
    auto iter = source.begin();
    while (iter != source.end())
    {
      if (!contains(iter->first))
      {
        insert(end(), *iter);
        // iter = source.erase(iter);
        // continue;
      }

      ++iter;
    }

    source.clear();
  }

public:  // Iterators
  inline iterator       begin() noexcept { return linked_container_.begin(); }
  inline const_iterator begin() const noexcept { return linked_container_.cbegin(); }
  inline const_iterator cbegin() const noexcept { return linked_container_.cbegin(); }

  inline iterator       end() noexcept { return linked_container_.end(); }
  inline const_iterator end() const noexcept { return linked_container_.end(); }
  inline const_iterator cend() const noexcept { return linked_container_.end(); }

  inline reverse_iterator       rbegin() noexcept { return linked_container_.rbegin(); }
  inline const_reverse_iterator rbegin() const noexcept { return linked_container_.rbegin(); }
  inline const_reverse_iterator crbegin() const noexcept { return linked_container_.crbegin(); }

  inline iterator       rend() noexcept { return linked_container_.rend(); }
  inline const_iterator rend() const noexcept { return linked_container_.rend(); }
  inline const_iterator crend() const noexcept { return linked_container_.crend(); }

public:
  friend bool operator==(const ordered_map& left, const ordered_map& right) noexcept
  {
    if (left.size() != right.size()) return false;

    auto iter1 = left.begin();
    auto iter2 = right.begin();

    while (iter1 != left.end())
    {
      if (iter1->first != iter2->first || iter1->second != iter2->second) return false;

      ++iter1;
      ++iter2;
    }

    return true;
  }

  friend bool operator!=(const ordered_map& left, const ordered_map& right) noexcept
  {
    return !(left == right);
  }

private:
  map_type  mapped_container_;
  link_type linked_container_;
};

FORMATS_NAMESPACE_END