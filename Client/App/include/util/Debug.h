#pragma once
#include <ostream>

namespace RBX
{
	class __declspec(novtable) Debugable
	{
	public:
		enum AssertAction
		{
			CrashOnAssert,
			IgnoreAssert
		};

	public:
		static AssertAction assertAction;
		static bool validatingDebug;

	public:
		virtual void dump(std::ostream& stream);

	/* public:
		Debugable(const Debugable&);
		Debugable();
		Debugable& operator=(const Debugable&); */

	public:
		static void forceBadTypeId();
		static void doCrash();
		static void dump(void*, std::ostream& stream);
	};
}

//#define RBXASSERT(expr) if ( RBX::Debugable::assertAction == Debugable::CrashOnAssert && !(expr)) RBX::Debugable::doCrash()
// copied from assert.h
//#define RBXASSERT(expr) (void)( ( RBX::Debugable::assertAction != RBX::Debugable::CrashOnAssert || !!(expr) ) || (RBX::Debugable::doCrash(), 0) )
#define SCOPED(expr) do \
	{ \
		expr; \
	} \
	while (0)
#if defined(_DEBUG) || defined(_RELEASEASSERT)
#define RBXASSERT(expr) SCOPED( (void)( ( RBX::Debugable::assertAction != RBX::Debugable::CrashOnAssert || !!(expr) ) || (RBX::Debugable::doCrash(), 0) ) )
#else
#define RBXASSERT(expr)
#endif

#define RBXCRASH() { int* badPtr1 = (int*)0; int* badPtr2 = (int*)4; *badPtr1 = *badPtr2; }

template <typename To, typename From>
To rbx_static_cast(From u)
{
	RBXASSERT(dynamic_cast<To>(u) == static_cast<To>(u));
	return static_cast<To>(u);
}
