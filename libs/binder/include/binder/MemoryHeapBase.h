/*
 * Copyright (C) 2008 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once

#include <stdlib.h>
#include <stdint.h>

#include <binder/IMemory.h>


namespace android {

// ---------------------------------------------------------------------------

class MemoryHeapBase : public virtual BnMemoryHeap
{
public:
    enum {
        READ_ONLY = IMemoryHeap::READ_ONLY,
        // memory won't be mapped locally, but will be mapped in the remote
        // process.
        DONT_MAP_LOCALLY = 0x00000100,
        NO_CACHING = 0x00000200
    };

    /*
     * maps the memory referenced by fd. but DOESN'T take ownership
     * of the filedescriptor (it makes a copy with dup()
     */
    MemoryHeapBase(int fd, size_t size, uint32_t flags = 0, off_t offset = 0);

    /*
     * maps memory from the given device
     */
    explicit MemoryHeapBase(const char* device, size_t size = 0, uint32_t flags = 0);

    /*
     * maps memory from ashmem, with the given name for debugging
     */
    explicit MemoryHeapBase(size_t size, uint32_t flags = 0, char const* name = nullptr);

    virtual ~MemoryHeapBase();

    /* implement IMemoryHeap interface */
    virtual int         getHeapID() const;

    /* virtual address of the heap. returns MAP_FAILED in case of error */
    virtual void*       getBase() const;

    virtual size_t      getSize() const;
    virtual uint32_t    getFlags() const;
            off_t       getOffset() const override;

    const char*         getDevice() const;

    /* this closes this heap -- use carefully */
    void dispose();

    /* this is only needed as a workaround, use only if you know
     * what you are doing */
    status_t setDevice(const char* device) {
        if (mDevice == nullptr)
            mDevice = device;
        return mDevice ? NO_ERROR : ALREADY_EXISTS;
    }

protected:
            MemoryHeapBase();
    // init() takes ownership of fd
    status_t init(int fd, void *base, size_t size,
            int flags = 0, const char* device = nullptr);

private:
    status_t mapfd(int fd, size_t size, off_t offset = 0);

    int         mFD;
    size_t      mSize;
    void*       mBase;
    uint32_t    mFlags;
    const char* mDevice;
    bool        mNeedUnmap;
    off_t       mOffset;
};

// ---------------------------------------------------------------------------
} // namespace android
