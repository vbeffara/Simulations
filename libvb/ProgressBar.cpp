#include <vb/ProgressBar.h>
#include <gsl/gsl>
#include <iostream>
#include <iomanip>
#include <cmath>

namespace vb {
    ProgressBar::ProgressBar (int length, double pow) : Auto(.05), final(length), power(pow) {}

    ProgressBar::~ProgressBar () {
        set(final); update(); std::cerr << "\n";
    }

    void ProgressBar::set (int pos) {
        if (pos<0) pos=0; else if (pos>final) pos=final;
        current = pos; step();
    }

    void ProgressBar::update () {
        static const std::string symbols[] = { u8" ", u8"▏", u8"▎", u8"▍", u8"▌", u8"▋", u8"▊", u8"▉", u8"█" };

        std::cerr << "\r[";
        int nchar = (current*50.0)/final;
        int prop = 8*((current*50.0)/final - nchar);
        for (int i=0; i<nchar; ++i)  std::cerr << symbols[8];
        if (nchar<50) std::cerr << gsl::at (symbols,prop);
        for (int i=nchar+1; i<50; ++i) std::cerr << " ";
        std::cerr << "]";

        if (current>0) {
            double done  = pow(current,power), todo = pow(final,power);
            Duration etd = (now()-start) * (todo/done - 1.0);
            int eta = etd.count(); bool big = false;

            std::cerr << " (";
            if (eta >= 3600*24) {
                std::cerr << eta/3600/24 << "d ";
                eta = eta % 3600*24;
                big = true;
            }
            if (big || (eta >= 3600)) {
                std::cerr << eta/3600 << ":" << std::setw(2) << std::setfill('0');
                eta = eta % 3600;
            }
            std::cerr << eta/60 << ":" << std::setw(2) << std::setfill('0') << eta%60 << ")   ";
        }
    }
}
