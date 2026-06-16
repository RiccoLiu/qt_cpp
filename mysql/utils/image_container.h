#ifndef UTILS_IMAGE_CONTAINER_H
#define UTILS_IMAGE_CONTAINER_H

#include <Eigen/Core>           // Eigen
#include <Eigen/Dense>

#include "fixedpoint.h"

namespace utils {

/**
* @brief Generic image class,
* Image data are stored by inheritance of an Eigen matrix.
* @tparam T Pixel type
*/
template <typename T>
class Image : protected Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>
{
public:

    /// Pixel data type
    using Tpixel = T;

    /// Full internal type
    using Base = Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>;


    /**
     * @brief Default constructor
     * @note This create an empty image
     */
    inline Image()
    {
        mWidth = 0;
        mHeight = 0;
        mPaddingHeight = 0;

        Base::resize( 0, 0 );
    }

    /**
    * @brief Full constructor
    * @param width Width of the image (ie number of column)
    * @param height Height of the image (ie number of row)
    * @param fInit Tell if the image should be initialized
    * @param val If fInit is true, set all pixel to the specified value
    */
    inline Image( int width, int height, bool fInit = true, const T val = T() )
    {
        mWidth = width;
        mHeight = height;
        mPaddingHeight = 0;

        Base::resize( height, width );
        if ( fInit )
        {
            Base::fill( val );
        }
    };

    /**
    * @brief Full constructor
    * @param width Width of the image (ie number of column)
    * @param height Height of the image (ie number of row)
    * @param paddingH Padding Height of the image
    * @param fInit Tell if the image should be initialized
    * @param val If fInit is true, set all pixel to the specified value
    */
    inline Image( int width, int height, int paddingH, bool fInit = true, const T val = T() )
    {
        mWidth = width;
        mHeight = height;
        mPaddingHeight = paddingH;

        Base::resize( height+paddingH, width );
        if ( fInit )
        {
            Base::fill( val );
        }
    };

    /**
    * @brief Copy constructor
    * @param I Source image
    */
    explicit inline Image( const Base& I )
        : Base( I )
    {
        mWidth = I.cols();
        mHeight = I.rows();
        mPaddingHeight = 0;
    }

    /**
    * @brief Move constructor
    * @param src Source image
    */
    explicit inline Image( Base && src )
        : Base( std::move( src ) )
    {
        mWidth = src.cols();
        mHeight = src.rows();
        mPaddingHeight = 0;
    }

    /**
    * @brief Assignment operator
    * @param I Source image
    * @return Image after assignment
    */
    inline Image& operator=( const Base& I )
    {
        mWidth = I.cols();
        mHeight = I.rows();
        mPaddingHeight = 0;

        Base::operator=( I );
        return *this;
    }

    /**
    * @brief destructor
    */
    virtual inline ~Image() = default;

    //------------------------------
    //-- accessors/getters methods
    /**
     * @brief Retrieve the width of the image
     * @return Width of image
     */
    inline int Width()  const
    {
        return mWidth;
    }

    /**
     * @brief Retrieve the height of the image
     * @return Height of the image
     */
    inline int Height() const
    {
        return mHeight;
    }

    /**
     * @brief Retrieve the padding height of the image
     * @return Height of the image
     */
    inline int Padding() const
    {
        return mPaddingHeight;
    }

    /**
    * @brief Reset Height & Padding Height
    * @param height Height of the image (ie number of row)
    * @param paddingH Padding Height of the image
    */
    inline void ResetV( int height, int padding )
    {
        assert(mHeight+mPaddingHeight==(height+padding));
        mHeight = height;
        mPaddingHeight = padding;
    };

    /**
    * @brief Return the depth in byte of the pixel
    * @return depth of the pixel (in byte)
    * @note (T=unsigned char will return 1)
    */
    inline int Depth() const
    {
        return sizeof( Tpixel );
    }

    /**
    * @brief get data pointer
    * @retval data pointer
    */
    inline void* Data()
    {
        return Base::data();
    }

    /**
    * @brief get data pointer
    * @retval const data pointer
    */
    inline const void* Data() const
    {
        return Base::data();
    }

    /**
     * @brief Fill the content of image
     * @param val set value
     */
    inline void Fill( const T val = T( 0 ) )
    {
        Base::fill( val );
    }

    /**
    * @brief constant random pixel access
    * @param y Index of the row
    * @param x Index of the column
    * @return Constant pixel reference at position (y,x)
    */
    inline const T& operator()( int y, int x ) const
    {
        return Base::operator()( y, x );
    }

    /**
     * @brief random pixel access
     * @param y Index of the row
     * @param x Index of the column
     * @return Pixel reference at position (y,x)
     */
    inline T& operator()( int y, int x )
    {
        return Base::operator()( y, x );
    }

