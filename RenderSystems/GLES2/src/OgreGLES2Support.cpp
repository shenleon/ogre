/*
-----------------------------------------------------------------------------
This source file is part of OGRE
    (Object-oriented Graphics Rendering Engine)
For the latest info, see http://www.ogre3d.org/

Copyright (c) 2000-2014 Torus Knot Software Ltd

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
-----------------------------------------------------------------------------
*/

#include "OgreGLES2Support.h"

#include "OgreLogManager.h"

namespace Ogre {
    void GLES2Support::initialiseExtensions(void)
    {
        String tmpStr;
#if 1
        // Set version string
        const GLubyte* pcVer = glGetString(GL_VERSION);
        assert(pcVer && "Problems getting GL version string using glGetString");
        tmpStr = (const char*)pcVer;

        // format explained here:
        // https://www.khronos.org/opengles/sdk/docs/man/xhtml/glGetString.xml
        size_t offset = sizeof("OpenGL ES ") - 1;
        if(tmpStr.length() > offset) {
            mVersion.fromString(tmpStr.substr(offset, tmpStr.find(" ", offset)));
        }
#else
        // GLES3 way, but should work with ES2 as well, so disabled for now
        glGetIntegerv(GL_MAJOR_VERSION, &mVersion.major);
        glGetIntegerv(GL_MINOR_VERSION, &mVersion.minor);
#endif

        LogManager::getSingleton().logMessage("GL_VERSION = " + mVersion.toString());


        // Get vendor
        const GLubyte* pcVendor = glGetString(GL_VENDOR);
        tmpStr = (const char*)pcVendor;
        LogManager::getSingleton().logMessage("GL_VENDOR = " + tmpStr);
        mVendor = tmpStr.substr(0, tmpStr.find(" "));

        // Get renderer
        const GLubyte* pcRenderer = glGetString(GL_RENDERER);
        tmpStr = (const char*)pcRenderer;
        LogManager::getSingleton().logMessage("GL_RENDERER = " + tmpStr);

        // Set extension list
        StringStream ext;
        String str;

        const GLubyte* pcExt = glGetString(GL_EXTENSIONS);
        OgreAssert(pcExt, "Problems getting GL extension string using glGetString");
        ext << pcExt;
        
        Log::Stream log = LogManager::getSingleton().stream();
        log << "GL_EXTENSIONS = ";
        while (ext >> str)
        {
#if OGRE_PLATFORM == OGRE_PLATFORM_EMSCRIPTEN
            // emscripten gives us both prefixed (GL_) and unprefixed (EXT_) forms
            // use prefixed form (GL_EXT) unless its a WebGL extension (WEBGL_)
            if ((str.substr(0, 3) != "GL_" && str.substr(0, 6) != "WEBGL_") || str.substr(0, 9) == "GL_WEBGL_")
                continue;
#endif
            log << str << " ";
            extensionList.insert(str);
        }
    }

    bool GLES2Support::hasMinGLVersion(int major, int minor) const
    {
        if (mVersion.major == major) {
            return mVersion.minor >= minor;
        }
        return mVersion.major > major;
    }

    bool GLES2Support::checkExtension(const String& ext) const
    {
        return extensionList.find(ext) != extensionList.end() || mNative->checkExtension(ext);
    }
}
