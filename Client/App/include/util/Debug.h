#pragma once
#include <ostream>

namespace RBX {
	class __declspec(novtable) Debugable {
		public:
			virtual void dump(std::ostream& stream);
			static void doCrash();
			enum AssertAction
			{
			  CrashOnAssert = 0x0,
			  IgnoreAssert = 0x1,
			};
			static AssertAction assertAction;
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

template <typename To, typename From>
To rbx_static_cast(From u)
{
	RBXASSERT(dynamic_cast<To>(u) == static_cast<To>(u));
	return static_cast<To>(u);
}