    /**
    * @brief constant random pixel access (suppose image as a line array)
    * @param p position
    * @return Pixel reference at position (p)
    */
    inline const T& operator[](int p) const
    {
        return Base::operator()(p);
    }
    /**
    * @brief constant random pixel access (suppose image as a line array)
    * @param p position
    * @return Pixel reference at position (p)
    */
    inline T& operator[](int p)
    {
        return Base::operator()(p);
    }

    /**
    * @brief Get low level access to the internal pixel data
    * @return const reference to internal matrix data
    */
    inline const Base& GetMat() const
    {
        return ( *this );
    }

    //-- accessors/getters methods
    //------------------------------

    /**
    * @brief Tell if a point is inside the image.
    * @param y Index of the row
    * @param x Index of the column
    * @retval true If pixel (y,x) is inside the image
    * @retval false If pixel (y,x) is outside the image
    */
    inline bool Contains( int y, int x ) const
    {
        return 0 <= x && x < Base::cols()
        && 0 <= y && y < Base::rows();
    }

    /**
    * @brief Pixelwise addition of two images
    * @param imgA First image
    * @param imgB Second image
    * @return pixelwise imgA + imgB
    * @note Images must have the same size
    */
    template<typename T1>
    friend Image<T1> operator+( const Image<T1> & imgA , const Image<T1> & imgB );

    /**
    * @brief Pixelwise subtraction of two images
    * @param imgA First image
    * @param imgB Second image
    * @return pixelwise imgA - imgB
    * @note Images must have the same size
    */
    template<typename T1>
    friend Image<T1> operator-( const Image<T1> & imgA , const Image<T1> & imgB );

    /**
    * @brief interpolated pixel
    * @param y Index of the row
    * @param x Index of the column
    * @retval true If pixel (x,y) is inside the image
    * @retval false If pixel (x,y) is outside the image
    */
    template<typename T1,typename T2>
    inline T1 Interpolate( T2 x, T2 y ) const
    {
        // LOG_FATAL("common::Image::interpolate not implement!");
        return T1();
    }

private:
    /**
     * @brief Change geometry of image(user can not change size)
     * @param width New width of image
     * @param height New height of image
     * @param fInit Indicate if new image should be initialized
     * @param val if fInit is true all pixel in the new image are set to this value
     */
    inline void resize( int width, int height, bool fInit = true, const T val = T( 0 ) )
    {
        mWidth = width;
        mHeight = height;
        mPaddingHeight = 0;

        Base::resize( height, width );
        if ( fInit )
        {
            Base::fill( val );
        }
    }

private:
    int mWidth;
    int mHeight;
    int mPaddingHeight;

public:
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW
};

/**
* @brief Pixelwise addition of two images
* @param imgA First image
* @param imgB Second image
* @return pixelwise imgA + imgB
* @note Images must have the same size
*/
template<typename T1>
Image<T1> operator+( const Image<T1> & imgA , const Image<T1> & imgB )
{
    return Image<T1>( imgA.Image<T1>::operator+( imgB ) );
}

/**
* @brief Pixelwise subtraction of two images
* @param imgA First image
* @param imgB Second image
* @return pixelwise imgA - imgB
* @note Images must have the same size
*/
template<typename T1>
Image<T1> operator-( const Image<T1> & imgA , const Image<T1> & imgB )
{
    return Image<T1>( imgA.Image<T1>::operator-( imgB ) );
}

/**
* @brief interpolated pixel
* @param y Index of the row
* @param x Index of the column
* @retval true If pixel (y,x) is inside the image
* @retval false If pixel (y,x) is outside the image
*/
template<>
template<>
inline float Image<uint8_t>::Interpolate<float,float>( float fx, float fy ) const
{
    uint32_t ix = fx;
    uint32_t iy = fy;
    uint32_t dx = (fx - ix)*FIXED32_ONE;
    uint32_t dy = (fy - iy)*FIXED32_ONE;
    uint32_t dxdy = dx * dy; dxdy >>= FIXED32_WBITS;

    int width = Width();
    uint8_t* I00 = ((uint8_t*)Data()) + ix + iy * width;
    uint8_t* I01 = I00 + 1;
    uint8_t* I10 = I00 + width;
    uint8_t* I11 = I00 + 1 + width;

    uint32_t value = dxdy * (*I11)
                       + (dy - dxdy) * (*I10)
                       + (dx - dxdy) * (*I01)
                       + (FIXED32_ONE - dx - dy + dxdy) * (*I00);

    return FIXED32_2FLOAT(value);
}

/**
* @brief interpolated pixel
* @param y Index of the row
* @param x Index of the column
* @retval true If pixel (y,x) is inside the image
* @retval false If pixel (y,x) is outside the image
*/
template<>
template<>
inline uint8_t Image<uint8_t>::Interpolate<uint8_t,int>( int x, int y ) const
{
    return *(((uint8_t*)Data()) + x + y * Width());
}

} // namespace utils

#endif // UTILS_IMAGE_CONTAINER_H


