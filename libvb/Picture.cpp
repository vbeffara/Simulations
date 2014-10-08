/// @file
/// Implementation of the vb::Picture class

#include <vb/Picture.h>
#include <FL/Fl.H>
#include <FL/fl_draw.H>
#include <iostream>
#include <iomanip>
#include <sstream>

namespace vb {
  Picture::Picture (int wd, int ht, const std::string &t) :
    AutoWindow (wd, ht, t.c_str()),

    surface (cairo_image_surface_create (CAIRO_FORMAT_RGB24, w(), h())),
    cr      (cairo_create (surface)),
    stride  (cairo_image_surface_get_stride (surface) / sizeof(Color)),

    snapshot_prefix(t), snapshot_number(0), snapshot_period(0.0), snapshot_task(-1)
  { }

  Picture::~Picture () {
    if (snapshot_task>=0) remove_task(snapshot_task);
  }

  void Picture::size (int wd, int ht) {
    AutoWindow::size (wd,ht);
    cairo_destroy (cr);
    cairo_surface_destroy (surface);
    surface = cairo_image_surface_create (CAIRO_FORMAT_RGB24, w(), h());
    cr      = cairo_create (surface);
    stride  = cairo_image_surface_get_stride (surface) / sizeof(Color);
 }

  void draw_cb (void * in, int x, int y, int w, unsigned char * out) {
    Picture & img = * (Picture*) in;
    Color   * src = (Color*) (cairo_image_surface_get_data(img.surface));

    for (int i=0; i<w; ++i) {
      Color &C = src [x+i + img.stride*y];
      out[3*i] = C.r; out[3*i + 1] = C.g; out[3*i + 2] = C.b;
    }
  }

  void Picture::draw () {
    paint ();
    fl_draw_image (draw_cb,this,0,0,w(),h());
  }

  void Picture::output_png (const std::string &s) {
    paint();

    std::ostringstream os;
    if (s == "") os << "output/" << (prog!="" ? prog+"/" : "") << title; else os << s;
    os << ".png";

    cairo_surface_write_to_png (surface, os.str().c_str());
  }

  void Picture::output (const std::string &s) { output_png (s); }

  void Picture::snapshot () {
    std::ostringstream fn_s;
    fn_s << "snapshots/" << snapshot_prefix << "_" << std::setw(4) << std::setfill('0') << snapshot_number++;
    std::string fn = fn_s.str();

    std::cerr << "Taking a snapshot as " << fn << ".png" << std::endl;
    output_png (fn);
  }

  void Picture::snapshot_setup (const std::string &prefix, double period) {
    if (snapshot_task>=0) remove_task(snapshot_task);
    snapshot_period = period;
    snapshot_prefix = prefix;
    snapshot();
    if (period>0) snapshot_task = add_task (period, [this]{this->snapshot();});
  }

  int Picture::handle (int event) {
    if ((event == FL_KEYDOWN) && (Fl::event_key() == 's')) snapshot();
    return AutoWindow::handle (event);
  }
}
