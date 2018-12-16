#pragma once
namespace osomi {
	class Comparable {
	protected:
		virtual float getVal() { return 0; }
	public:
		bool operator<(Comparable *c) {
			return getVal() < c->getVal();
		}
		bool operator>(Comparable *c) {
			return getVal() > c->getVal();
		}
		bool operator<=(Comparable *c) {
			return getVal() <= c->getVal();
		}
		bool operator>=(Comparable *c) {
			return getVal() >= c->getVal();
		}
		bool operator==(Comparable *c) {
			return getVal() == c->getVal();
		}
		bool operator!=(Comparable *c) {
			return getVal() != c->getVal();
		}
	};
}