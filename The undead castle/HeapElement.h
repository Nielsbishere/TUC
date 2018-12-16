#pragma once
namespace osomi {
	template<typename T, float (T::*val)(), float (T::*tiebreaker)()>
	class HeapElement{
	private:
		unsigned int index;
		T *c;
	public:
		HeapElement(unsigned int _index, T *_c) : index(_index), c(_c) {}
		HeapElement() : HeapElement(0, nullptr) {}
		bool operator<(HeapElement<T, val, tiebreaker> &c) {
			return (c.c->*val)() < (this->c->*val)() || (*this == c && (c.c->*tiebreaker)() < (this->c->*tiebreaker)());
		}
		bool operator>(HeapElement<T, val, tiebreaker> &c) {
			return (c.c->*val)() > (this->c->*val)() || (*this == c && (c.c->*tiebreaker)() > (this->c->*tiebreaker)());
		}
		bool operator<=(HeapElement<T, val, tiebreaker> &c) {
			return (c.c->*val)() <= (this->c->*val)() || (*this == c && (c.c->*tiebreaker)() <= (this->c->*tiebreaker)());
		}
		bool operator>=(HeapElement<T, val, tiebreaker> &c) {
			return (c.c->*val)() <= (this->c->*val)() || (*this == c && (c.c->*tiebreaker)() >= (this->c->*tiebreaker)());
		}
		bool operator==(HeapElement<T, val, tiebreaker> &c) {
			return (this->c->*val)() == (c.c->*val)();
		}
		bool operator!=(HeapElement<T, val, tiebreaker> &c) {
			return (this->c->*val)() != (c.c->*val)();
		}
		void set(T *_c) {
			c = _c;
		}
		void setIndex(unsigned int i) {
			index = i;
		}
		void swap(HeapElement<T, val, tiebreaker> &he) {
			unsigned int hind = he.index;
			he.index = index;
			index = hind;
			T *hc = he.c;
			he.c = c;
			c = hc;
		}
		T *getValue() { return c; }
		unsigned int getIndex() { return index; }
	};
}