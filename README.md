# Plastic

My implementation of common data structures for study.

## Linear Structure

| | **`vector`** | **`deque`** | **`forward_list`** | **`list`** |
| :--: | :--: | :--: | :--: | :--: |
| **`empty`** | `empty` | `empty` | `empty` | `empty` |
| **`size`** | `size` | `size` | `size` | `size` |
| **`clear`** | `clear` | `clear` | `clear` | `clear` |
| **`resize`** | `resize` | `resize` | `resize` | `resize` |
| **`capacity`** | `capacity` | `capacity` | | |
| **`reserve`** | `reserve` | `reserve` | | |
| **`begin`** | `begin` | `begin` | `begin` | `begin` |
| **`end`** | `end` | `end` | `end` | `end` |
| **`cbegin`** | `cbegin` | `cbegin` | `cbegin` | `cbegin` |
| **`cend`** | `cend` | `cend` | `cend` | `cend` |
| **`rbegin`** | `rbegin` | `rbegin` | | `rbegin` |
| **`rend`** | `rend` | `rend` | | `rend` |
| **`crbegin`** | `crbegin` | `crbegin` | | `crbegin` |
| **`crend`** | `crend` | `crend` | | `crend` |
| **`front`** | `front` | `front` | `front` | `front` |
| **`back`** | `back` | `back` | | `back` |
| **`data`** | `data` | | | |
| **`operator[]`** | `operator[]` | `operator[]` | | |
| **`push_front`** | | `push_front` | `push_front` | `push_front` |
| **`pop_front`** | | `pop_front` | `pop_front` | `pop_front` |
| **`push_back`** | `push_back` | `push_back` | | `push_back` |
| **`pop_back`** | `pop_back` | `pop_back` | | `pop_back` |
| **`insert`** | `insert` | `insert` | `insert_after` | `insert` |
| **`erase`** | `erase` | `erase` | `erase_after` | `erase` |
| **`operator==`** | `operator==` | `operator==` | `operator==` | `operator==` |
| **`operator<=>`** | `operator<=>` | `operator<=>` | `operator<=>` | `operator<=>` |

## Search Tree

| | **`binary_search_tree`** | **`red_black_tree`** | **`avl_tree`** | **`b_tree`** |
| :--: | :--: | :--: | :--: | :--: |
| **`empty`** | `empty` | | | |
| **`size`** | `size` | | | |
| **`clear`** | `clear` | | | |
| **`begin`** | `begin` | | | |
| **`end`** | `end` | | | |
| **`rbegin`** | `rbegin` | | | |
| **`rend`** | `rend` | | | |
| **`front`** | `front` | | | |
| **`back`** | `back` | | | |
| **`lower_bound`** | `lower_bound` | | | |
| **`upper_bound`** | `upper_bound` | | | |
| **`equal_range`** | `equal_range` | | | |
| **`find`** | `find` | | | |
| **`contains`** | `contains` | | | |
| **`count`** | `count` | | | |
| **`insert`** | `insert` | | | |
| **`erase`** | `erase` | | | |
| **`merge`** | `merge` | | | |
| **`operator==`** | `operator==` | | | |
| **`operator<=>`** | `operator<=>` | | | |

## Heap

| | **`binary_heap`** |
| :--: | :--: |
| **`empty`** | `empty` |
| **`size`** | `size` |
| **`clear`** | `clear` |
| **`top`** | `top` |
| **`push`** | `push` |
| **`pop`** | `pop` |
| **`merge`** | `merge` |
| **`assign`** | `assign` |
| **`erase`** | `erase` |