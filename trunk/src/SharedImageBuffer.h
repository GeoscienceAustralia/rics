/*
Author: Nariman Habili

Description: Image data buffer.

Copyright (c) 2011-2012 Commonwealth of Australia (Geoscience Australia)

This file is part of RICS.

RICS is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

RICS is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with RICS.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef SHARED_IMAGE_BUFFER
#define SHARED_IMAGE_BUFFER

#include "wx/thread.h"
#include <boost/shared_array.hpp>
#include <boost/shared_ptr.hpp>

namespace rics
{
    class SharedImageBuffer
    {
    public:
        SharedImageBuffer():
        cond_(mutex_)
        {
        }

        ~SharedImageBuffer()
        {
        }
        
        void readLock()
        {
            mutex_.Lock();
        }

        void readUnlock()
        {
            cond_.Signal();
            mutex_.Unlock();
        }

        void writeLock()
        {
            mutex_.Lock();
        }

        void writeUnlock()
        {
            cond_.Wait();
            mutex_.Unlock();
        }

        boost::shared_array<unsigned char> data() const
        {
            return data_;
        }

        void setData(const boost::shared_array<unsigned char>& data)
        {
            data_ = data;
        }
        
    private:
        wxMutex mutex_;
        wxCondition cond_;

        boost::shared_array<unsigned char> data_;
    };

    typedef boost::shared_ptr<SharedImageBuffer> SharedImageBufferPtr;

}//namespace

#endif //SHARED_IMAGE_BUFFER