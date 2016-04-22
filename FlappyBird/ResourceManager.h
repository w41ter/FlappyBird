#pragma once

namespace games
{
	using std::wstring;
	using std::vector;
	using std::map;

	typedef int resource;

	template<typename resID>
	class Resource
	{
	public:
		Resource(resID id) : startID(id) {}
		virtual ~Resource() {};

		resID start() { return startID; }

	protected:
		resID curID;
		const resID startID;
	};

	template<typename resID, typename Type>
	class ResourceLibrary : public Resource<resID>
	{
	public:
		ResourceLibrary() : Resource<resID>(1000000) {}
		ResourceLibrary(resID id) : Resource<resID>(id) {}
		virtual ~ResourceLibrary() override {};

		// add delete find change
		resID insert(Type type, resID id = -1);
		bool remove(resID id);
		bool change(resID id, Type str);
		bool find(resID id);
		bool load(resID id, Type &str);
		void clear() { resourceList.clear(); }

		Type &operator[](resID id);

		typename map<resID, Type>::iterator begin() { return resourceList.begin(); }
		typename map<resID, Type>::iterator end() { return resourceList.end(); }

	protected:
		map<resID, Type> resourceList;
	};

	template<typename resID, typename Type>
	resID ResourceLibrary<resID, Type>::insert(Type type, resID id)
	{
		if (id == -1)
		{
			resourceList[curID] = std::move(type);
			return curID++;
		}
		else
		{
			resourceList[id] = std::move(type);
			return id;
		}
	}

	template<typename resID, typename Type>
	bool ResourceLibrary<resID, Type>::remove(resID id)
	{
		auto iterator = resourceList.find(id);
		if (iterator == resourceList.end())
			return false;

		resourceList.erase(iterator);
		return true;
	}

	template<typename resID, typename Type>
	bool ResourceLibrary<resID, Type>::find(resID id)
	{
		if (resourceList.find(id) != resourceList.end())
			return true;
		else
			return false;
	}

	template<typename resID, typename Type>
	bool ResourceLibrary<resID, Type>::change(resID id, Type type)
	{
		auto iterator = resourceList.find(id);
		if (iterator == resourceList.end())
			return false;

		iterator->second = std::move(type);
		return true;
	}

	template<typename resID, typename Type>
	bool ResourceLibrary<resID, Type>::load(resID id, Type &type)
	{
		auto iterator = resourceList.find(id);
		if (iterator == resourceList.end())
			return false;

		type = iterator->second;
		return true;
	}

	// 
	// if resource id not exists, will insert new type and return it.
	//
	template<typename resID, typename Type>
	Type &ResourceLibrary<resID, Type>::operator[](resID id)
	{
		auto iterator = resourceList.find(id);
		if (iterator == resourceList.end())
			return resourceList[id] = std::move(Type());//GetErrorHandle().deal(ProgramError::NO_REQUIST_RESOURCE);

		return iterator->second;
	}

	ResourceLibrary<resource, wstring> &wstringResource();
}

