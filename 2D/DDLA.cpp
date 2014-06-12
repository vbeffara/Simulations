#include <vb/CL_Parser.h>
#include <vb/Image.h>
#include <vb/PRNG.h>
#include <vb/Point.h>

using namespace vb;
using namespace std;

int cursum;
int n,maxx,maxy;
double p;
bool s;

class DDLA : public CoarseImage {
public:
	DDLA (CL_Parser & CLP) : CoarseImage (CLP('n'),CLP('n'),"A directed DLA cluster", CLP('f') ? pow(double(n),.33) : 1) {};

	void reshape (int x, int y) {
		if (x==maxx) ++maxx;
		if (y==maxy) ++maxy;
		if (x+y>cursum) cursum=x+y;
	}

	bool ok (coo z) {
		int xx=z.x, yy=z.y;
		coo zz=z;
		bool just_started=true;
		int ans=-1;

		while (ans == -1) {
			if (prng.bernoulli(p)) { ++xx; zz+=coo(1,0); } else { ++yy; zz+=coo(0,1); }

			if ((xx>=n) || (yy>=n) || (xx+yy > cursum)) ans = 1;
			if (at(zz) == 1) ans = 0;

			if (s && (!just_started) && (xx<n-1) && (yy<n-1) && ((at(zz+coo(1,0))==1) || (at(zz+coo(0,1))==1))) ans = 0;
			just_started = false;
		}

		return ans;
	}

	void addapoint (coo z) {
		put (z,1);
		if (f) {
			int x=z.x, y=z.y;
			if ( (x>0) && (y<n-1) && (at(z-coo(1,0)+coo(0,1))==1) && (at(z-coo(1,0))==0) ) addapoint (z-1);
			if ( (y>0) && (x<n-1) && (at(z+coo(1,0)-coo(0,1))==1) && (at(z-coo(0,1))==0) ) addapoint (z-J);
		}
	}

	bool f;
};

int main (int argc, char **argv) {
	CL_Parser CLP (argc,argv,"n=500,p=.5,s,t,f");
	n = CLP('n');
	p = CLP('p');
	s = CLP('s');

	PointQueue queue;

	maxx=1; maxy=1; cursum=0;

	DDLA img (CLP);
	img.f = CLP('f');
	img.show();

	img.put(0,1);
	queue << Point(coo(1,0),prng.exponential()) << Point(coo(0,1),prng.exponential());

	int running=1;
	while (running) {
		Point pt (queue);

		if ((pt.x==n-1) || (pt.y==n-1)) running=0;
		else if (img.at(pt) == 0) {
			double curtime = pt.t;
			if (img.ok(pt)) {
				img.addapoint (pt);
				img.reshape (pt.x,pt.y);
				if ( (pt.x<n-1) && (img.at(pt+coo(1,0))==0) ) queue << Point(pt+coo(1,0),curtime+prng.exponential()/p);
				if ( (pt.y<n-1) && (img.at(pt+coo(0,1))==0) ) queue << Point(pt+coo(0,1),curtime+prng.exponential()/(1-p));
				if ( s && (pt.x>0) && (img.at(pt-coo(1,0))==0) ) queue << Point(pt-coo(1,0),curtime+prng.exponential());
				if ( s && (pt.y>0) && (img.at(pt-coo(0,1))==0) ) queue << Point(pt-coo(0,1),curtime+prng.exponential());
			} else {
				queue << Point(pt,curtime+prng.exponential());
			}
		}
	}

	img.output();
	return 0;
}
