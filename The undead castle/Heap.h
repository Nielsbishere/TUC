#pragma once
#include "HeapElement.h"
using namespace std;
namespace osomi {
	template<typename T, float (T::*val)(), float (T::*tiebreaker)()>
	class Heap {
	private:
		HeapElement<T, val, tiebreaker> *elements;
		unsigned int curr, max;

		void sortUp(unsigned int id) {
			HeapElement<T, val, tiebreaker> &h = elements[id];
			while (true) {
				if (h.getIndex() == 0)return;
				HeapElement<T, val, tiebreaker> &p = elements[(h.getIndex() - 1) / 2];
				if (h < p) p.swap(h);
				else return;
			}
		}
		void sortDown(unsigned int id) {
			HeapElement<T, val, tiebreaker> &h = elements[id];
			while (true) {
				unsigned int lchild = 2 * h.getIndex() + 1;
				unsigned int rchild = 2 * h.getIndex() + 2;
				if (lchild >= curr-1)return;
				HeapElement<T, val, tiebreaker> &lc = elements[lchild];
				if (rchild < curr-1) {
					HeapElement<T, val, tiebreaker> &rc = elements[rchild];
					HeapElement<T, val, tiebreaker> &swap = lc > rc ? lc : rc;
					if (h > swap) swap.swap(h);
					else return;
				}
				else {
					if (h > lc)lc.swap(h);
					else return;
				}
			}
		}
	public:
		Heap(unsigned int i) {
			max = i;
			elements = new HeapElement<T, val, tiebreaker>[max];
			curr = 0;
		}
		//The heap.first() function.
		T *operator-() {
			if (curr == 0)return nullptr;
			T *res = elements[0].getValue();
			elements[0].set(nullptr);
			curr--;
			if (curr > 0) {
				elements[0].set(elements[curr].getValue());
				sortDown(0);
			}
			return res;
		}
		T *first() {
			return -*this;
		}
		//The heap.index() function
		unsigned int index(HeapElement<T, val, tiebreaker> &h) {
			if (*this == h)return h.getIndex();
			return curr;
		}
		//The heap.contains() function
		bool operator==(HeapElement<T, val, tiebreaker> &h) {
			if (h.getIndex() >= curr)return false;
			return elements[curr].getValue() == h.c;
		}
		bool contains(HeapElement<T, val, tiebreaker> &h) {
			return *this == h;
		}
		//The less efficient heap.index() function
		unsigned int index(T *c) {
			for (unsigned int i = 0; i < curr; i++) 
				if (elements[i].getValue() == c)
					return i;
			return curr;
		}
		//The less efficient heap.contains() function
		bool operator==(T *c) {
			return index(c) != curr;
		}
		bool contains(T *c) {
			return *this == c;
		}
		//The heap.add() function
		Heap &operator+=(T *c) {
			if (curr >= max || c == nullptr) return *this;
			elements[curr].setIndex(curr);
			elements[curr].set(c);
			sortUp(curr);
			curr++;
			return *this;
		}
		Heap &add(T *c) {
			return *this += c;
		}
		//The heap.size() function
		unsigned int operator!() {return curr; }
		unsigned int size() { return !*this; }
		//The heap.update() function
		void operator[](unsigned int id) {
			if (id >= curr)return;
			sortUp(id);
			sortDown(id);
		}
		void update(unsigned int id) {
			(*this)[id];
		}
		//The less efficient heap.update() function
		void operator[](T *c) {
			unsigned int id = index(c);
			if (id >= curr)return;
			sortUp(id);
			sortDown(id);
		}
		void update(T *c) {
			(*this)[c];
		}
	};
}