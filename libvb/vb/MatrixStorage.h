
/// @file MatrixStorage.h
/// Definitions of various kinds of matrix storage.

#ifndef __VB_MATRIXSTORAGE_H
#define __VB_MATRIXSTORAGE_H

#include <vb/Vector.h>

namespace vb {

  /** Abstract base class for a general matrix storage class.
   *
   * All matrix-matrix operations return a pointer to a (possibly 
   * different) MatrixStorage subclass, it is the responsibility of the 
   * surrounding vb::Matrix to juggle between them. Not sure that this 
   * is such a wonderful way of doing it, but it works pretty well and 
   * is transparent to the user.
   */

  template <typename T> class MatrixStorage {
    public:
      unsigned int width, height;

      MatrixStorage (unsigned int h, unsigned int w) : width(w), height(h) {}
      virtual ~MatrixStorage () {}

      virtual MatrixStorage<T> *copy () =0;
      virtual MatrixStorage<T> *compute () =0;

      virtual T at (unsigned int i, unsigned int j) const =0;
      virtual MatrixStorage<T> *put (unsigned int i, unsigned int j, const T &t) =0;
      virtual MatrixStorage<T> *add (MatrixStorage<T> *M) =0;
      virtual MatrixStorage<T> *sub (MatrixStorage<T> *M) =0;
      virtual MatrixStorage<T> *mul (MatrixStorage<T> *M) =0;
      virtual MatrixStorage<T> *rank1update (const Vector<T> &A, const Vector<T> &B) =0;
      virtual Vector<T> map_right (const Vector<T> &X) =0;
  };

  /** Plain old matrix, storing each entry separately.
   *
   * Internally it is just a vector of lines, themselves being vectors. 
   * This is so that scalar_product() can be used for matrix 
   * multiplication.
   */

  template <typename T> class MatrixStorage_Plain : public MatrixStorage<T> {
    public:
      std::vector< Vector<T> > lines;

      MatrixStorage_Plain (unsigned int h, unsigned int w) : MatrixStorage<T> (h,w), lines (std::vector< Vector<T> > (h,Vector<T>(w))) { }
      virtual ~MatrixStorage_Plain () {}

      virtual MatrixStorage<T> *copy () {
        return new MatrixStorage_Plain<T> (*this);
      }
      virtual MatrixStorage<T> *compute () { return this; }

      virtual T at (unsigned int i, unsigned int j) const {
        return lines[i][j];
      }
      virtual MatrixStorage<T> *put (unsigned int i, unsigned int j, const T &t) {
        lines[i][j] = t; return this;
      }
      virtual MatrixStorage<T> *add (MatrixStorage<T> *M) {
        for (unsigned int i=0; i<this->height; ++i)
          for (unsigned int j=0; j<this->width; ++j)
            lines[i][j] += M->at(i,j);
        return this;
      }
      virtual MatrixStorage<T> *sub (MatrixStorage<T> *M) {
        for (unsigned int i=0; i<this->height; ++i)
          for (unsigned int j=0; j<this->width; ++j)
            lines[i][j] -= M->at(i,j);
        return this;
      }
      virtual MatrixStorage<T> *mul (MatrixStorage<T> *M) {
        MatrixStorage_Plain<T> *tmp = new MatrixStorage_Plain<T> (this->height, M->width);
        for (unsigned int i=0; i<this->height; ++i) {
          for (unsigned int j=0; j<M->width; ++j) {
            tmp->lines[i][j] = lines[i][0] * M->at(0,j);
            for (unsigned int k=1; k<this->width; ++k) 
              tmp->lines[i][j] += lines[i][k] * M->at(k,j);
          }
        }
        return tmp;
      }
      virtual MatrixStorage<T> *rank1update (const Vector<T> &A, const Vector<T> &B) {
        for (unsigned int i=0; i<this->height; ++i)
          for (unsigned int j=0; j<this->width; ++j)
            lines[i][j] += A[i]*B[j];
        return this;
      }
      virtual Vector<T> map_right (const Vector<T> &X) {
        Vector<T> Y(this->height);
        for (unsigned int i=0; i<this->height; ++i)
          Y[i] = scalar_product (lines[i],X);
        return Y;
      }
  };

  /** Clever storage as a diagonal matrix plus rank-1 perturbations.
   *
   * It is extremely useful for BFGS algorithms and the like; 
   * rank1update() works in linear time (copy of the arguments) instead 
   * of quadratic, except whenever the matrix upgrades itself to 
   * MatrixStorage_Plain.
   *
   * @todo Generalize it as any matrix storage plus rank-1 updates ?
   */

  template <typename T> class MatrixStorage_DiagSmallRank : public MatrixStorage<T> {
    public:
      Vector<T> diag;
      Vector< std::pair< Vector<T>,Vector<T> > > updates;

      MatrixStorage_DiagSmallRank (unsigned int h, unsigned int w) : MatrixStorage<T> (h,w) {
        diag = Vector<T> (h);
        for (unsigned int i=0; i<h; ++i) diag[i]=0;
      }
      MatrixStorage_DiagSmallRank (unsigned int h, unsigned int w, const Vector<T> &d) : MatrixStorage<T> (h,w), diag(d) { }
      virtual ~MatrixStorage_DiagSmallRank () {}

      virtual MatrixStorage<T> *copy () {
        return new MatrixStorage_DiagSmallRank (*this);
      }
      MatrixStorage<T> *compute () {
        MatrixStorage_Plain<T> *tmp = new MatrixStorage_Plain<T> (this->height, this->width);
        for (unsigned int i=0; i<this->height; ++i) {
          for (unsigned int j=0; j<updates.size(); ++j)
            tmp->lines[i] += updates[j].first[i] * updates[j].second;
        }
        for (unsigned int i=0; i<(this->height<this->width?this->height:this->width); ++i)
          tmp->lines[i][i] += diag[i];
        return tmp;
      }

      virtual T at (unsigned int i, unsigned int j) const {
        T tmp = 0;
        if (i==j) tmp += diag[i];
        for (unsigned int k=0; k<updates.size(); ++k)
          tmp += updates[k].first[i] * updates[k].second[j];
        return tmp;
      }
      virtual MatrixStorage<T> *put (unsigned int i, unsigned int j, const T &t) {
        MatrixStorage<T> *tmp = this->compute();
        tmp->put (i,j,t);
        return tmp;
      }
      virtual MatrixStorage<T> *add (MatrixStorage<T> *M) {
        MatrixStorage<T> *tmp = this->compute();
        tmp->add(M);
        return tmp;
      }
      virtual MatrixStorage<T> *sub (MatrixStorage<T> *M) {
        MatrixStorage<T> *tmp = this->compute();
        tmp->sub(M);
        return tmp;
      }
      virtual MatrixStorage<T> *mul (MatrixStorage<T> *M) {
        MatrixStorage<T> *tmp = this->compute();
        tmp->mul(M);
        return tmp;
      }
      virtual MatrixStorage<T> *rank1update (const Vector<T> &A, const Vector<T> &B) {
        updates.push_back (std::pair< Vector<T>,Vector<T> > (A,B));
        if (updates.size() > this->height/10) {
          return this->compute();
        } else {
          return this;
        }
      }
      virtual Vector<T> map_right (const Vector<T> &X) {
        Vector<T> tmp(this->width);
        for (unsigned int i=0; i<(this->width<this->height?this->width:this->height); ++i) tmp[i] = diag[i]*X[i];
        for (unsigned int i=0; i<updates.size(); ++i) tmp += updates[i].first * scalar_product(updates[i].second,X);
        return tmp;
      }
  };
}

#endif

