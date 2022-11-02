#include "hzpch.h"
#include "Reference.h"

namespace Excimer
{
	RefCount::RefCount()
	{
		m_Refcount.Init();
		m_RefcountInit.Init();
		m_WeakRefcount.Init(0);
	}

	RefCount::~RefCount()
	{
	}

	bool RefCount::InitRef()
	{
		if(reference())
		{
			if (m_RefcountInit.Get()>0)
			{
				m_RefcountInit.Unref();
				unreference();
			}

            return true;
		}
		else
		{
			return false;
		}
	}

    int RefCount::GetReferenceCount() const
    {
        return m_Refcount.Get();
    }

    int RefCount::GetWeakReferenceCount() const
    {
        return m_WeakRefcount.Get();
    }

    bool RefCount::reference()
    {
        bool success = m_Refcount.SharedPtr();

        return success;
    }

    bool RefCount::unreference()
    {
        bool die = m_Refcount.Unref();

        return die;
    }

    bool RefCount::weakReference()
    {
        bool success = m_WeakRefcount.SharedPtr();

        return success;
    }

    bool RefCount::weakUnreference()
    {
        bool die = m_WeakRefcount.Unref() && m_Refcount.count == 0;

        return die;
    }
}