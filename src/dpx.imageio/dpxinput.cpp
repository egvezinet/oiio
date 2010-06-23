/*
  Copyright 2010 Larry Gritz and the other authors and contributors.
  All Rights Reserved.

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are
  met:
  * Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.
  * Neither the name of the software's owners nor the names of its
    contributors may be used to endorse or promote products derived from
    this software without specific prior written permission.
  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

  (This is the Modified BSD License)
*/

#include "libdpx/DPX.h"

#include "typedesc.h"
#include "imageio.h"

using namespace OpenImageIO;



class DPXInput : public ImageInput {
public:
    DPXInput () : m_stream(NULL) { init(); }
    virtual ~DPXInput () { close(); }
    virtual const char * format_name (void) const { return "dpx"; }
    virtual bool open (const std::string &name, ImageSpec &newspec);
    virtual bool close ();
    virtual int current_subimage (void) const { return m_subimage; }
    virtual bool seek_subimage (int index, ImageSpec &newspec);
    virtual bool read_native_scanline (int y, int z, void *data);

private:
    int m_subimage;
    InStream *m_stream;
    dpx::Reader m_dpx;

    /// Reset everything to initial state
    ///
    void init () {
        if (m_stream) {
            m_stream->Close ();
            delete m_stream;
            m_stream = NULL;
        }
    }
};



// Obligatory material to make this a recognizeable imageio plugin:
extern "C" {

DLLEXPORT ImageInput *dpx_input_imageio_create () { return new DPXInput; }

DLLEXPORT int dpx_imageio_version = OPENIMAGEIO_PLUGIN_VERSION;

DLLEXPORT const char * dpx_input_extensions[] = {
    "dpx", NULL
};

};



bool
DPXInput::open (const std::string &name, ImageSpec &newspec)
{
    // open the image
    m_stream = new InStream();
    if (! m_stream->Open(name.c_str())) {
        error ("Could not open file \"%s\"", name.c_str());
        return false;
    }
    
    m_dpx.SetInStream(m_stream);
    if (! m_dpx.ReadHeader()) {
        error ("Could not read header");
        return false;
    }

    seek_subimage (0, m_spec);
    newspec = spec ();
    return true;
}



bool
DPXInput::seek_subimage (int index, ImageSpec &newspec)
{
    if (index < 0 || index >= m_dpx.header.ImageElementCount ())
        return false;

    m_subimage = index;

    // create imagespec
    TypeDesc typedesc;
    switch (m_dpx.header.ComponentDataSize(index)) {
        case dpx::kByte:
            typedesc = TypeDesc::UINT8;
            break;
        case dpx::kWord:
            typedesc = TypeDesc::UINT16;
            break;
        case dpx::kInt:
            typedesc = TypeDesc::UINT32;
            break;
        case dpx::kFloat:
            typedesc = TypeDesc::FLOAT;
            break;
        case dpx::kDouble:
            typedesc = TypeDesc::DOUBLE;
            break;
        default:
            error ("Invalid component data size");
            return false;
    }
    m_spec = ImageSpec (m_dpx.header.Width(), m_dpx.header.Height(),
        m_dpx.header.ImageElementComponentCount(index), typedesc);
    // fill channel names
    m_spec.channelnames.clear ();
    switch (m_dpx.header.ImageDescriptor(index)) {
        /*case dpx::kUserDefinedDescriptor:
            break;*/
        case dpx::kRed:
            m_spec.channelnames.push_back("R");
            break;
        case dpx::kGreen:
            m_spec.channelnames.push_back("G");
            break;
        case dpx::kBlue:
            m_spec.channelnames.push_back("B");
            break;
        case dpx::kAlpha:
            m_spec.channelnames.push_back("A");
            m_spec.alpha_channel = 0;
            break;
        case dpx::kLuma:
            m_spec.channelnames.push_back("I");
            break;
        case dpx::kDepth:
            m_spec.channelnames.push_back("Z");
            m_spec.z_channel = 0;
            break;
        /*case dpx::kCompositeVideo:
            break;*/
        case dpx::kRGB:
        case dpx::kRGBA:
            m_spec.default_channel_names ();
            break;
        case dpx::kCbYCrY:
            m_spec.channelnames.push_back("Cb");
            m_spec.channelnames.push_back("Y");
            m_spec.channelnames.push_back("Cr");
            //m_spec.channelnames.push_back("Y");
            break;
        case dpx::kCbYACrYA:
            m_spec.channelnames.push_back("Cb");
            m_spec.channelnames.push_back("Y");
            m_spec.channelnames.push_back("A");
            m_spec.channelnames.push_back("Cr");
            /*m_spec.channelnames.push_back("Y");
            m_spec.channelnames.push_back("A");*/
            m_spec.alpha_channel = 2;
            break;
        case dpx::kCbYCr:
            m_spec.channelnames.push_back("Cb");
            m_spec.channelnames.push_back("Y");
            m_spec.channelnames.push_back("Cr");
            break;
        case dpx::kCbYCrA:
            m_spec.channelnames.push_back("Cb");
            m_spec.channelnames.push_back("Y");
            m_spec.channelnames.push_back("Cr");
            m_spec.channelnames.push_back("A");
            m_spec.alpha_channel = 3;
            break;
        default:
            {
                for (int i = 0;
                    i < m_dpx.header.ImageElementComponentCount(index); i++) {
                    std::string ch = "channel" + i;
                    m_spec.channelnames.push_back(ch);
                }
            }
    }
    // bits per pixel
    m_spec.attribute ("BitsPerSample", m_dpx.header.BitDepth(index)
        * m_dpx.header.ImageElementComponentCount(index));
    // image orientation - see appendix B.2 of the OIIO documentation
    int orientation;
    switch (m_dpx.header.ImageOrientation ()) {
        case dpx::kLeftToRightTopToBottom:
            orientation = 1;
            break;
        case dpx::kRightToLeftTopToBottom:
            orientation = 2;
            break;
        case dpx::kLeftToRightBottomToTop:
            orientation = 4;
            break;
        case dpx::kRightToLeftBottomToTop:
            orientation = 3;
            break;
        case dpx::kTopToBottomLeftToRight:
            orientation = 5;
            break;
        case dpx::kTopToBottomRightToLeft:
            orientation = 6;
            break;
        case dpx::kBottomToTopLeftToRight:
            orientation = 8;
            break;
        case dpx::kBottomToTopRightToLeft:
            orientation = 7;
            break;
        default:
            orientation = 0;
            break;
    }
    m_spec.attribute ("Orientation", orientation);
        
    // general metadata
    m_spec.attribute ("Copyright", m_dpx.header.copyright);
    m_spec.attribute ("Software", m_dpx.header.creator);
    m_spec.attribute ("DocumentName", m_dpx.header.project);

    // DPX-specific metadata
    m_spec.attribute ("dpx:EncryptKey", m_dpx.header.EncryptKey ());
    
    return true;
}



bool
DPXInput::close ()
{
    if (m_stream) {
        m_stream->Close ();
        delete m_stream;
        m_stream = NULL;
    }
    
    init();  // Reset to initial state
    return true;
}



bool
DPXInput::read_native_scanline (int y, int z, void *data)
{
    dpx::Block block(0, y, m_dpx.header.Width () - 1, y);

    if (!m_dpx.ReadBlock(data, m_dpx.header.ComponentDataSize(m_subimage),
        block, m_dpx.header.ImageDescriptor(m_subimage)))
        return false;
    return true;
}