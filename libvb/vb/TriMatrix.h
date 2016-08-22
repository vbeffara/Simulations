#pragma once /// \file
#include <vb/coo.h>
#include <vector>

namespace vb {
	#define BSIZE 128

	template <typename T> class TriMatrix {
	public:
		TriMatrix (T e = 0) : size(0), sub_size(0), sub_shift(0), empty(e) { }

		T at (coo z) const {
			int target = std::max (std::abs(z.x), std::abs(z.y)); if (target >= size) return empty;
			if (size==BSIZE) return tile [(2*BSIZE+1)*BSIZE + z.x + 2*BSIZE*z.y];
			int index = 4;
			if (z.x >= sub_size) { index += 1; z.x -= sub_shift; } else if (z.x <= - sub_size) { index -= 1; z.x += sub_shift; }
			if (z.y >= sub_size) { index += 3; z.y -= sub_shift; } else if (z.y <= - sub_size) { index -= 3; z.y += sub_shift; }
			return sub[index].at(z);
		}

		void put (coo z, T t) {
			if (size==0) { tile.resize (2*BSIZE*2*BSIZE, empty); size = BSIZE; }
			int target = std::max (std::abs(z.x), std::abs(z.y)); while (size <= target) triple();
			if (size==BSIZE) tile [(2*BSIZE+1)*BSIZE + z.x + 2*BSIZE*z.y] = t; else {
				int index = 4;
				if (z.x >= sub_size) { index += 1; z.x -= sub_shift; } else if (z.x <= - sub_size) { index -= 1; z.x += sub_shift; }
				if (z.y >= sub_size) { index += 3; z.y -= sub_shift; } else if (z.y <= - sub_size) { index -= 3; z.y += sub_shift; }
				sub[index].put(z,t);
			}
		}

	private:
		void triple () {
			if (size==0) return;
			std::vector < TriMatrix<T> > tmp_sub (9, empty);
			tmp_sub.swap (sub); tmp_sub.swap (sub[4].sub);
			sub[4].tile.swap(tile);
			sub[4].size = size; sub[4].sub_size = sub_size; sub[4].sub_shift = sub_shift;
			sub_size = size; size = 3*size-1; sub_shift = size - sub_size;
		}

		int size, sub_size, sub_shift;
		T empty;
		std::vector <T> tile;
		std::vector <TriMatrix<T>> sub;
	};
}
