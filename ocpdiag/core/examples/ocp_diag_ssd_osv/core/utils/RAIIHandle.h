// Copyright 2024 Samsung Electronics
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#ifndef OCP_DIAG_SSD_OSV_RAIIHANDLE_H
#define OCP_DIAG_SSD_OSV_RAIIHANDLE_H

template<
        typename handleT,
        template<typename handleN> class deleteT,
        handleT invalidHandleValue
>

class RAIIHandleT
{
public:
    typedef handleT handleValueType;

public:
    RAIIHandleT() : m_H(invalidHandleValue) {}
    RAIIHandleT(handleT h) :m_H(h) {}
    RAIIHandleT(RAIIHandleT&& h) : m_H(std::move(h))
    {
    }

    ~RAIIHandleT()
    {
        if (m_H != invalidHandleValue)
        {
            deleteT<handleT> deletor;
            deletor(m_H);
        }
    }

    static bool IsValidHandle(handleValueType handle)
    {
        return handle != invalidHandleValue;
    }

    bool operator==(handleT h)
    {
        return m_H == h;
    }

    operator handleT()
    {
        return m_H;
    }

    bool IsValidHandle() const
    {
        return m_H != invalidHandleValue;
    }

    void Reset(handleT h)
    {
        if (m_H != invalidHandleValue)
        {
            deleteT<handleT> deletor;
            deletor(m_H);
        }

        m_H = h;
    }

    void Release()
    {
        if (m_H != invalidHandleValue)
        {
            deleteT<handleT> deletor;
            deletor(m_H);
            m_H = invalidHandleValue;
        }
    }

public:
    const RAIIHandleT& operator=(const RAIIHandleT& rhs) = delete;
    RAIIHandleT(const RAIIHandleT&) = delete;

    handleT m_H;
};

#ifdef __linux__

#include <unistd.h>

template <class handleT>
struct DeleteByClose
{
    void operator()(handleT h)
    {
        ::close(h);
        h = -1;
    }
};

typedef RAIIHandleT<int, DeleteByClose, -1> RAIIHandle;

#elif _WIN32
#include <Windows.h>

template <class handleT>
struct DeleteByCloseHandle
{
    void operator()(handleT h)
    {
        ::CloseHandle(h);
    }
};

typedef RAIIHandleT<HANDLE, DeleteByCloseHandle, INVALID_HANDLE_VALUE> FDRAII;
#endif

#endif //OCP_DIAG_SSD_OSV_RAIIHANDLE_H
