#pragma once
namespace osomi {
	class Comparable {
	public:
		virtual ~Comparable() {}
		bool operator<(Comparable &c) {
			return v() < c.v();
		}
		bool operator>(Comparable &c) {
			return v() > c.v();
		}
		bool operator<=(Comparable &c) {
			return v() <= c.v();
		}
		bool operator>=(Comparable &c) {
			return v() >= c.v();
		}
		bool operator==(Comparable &c) {
			return v() == c.v();
		}
		bool operator!=(Comparable &c) {
			return v() != c.v();
		}
		virtual float v() { return 0; }
	};
}