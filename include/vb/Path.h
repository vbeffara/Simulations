#pragma once /// @file
#include <string>
#include <vector>

namespace vb {
    /** A class to describe a path in Z^2.
     */

    class OldPath : public std::vector<uint8_t> {
    public:
        /** Create a vb::Path of a given length.
         *
         * @param l      The length of the path.
         * @param title_ The title of the generated image.
         * @param rel    Whether the path is stored as relative or absolute jumps.
         */

        explicit OldPath(size_t l, bool rel = false);

        /** Output the path as an image.
         *
         * This uses the default format (i.e., PDF), and the file name
         * defaults to the title of the vb::Path opbject.
         *
         * @param s The filename of the output.
         */

        void output(const std::string &s) const;

        /** Output the path as a PDF file.
         *
         * @param s The filename of the output.
         */

        void output_pdf(const std::string &s) const;

        /// Check whether the path is self-avoiding.
        bool self_avoiding();

    private:
        std::string title;
        bool        relative;
    };
} // namespace vb
