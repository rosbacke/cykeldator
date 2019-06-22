#include <gtest/gtest.h>

#include <context.h>
#include <fiber_context.h>

#include <utility>
#include <fmt/format.h>

using namespace context::detail;

static void entryFkn(transfer_t t)
{}

TEST(basic_context, test1)
{
  static char stack[500];
  fcontext_t ctx = make_fcontext(stack + sizeof stack - 4, sizeof stack, entryFkn);

  EXPECT_GE((intptr_t)ctx, (intptr_t)stack);
  EXPECT_LT((intptr_t)ctx, (intptr_t)(stack + sizeof stack));
}

TEST(fiber_context, Stack_object_is_8_byte_aligned)
{
	Stack<> s;
	EXPECT_EQ((intptr_t)&s, ((intptr_t)&s & ~7));
}

TEST(fiber_context, Stack_object_top_of_stack)
{
	Stack<> s;
	EXPECT_EQ(s.emptySP(), &s.m_stack[498]);
	EXPECT_EQ((intptr_t)s.emptySP() & 7, 0);
}

TEST(fiber_context, Stack_object_stack_size)
{
	Stack<> s;
	EXPECT_EQ(s.stackSize(), 498 * sizeof (std::uint32_t));
	EXPECT_EQ(s.stackSize() & 7, 0u);
}

TEST(fiber_context, make_fcontext_can_use_stack_object)
{
	Stack<> s;
	fcontext_t fctx = make_fcontext( s.emptySP(), s.stackSize(), nullptr);
	EXPECT_LT(fctx, (void*)(&s + 1));
	EXPECT_GE(fctx, (void*)&s);
}

int dummy = 0;

TEST(fiber_context, can_jump_to_created_stack)
{
	auto fkn = [](transfer_t t)
	{
		int * ip = (int*)t.data;
		EXPECT_EQ(ip, &dummy);
		(*ip)++;
		jump_fcontext(t.fctx, t.data);
	};

	Stack<> s;
	fcontext_t fctx = make_fcontext( s.emptySP(), s.stackSize(), fkn);

	transfer_t t1 = jump_fcontext(fctx, (void*)&dummy);
	EXPECT_EQ(dummy, 1);
	EXPECT_LT(t1.fctx, (void*)(&s + 1));
	EXPECT_GE(t1.fctx, (void*)&s);
	EXPECT_EQ(t1.data, (void*)&dummy);

	fmt::print("{}\n", (char*)fctx - (char*)&s);
}

TEST(fiber_context, construction)
{
	fiber_context fc;
	EXPECT_TRUE(fc.empty());
	EXPECT_FALSE(bool(fc));
}

static int testData = 0;

static fiber_context testFkn(fiber_context&& fc)
{
	testData++;
	return fiber_context{};
}

TEST(fiber_context, construction2)
{
	testData = 0;
	fiber_context fc(testFkn);
	EXPECT_FALSE(fc.empty());
	EXPECT_TRUE(bool(fc));
	EXPECT_EQ(testData, 0);
}

static fiber_context testFkn2(fiber_context&& fc)
{
	return std::move(fc).resume();
}

TEST(fiber_context, resume_invalidates_prec_context)
{
	fiber_context fc(testFkn2);
	EXPECT_TRUE(bool(fc));

	fiber_context fc2{ 	std::move(fc).resume() };
	EXPECT_FALSE(bool(fc));
}

TEST(fiber_context, swap)
{
	fiber_context fc(testFkn2);
	EXPECT_TRUE(bool(fc));
	fiber_context fc2;
	EXPECT_FALSE(bool(fc2));

	fc.swap(fc2);
	EXPECT_FALSE(bool(fc));
	EXPECT_TRUE(bool(fc2));
}

TEST(fiber_context, swap2)
{
	fiber_context fc(testFkn2);
	EXPECT_TRUE(bool(fc));
	fiber_context fc2;
	EXPECT_FALSE(bool(fc2));

	std::swap(fc, fc2);
	EXPECT_FALSE(bool(fc));
	EXPECT_TRUE(bool(fc2));
}

TEST(fiber_context, resume_return_a_valid_context)
{
	fiber_context fc(testFkn2);
	EXPECT_TRUE(bool(fc));
	fiber_context fc2;
	fc2 = std::move(fc).resume();
	EXPECT_FALSE(bool(fc));
	EXPECT_TRUE(bool(fc2));
}

TEST(fiber_context, construct_with_functor)
{
	auto t = 0;
    auto fkn = [&](fiber_context&& fc) -> fiber_context {
        t++;
        return std::move(fc).resume();
    };
    fiber_context fc(fkn);
}

TEST(fiber_context, construct_with_functor2)
{
	auto t = 0;
    auto fkn = [&](fiber_context&& fc) -> fiber_context {
        t++;
        return std::move(fc).resume();
    };
    fiber_context fc(fkn);

    EXPECT_TRUE(bool(fc));
	fiber_context fc2;
	fc2 = std::move(fc).resume();
	EXPECT_FALSE(bool(fc));
	EXPECT_TRUE(bool(fc2));
	EXPECT_EQ(t, 1);
}

TEST(fiber_context, repeated_resume_works)
{
	int t = 0;
	int* ip = nullptr;

    auto fkn = [&](fiber_context&& fc) -> fiber_context {
        t++;
        fc = std::move(fc).resume();
        ip = &t;
        return std::move(fc).resume();
    };
    fiber_context fc(fkn);
	EXPECT_EQ(t, 0);
	EXPECT_EQ(ip, nullptr);
	fc = std::move(fc).resume();
	EXPECT_EQ(t, 1);
	EXPECT_EQ(ip, nullptr);
	fc = std::move(fc).resume();
	EXPECT_EQ(t, 1);
	EXPECT_EQ(ip, &t);
}

TEST(fiber_context, can_call_ontop)
{
	int t = 0;

    auto fkn = [&](fiber_context&& fc) -> fiber_context {
    	t++;
    	EXPECT_EQ(t, 1);
        fc = std::move(fc).resume();
    	EXPECT_EQ(t, 2);
    	t++;
        fc = std::move(fc).resume();
    	t=0;
        return std::move(fc).resume();
    };
    auto ontop = [&](fiber_context&& fc) -> fiber_context
    {
    	EXPECT_EQ(t, 1);
    	t++;
    	return std::move(fc);
    };

    fiber_context fc(fkn);
	EXPECT_EQ(t, 0);
	fc = std::move(fc).resume();
	EXPECT_EQ(t, 1);
	fc = std::move(fc).resume_with(ontop);
	EXPECT_EQ(t, 3);
	fc = std::move(fc).resume();
	EXPECT_EQ(t, 0);
}

static int s_t = 0;

auto s_fkn(fiber_context&& fc) -> fiber_context
{
	s_t++;
	EXPECT_EQ(s_t, 1);
    fc = std::move(fc).resume();
	EXPECT_EQ(s_t, 2);
	s_t++;
    fc = std::move(fc).resume();
    s_t=0;
    return std::move(fc).resume();
};

auto s_ontop(fiber_context&& fc) -> fiber_context
{
	EXPECT_EQ(s_t, 1);
	s_t++;
	return std::move(fc);
};

TEST(fiber_context, can_call_ontop_freeFkn)
{
    fiber_context fc(s_fkn);
	EXPECT_EQ(s_t, 0);
	fc = std::move(fc).resume();
	EXPECT_EQ(s_t, 1);
	fc = std::move(fc).resume_with(&s_ontop);
	EXPECT_EQ(s_t, 3);
	fc = std::move(fc).resume();
	EXPECT_EQ(s_t, 0);
}
