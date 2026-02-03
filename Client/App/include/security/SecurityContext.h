#include <boost/thread/tss.hpp>
#include <G3D/format.h>

namespace RBX
{
	namespace Security
	{
		enum Identities
		{
			Anonymous,
			LocalGUI,
			GameScript,
			CmdLine,
			TrustedCOM,
			TrustedWebService,
			Replicator
		};

		enum Permissions
		{
			None,
			Administrator
		};

		class Context
		{
			friend class Impersonator;

		private:
			const Identities identity;
		public:
			void requirePermission(Permissions permission, const char* operation) const
			{
				if (!isInRole(identity, permission))
				{
					if (operation)
						throw std::runtime_error(G3D::format("The current security context cannot %s", operation));
					else
						throw std::runtime_error(G3D::format("The current security context cannot perform the requested operation"));
				}
			}

			bool hasPermission(Permissions);
		private:
			Context(Identities identity)
				: identity(identity)
			{
			}

		public:
			static Context& current()
			{
				Context* context = ptr().get();
				if (!context)
				{
					context = new Context(Anonymous);
					ptr().reset(context);
				}

				return *context;
			}
			static __declspec(noinline) bool isInRole(Identities identity, Permissions permission);
		private:
			static boost::thread_specific_ptr<Context>& ptr()
			{
				static boost::thread_specific_ptr<Context> value;
				return value;
			}
		};

		class Impersonator
		{
		private:
			Context* previous;
		public:
			Impersonator(Identities identity)
			{
				Context* newContext = new Context(identity);
				previous = Context::ptr().release();
				Context::ptr().reset(newContext);
			}

			~Impersonator()
			{
				Context::ptr().reset(previous);
			}
		};
	}
}
