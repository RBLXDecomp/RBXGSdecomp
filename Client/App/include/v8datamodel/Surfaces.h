#pragma once
#include <boost/noncopyable.hpp>
#include "v8datamodel/Surface.h"
#include "util/Vector6.h"

namespace RBX
{
	class PartInstance;

	class Surfaces : public boost::noncopyable
	{
	private:
		Surface Top;
		Surface Bottom;
		Surface Left;
		Surface Right;
		Surface Front;
		Surface Back;
	  
	public:
		Surfaces(PartInstance*);
	public:
		const Surface& operator[](NormalId) const;
		Surface& operator[](NormalId);
		Vector6<SurfaceType> surf6() const;
		const Reflection::PropertyDescriptor& getSurfaceType(NormalId) const;
		const Reflection::PropertyDescriptor& getSurfaceInput(NormalId) const;
		const Reflection::PropertyDescriptor& getParamA(NormalId) const;
		const Reflection::PropertyDescriptor& getParamB(NormalId) const;
		const bool isStandardPart() const;
	  
	public:
		static bool isSurfaceDescriptor(const Reflection::PropertyDescriptor&);
	};

	template<NormalId id>
	class SurfaceDescriptor : public Reflection::PropertyDescriptor
	{
	public:
		SurfaceDescriptor(const char*);

		virtual bool equalValues(const Reflection::DescribedBase*, const Reflection::DescribedBase*) const;
		virtual bool isReadOnly() const;
		virtual void readValue(Reflection::DescribedBase*, const XmlElement*, IReferenceBinder&) const;
		virtual void writeValue(const Reflection::DescribedBase*, XmlElement*) const;
		virtual bool hasStringValue() const;
		virtual std::string getStringValue(const Reflection::DescribedBase*) const;
		virtual bool setStringValue(Reflection::DescribedBase*, const std::string&) const;
	};

	template<NormalId id, typename Type>
	class SurfacePropDescriptor : public Reflection::TypedPropertyDescriptor<Type>
	{
	};

	template<NormalId id, typename Enum>
	class SurfaceEnumPropDescriptor : public Reflection::EnumPropertyDescriptor
	{
	private:
		std::auto_ptr<typename Reflection::TypedPropertyDescriptor<Enum>::GetSet> getset;

	public:
		Enum getValue(const Reflection::DescribedBase*) const;
		void setValue(Reflection::DescribedBase*, const Enum&) const;
		virtual bool equalValues(const Reflection::DescribedBase*, const Reflection::DescribedBase*) const;
		virtual int getEnumValue(const Reflection::DescribedBase*) const;
		virtual bool setEnumValue(Reflection::DescribedBase*, int) const;
		virtual unsigned getIndexValue(const Reflection::DescribedBase*) const;
		virtual bool setIndexValue(Reflection::DescribedBase*, unsigned) const;
		virtual bool hasStringValue() const;
		virtual std::string getStringValue(const Reflection::DescribedBase*) const;
		virtual bool setStringValue(const Reflection::DescribedBase*, const Name&) const;
		virtual bool setStringValue(const Reflection::DescribedBase*, const std::string&) const;
		virtual void readValue(Reflection::DescribedBase*, const XmlElement*, IReferenceBinder&) const;
		virtual void writeValue(const Reflection::DescribedBase*, XmlElement*) const;
	};

	template<NormalId id, typename Type, typename GetFunction, typename SetFunction>
	class SurfaceGetSet : public Reflection::TypedPropertyDescriptor<Type>::GetSet
	{
	private:
		GetFunction get;
		SetFunction set;

	public:
		SurfaceGetSet(GetFunction, SetFunction);
		virtual bool isReadOnly() const;
		virtual Type getValue(const Reflection::DescribedBase*) const;
		virtual void setValue(Reflection::DescribedBase*, const Type&) const;
	};
}
