#pragma once
#include <utility>
#include <type_traits>

#include <context.h>

template<typename Fkn = context::detail::transfer_t (*)(context::detail::transfer_t)>
struct alignas(8) Stack
{
	Stack()
	: m_fkn(Fkn{})
	{}
	Stack(Fkn&& fkn)
	: m_fkn(std::forward<Fkn>(fkn))
	{}

	static const constexpr int stack_size = 500;

	// Return 8-aligned last position.
	uint32_t* emptySP()
	{
		return &(m_stack[stack_size - 2]);
	}
	size_t stackSize()
	{
		return sizeof m_stack - 8;
	}

	uint32_t m_stack[stack_size];
	Fkn m_fkn;
};

class fiber_context
{
	using Fkn = fiber_context (*)(fiber_context&&);
public:
	fiber_context() noexcept = default;
	fiber_context(const fiber_context&) = delete;
	fiber_context(fiber_context&& fc)
	: m_fkn(std::exchange(fc.m_fkn, nullptr))
	{	}

	fiber_context& operator=(const fiber_context&) = delete;
	fiber_context& operator=(fiber_context&& fc)
	{
		m_fkn = std::exchange(fc.m_fkn, nullptr);
		return *this;
	}

	void swap(fiber_context& a)
	{
		std::swap(a.m_fkn, m_fkn);
	}

	template<typename Fn>
	explicit fiber_context(Fn&& fn)
	{
		auto s = new Stack<Fn>(std::forward<Fn>(fn));
		auto ctx = context::detail::make_fcontext( s->emptySP(), s->stackSize(), &topFkn<Fn>);
		m_fkn = context::detail::jump_fcontext(ctx, (void*)s).fctx; // Will return directly.
	}

	bool empty() const noexcept
	{
		return !m_fkn;
	}

	explicit operator bool() noexcept
	{
		return !empty();
	}

	fiber_context resume() &&
	{
		return fiber_context(context::detail::jump_fcontext(std::exchange(m_fkn, nullptr), nullptr));
	}

	template<typename Fn>
	fiber_context resume_with(Fn&& fn) &&
	{
		using Fn_ = typename std::remove_reference<Fn>::type;
		auto adapterFkn = [](context::detail::transfer_t t) -> context::detail::transfer_t
		{
			auto res = (*static_cast<Fn_*>(t.data))(fiber_context{t.fctx});
			return context::detail::transfer_t{ res.m_fkn, nullptr };
		};
		Fn_ fn_{std::forward<Fn_>(fn)};
        return fiber_context(context::detail::ontop_fcontext(
            std::exchange(m_fkn, nullptr), static_cast<void*>(&fn_),
            adapterFkn));
    }

  private:
	fiber_context(context::detail::transfer_t t)
	: m_fkn(t.data ? nullptr : t.fctx)
	{
	};
	fiber_context(context::detail::fcontext_t fctx) : m_fkn(fctx)
	{};

	template<typename Fn>
	static void topFkn(context::detail::transfer_t t)
	{
		auto s = (Stack<Fn>*)t.data;
		auto t3 = context::detail::jump_fcontext(t.fctx, nullptr);
		// First external jump to us. Run top fkn.
		fiber_context fc = s->m_fkn(fiber_context{ t3.fctx });

		// Returned from our top function. This context is done.
		context::detail::ontop_fcontext(fc.m_fkn, s, cleanup<Fn>);
	}

	template<typename Fn>
	static context::detail::transfer_t cleanup(context::detail::transfer_t t)
	{
		delete (Stack<Fn>*)t.data;
		return t;
	}

	context::detail::fcontext_t m_fkn = nullptr;
};
