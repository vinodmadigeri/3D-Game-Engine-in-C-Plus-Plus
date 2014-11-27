#ifndef __MATERIAL_CONSTANTDATA_H
#define __MATERIAL_CONSTANTDATA_H
#include "PreCompiled.h"
#include <d3dx9shader.h>

namespace Engine
{
	struct BelongsToenum
	{
		enum BELONGSTO
		{
			FRAGMENT_SHADER = 1,
			VERTEX_SHADER,
			NONE
		};
	};

	struct IsAenum
	{
		enum IsA
		{
			FLOAT = 1,
			FLOAT_ARRAY,
			VECTOR_4D,
			MATRIX_4X4,
			NONE
		};
	};

	class IMaterialConstant
	{
	public:
		IMaterialConstant(){}
		virtual ~IMaterialConstant(){}
		virtual BelongsToenum::BELONGSTO GetBelongsTo() const = 0;
		virtual const std::string GetName() const = 0;
		virtual const IsAenum::IsA GetIsA() const = 0;
		virtual unsigned int GetCount() const = 0;
		virtual void SetValue(const void * i_defaultValues, unsigned int i_count) = 0;
		virtual const void* GetValue() const = 0;
		virtual const D3DXHANDLE GetHandle() const = 0;
	};

	template<class T>
	class MaterialConstantData :public IMaterialConstant
	{
	private:
		const D3DXHANDLE mHandle;
		const std::string mConstantName;
		T *mDefaultValues;
		const unsigned int mCount;
		const BelongsToenum::BELONGSTO eBelongsTo;
		const IsAenum::IsA eIsA;
	public:

		MaterialConstantData(const char * i_constantName,
			const T * i_defaultValues,
			unsigned int i_count,
			const D3DXHANDLE i_Handle,
			const BelongsToenum::BELONGSTO i_enbelongsto,
			const IsAenum::IsA i_enIsA);

		~MaterialConstantData();
		bool IsA(const char * i_name) const;
		BelongsToenum::BELONGSTO GetBelongsTo() const;
		const std::string GetName() const;
		const IsAenum::IsA GetIsA() const;
		unsigned int GetCount() const;
		void SetValue(const void * i_defaultValues, unsigned int i_count);
		const void* GetValue() const;
		const D3DXHANDLE GetHandle() const;
	};
}

namespace Engine
{
	template<class T>
	MaterialConstantData<T>::MaterialConstantData(const char * i_constantName,
		const T * i_defaultValues,
		unsigned int i_count,
		const D3DXHANDLE i_Handle,
		const BelongsToenum::BELONGSTO i_enbelongsto,
		const IsAenum::IsA i_enIsA) :
		mConstantName(i_constantName),
		mHandle(i_Handle),
		mCount(i_count),
		eBelongsTo(i_enbelongsto),
		eIsA(i_enIsA)
	{
		assert(i_constantName && i_defaultValues && (i_count > 0) && (i_Handle != NULL) && (i_enbelongsto != BelongsToenum::BELONGSTO::NONE));

		mDefaultValues = new T[mCount];

		//Copy every object using its copy constructor
		for (unsigned int i = 0; i < mCount; i++)
		{
			mDefaultValues[i] = i_defaultValues[i];
		}
	}

	template<class T>
	MaterialConstantData<T>::~MaterialConstantData()
	{
		if (mDefaultValues)
		{
			delete[] mDefaultValues;
		}
	}

	template<class T>
	BelongsToenum::BELONGSTO MaterialConstantData<T>::GetBelongsTo() const
	{
		return eBelongsTo;
	}

	template<class T>
	const std::string MaterialConstantData<T>::GetName() const
	{
		return mConstantName;
	}

	template<class T>
	const IsAenum::IsA MaterialConstantData<T>::GetIsA() const
	{
		return eIsA;
	}

	template<class T>
	unsigned int MaterialConstantData<T>::GetCount() const
	{
		return mCount;
	}

	template<class T>
	void MaterialConstantData<T>::SetValue(const void * i_defaultValues, unsigned int i_count)
	{
		assert((mCount == i_count) && mDefaultValues);

		//Copy every object using its copy constructor
		for (unsigned int i = 0; i < mCount; i++)
		{
			mDefaultValues[i] = (reinterpret_cast<const T *>(i_defaultValues))[i];
		}
	}

	template<class T>
	const void * MaterialConstantData<T>::GetValue() const
	{
		assert(mDefaultValues);

		return mDefaultValues;
	}

	template<class T>
	const D3DXHANDLE MaterialConstantData<T>::GetHandle() const
	{
		return mHandle;
	}
}
#endif //__MATERIAL_CONSTANTDATA_H