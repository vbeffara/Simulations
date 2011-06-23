/// @file
/// Implementation of the vb::Picture class

#include <vb/Picture.h>

namespace vb {
  void Picture_update   (void * AW) { ((Picture*)AW) -> update();   }
  void Picture_snapshot (void * AW) { ((Picture*)AW) -> snapshot(); }

  Picture::Picture (int wd, int ht, const std::string &t) :
    AutoWindow (wd, ht, t.c_str()),

    surface (Cairo::ImageSurface::create (Cairo::FORMAT_RGB24, w(), h())),
    stride  (surface -> get_stride() / sizeof(Color)),
    cr      (Cairo::Context::create (surface)),

    snapshot_prefix("snapshot"), snapshot_number(0), snapshot_period(0.0), snapshot_task(-1)
  { }

  Picture::~Picture () {
    global_clock.remove(snapshot_task);
  }

  void Picture::size (int wd, int ht) {
    AutoWindow::size (wd,ht);
    surface = Cairo::ImageSurface::create (Cairo::FORMAT_RGB24, w(), h());
    stride = surface -> get_stride() / sizeof(Color);
    cr = Cairo::Context::create (surface);
  }

#ifdef HAVE_FLTK
  void draw_cb (void * in, int x, int y, int w, unsigned char * out) {
    Picture & img  = * (Picture*) in;
    Color      * data = (Color*) img.surface -> get_data();

    for (int i=0; i<w; ++i) {
      Color &C = data [x+i + img.stride*y];
      out[3*i] = C.r; out[3*i + 1] = C.g; out[3*i + 2] = C.b;
    }
  }

  void Picture::draw () {
    paint ();
    fl_draw_image (draw_cb,this,0,0,w(),h());
  }
#endif

  void Picture::output_png (const std::string &s) {
    paint();

    std::ostringstream os;
    if (s == "") os << "output/" << title; else os << s;
    os << ".png";

    surface->write_to_png (os.str());
  }

  void Picture::output (const std::string &s) { output_png (s); }

  void Picture::snapshot () {
    std::ostringstream fn_s;
    fn_s << snapshot_prefix << "_" << std::setw(4) << std::setfill('0') << snapshot_number++;
    std::string fn = fn_s.str();

    std::cerr << "Taking a snapshot as " << fn << ".png" << std::endl;
    output_png (fn);
  }

  void Picture::snapshot_setup (const std::string &prefix, double period) {
    snapshot_period = period;
    snapshot_prefix = prefix;
    snapshot_number = 0;
    global_clock.add (100*period, Picture_snapshot, this);
  }
}
