/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2008 Larry Gritz
// 
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the
// "Software"), to deal in the Software without restriction, including
// without limitation the rights to use, copy, modify, merge, publish,
// distribute, sublicense, and/or sell copies of the Software, and to
// permit persons to whom the Software is furnished to do so, subject to
// the following conditions:
// 
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
// LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
// OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
// WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
// 
// (this is the MIT license)
/////////////////////////////////////////////////////////////////////////////


#include <cstdio>
#include <cstdlib>
#include <cmath>

#include "dassert.h"
#include "paramtype.h"
#include "imageio.h"


using namespace OpenImageIO;


class DLLPUBLIC TIFFOutput : public ImageOutput {
public:
    TIFFOutput ();
    virtual ~TIFFOutput ();
    virtual bool supports (const char *feature) const;
    virtual bool open (const char *name, const ImageIOFormatSpec &spec,
        int nparams, const ImageIOParameter *param, bool append=false);
    virtual bool write_scanline (int y, int z, ParamBaseType format,
                                 const void *data, int xstride);
    virtual bool write_tile (int x, int y, int z,
                             ParamType format, const void *data,
                             int xstride, int ystride, int zstride);
    virtual bool write_rectangle (int xmin, int xmax, int ymin, int ymax,
                                  int zmin, int zmax,
                                  ParamType format, const void *data,
                                  int xstride, int ystride, int zstride);
    virtual bool close ();
};



TIFFOutput::TIFFOutput ()
{
}



TIFFOutput::~TIFFOutput ()
{
}



bool
TIFFOutput::supports (const char *feature) const
{
}



bool
TIFFOutput::open (const char *name, const ImageIOFormatSpec &spec,
                 int nparams, const ImageIOParameter *param, bool append)
{
}



bool
TIFFOutput::write_scanline (int y, int z, ParamBaseType format,
                           const void *data, int xstride)
{
}



bool
TIFFOutput::write_tile (int x, int y, int z,
                       ParamType format, const void *data,
                       int xstride, int ystride, int zstride)
{
}



bool
TIFFOutput::write_rectangle (int xmin, int xmax, int ymin, int ymax,
                            int zmin, int zmax,
                            ParamType format, const void *data,
                            int xstride, int ystride, int zstride)
{
}



bool
TIFFOutput::close ()
{
}

