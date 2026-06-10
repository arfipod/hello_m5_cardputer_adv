#include <unity.h>

#include "common/result.hpp"
#include "common/ring_buffer.hpp"

void setUp() {}

void tearDown() {}

void ring_buffer_preserves_order() {
    common::RingBuffer<int, 3> buffer;
    TEST_ASSERT_TRUE(buffer.push(10));
    TEST_ASSERT_TRUE(buffer.push(20));

    int value = 0;
    TEST_ASSERT_TRUE(buffer.pop(value));
    TEST_ASSERT_EQUAL(10, value);
    TEST_ASSERT_TRUE(buffer.pop(value));
    TEST_ASSERT_EQUAL(20, value);
    TEST_ASSERT_TRUE(buffer.empty());
}

void ring_buffer_rejects_overflow() {
    common::RingBuffer<int, 2> buffer;
    TEST_ASSERT_TRUE(buffer.push(1));
    TEST_ASSERT_TRUE(buffer.push(2));
    TEST_ASSERT_FALSE(buffer.push(3));
    TEST_ASSERT_TRUE(buffer.full());
}

void result_reports_success_and_error() {
    common::Result<int> ok = common::Result<int>::ok(42);
    TEST_ASSERT_TRUE(ok);
    TEST_ASSERT_EQUAL(42, ok.value());

    common::Result<int> err = common::Result<int>::err(common::Error::NotReady);
    TEST_ASSERT_FALSE(err);
    TEST_ASSERT_EQUAL(static_cast<int>(common::Error::NotReady), static_cast<int>(err.error()));
}

int main(int, char**) {
    UNITY_BEGIN();
    RUN_TEST(ring_buffer_preserves_order);
    RUN_TEST(ring_buffer_rejects_overflow);
    RUN_TEST(result_reports_success_and_error);
    return UNITY_END();
}
