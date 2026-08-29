/*
 * test_logger.c — Tests for logger edge cases
 *
 * Tests:
 *   - Empty format string
 *   - Very long messages (>2048 buffer)
 *   - Format specifiers in data
 *   - NULL file/func pointers
 *   - All log levels
 *   - Rapid concurrent logging
 *
 * The logger writes to stderr, so we verify it doesn't crash
 * rather than checking output content.
 */

#include "test_framework.h"
#include "csorma.h"
#include "logger.h"

#include <pthread.h>
#include <stdatomic.h>

/* ============================================================
 *  Basic logging tests (verify no crash)
 * ============================================================ */

static bool t_log_all_levels(void)
{
    /* Call each level directly — should not crash */
    csorma_logger_write(CSORMA_LOGGER_LEVEL_TRACE,   __FILE__, __LINE__, __func__, "trace msg");
    csorma_logger_write(CSORMA_LOGGER_LEVEL_DEBUG,   __FILE__, __LINE__, __func__, "debug msg");
    csorma_logger_write(CSORMA_LOGGER_LEVEL_INFO,    __FILE__, __LINE__, __func__, "info msg");
    csorma_logger_write(CSORMA_LOGGER_LEVEL_WARNING, __FILE__, __LINE__, __func__, "warning msg");
    csorma_logger_write(CSORMA_LOGGER_LEVEL_ERROR,   __FILE__, __LINE__, __func__, "error msg");
    return true;
}

static bool t_log_empty_format(void)
{
    csorma_logger_write(CSORMA_LOGGER_LEVEL_INFO, __FILE__, __LINE__, __func__, "");
    return true;
}

static bool t_log_no_args(void)
{
    csorma_logger_write(CSORMA_LOGGER_LEVEL_INFO, __FILE__, __LINE__, __func__, "no format args here");
    return true;
}

static bool t_log_with_int_args(void)
{
    csorma_logger_write(CSORMA_LOGGER_LEVEL_INFO, __FILE__, __LINE__, __func__,
                        "int=%d long=%lld hex=0x%x", 42, 999999999LL, 0xFF);
    return true;
}

static bool t_log_with_string_args(void)
{
    csorma_logger_write(CSORMA_LOGGER_LEVEL_INFO, __FILE__, __LINE__, __func__,
                        "str1=%s str2=%s", "hello", "world");
    return true;
}

/* ============================================================
 *  Edge cases
 * ============================================================ */

static bool t_log_very_long_message(void)
{
    /* Buffer is CSORMA_LOGGER_MAX_MSG_LEN (2048).
     * Test with a message that exceeds this. */
    char long_msg[4096];
    memset(long_msg, 'A', sizeof(long_msg) - 1);
    long_msg[sizeof(long_msg) - 1] = '\0';

    csorma_logger_write(CSORMA_LOGGER_LEVEL_INFO, __FILE__, __LINE__, __func__,
                        "%s", long_msg);
    /* Should truncate, not overflow */
    return true;
}

static bool t_log_exactly_buffer_size(void)
{
    /* Exactly 2047 chars + NUL = 2048 */
    char msg[2048];
    memset(msg, 'B', 2047);
    msg[2047] = '\0';

    csorma_logger_write(CSORMA_LOGGER_LEVEL_INFO, __FILE__, __LINE__, __func__,
                        "%s", msg);
    return true;
}

static bool t_log_one_over_buffer(void)
{
    /* 2048 chars + NUL = 2049 (one over) */
    char msg[2049];
    memset(msg, 'C', 2048);
    msg[2048] = '\0';

    csorma_logger_write(CSORMA_LOGGER_LEVEL_INFO, __FILE__, __LINE__, __func__,
                        "%s", msg);
    return true;
}

static bool t_log_format_specifiers_in_data(void)
{
    /* Data contains format specifiers — should NOT be interpreted
     * because they're passed as %s arguments, not as the format string */
    const char *dangerous = "%s%s%s%s%s%n%n%n%d%d%d";
    csorma_logger_write(CSORMA_LOGGER_LEVEL_INFO, __FILE__, __LINE__, __func__,
                        "data=%s", dangerous);
    return true;
}

static bool t_log_format_string_with_percent(void)
{
    /* Format string with literal percent */
    csorma_logger_write(CSORMA_LOGGER_LEVEL_INFO, __FILE__, __LINE__, __func__,
                        "100%% complete");
    return true;
}

static bool t_log_null_file(void)
{
    /* NULL file pointer — tests strrchr(NULL, '/') handling */
    csorma_logger_write(CSORMA_LOGGER_LEVEL_INFO, NULL, __LINE__, __func__, "null file");
    return true;
}

static bool t_log_null_func(void)
{
    csorma_logger_write(CSORMA_LOGGER_LEVEL_INFO, __FILE__, __LINE__, NULL, "null func");
    return true;
}

static bool t_log_null_file_and_func(void)
{
    csorma_logger_write(CSORMA_LOGGER_LEVEL_INFO, NULL, 0, NULL, "both null");
    return true;
}

static bool t_log_special_chars(void)
{
    csorma_logger_write(CSORMA_LOGGER_LEVEL_INFO, __FILE__, __LINE__, __func__,
                        "special: \t\n\r\0\xff\x01 end");
    return true;
}

static bool t_log_unicode(void)
{
    csorma_logger_write(CSORMA_LOGGER_LEVEL_INFO, __FILE__, __LINE__, __func__,
                        "unicode: \xc3\xa9\xc3\xa8\xc3\xaa \xe4\xb8\xad\xe6\x96\x87");
    return true;
}

/* ============================================================
 *  Macro-level tests
 * ============================================================ */

static bool t_log_macros(void)
{
    CSORMA_LOGGER_TRACE("trace via macro");
    CSORMA_LOGGER_DEBUG("debug via macro");
    CSORMA_LOGGER_INFO("info via macro");
    CSORMA_LOGGER_WARNING("warning via macro");
    CSORMA_LOGGER_ERROR("error via macro");
    CSORMA_LOGGER_ALWAYS("always via macro");
    return true;
}

static bool t_log_macro_with_args(void)
{
    CSORMA_LOGGER_INFO("value=%d str=%s ptr=%p", 42, "test", (void*)0x1234);
    return true;
}

/* ============================================================
 *  Concurrent logging stress
 * ============================================================ */
static atomic_int g_log_count = 0;

static void *log_worker(void *arg)
{
    int tid = (int)(intptr_t)arg;
    for (int i = 0; i < 100; i++)
    {
        csorma_logger_write(CSORMA_LOGGER_LEVEL_INFO, __FILE__, __LINE__, __func__,
                            "thread %d iteration %d", tid, i);
        atomic_fetch_add(&g_log_count, 1);
    }
    return NULL;
}

static bool t_log_concurrent(void)
{
    atomic_store(&g_log_count, 0);

    pthread_t threads[4];
    for (int i = 0; i < 4; i++)
        pthread_create(&threads[i], NULL, log_worker, (void *)(intptr_t)i);
    for (int i = 0; i < 4; i++)
        pthread_join(threads[i], NULL);

    int total = atomic_load(&g_log_count);
    T_ASSERT_INT_EQ(total, 400, "all log calls completed");
    return true;
}

/* ============================================================
 *  Rapid sequential logging
 * ============================================================ */

static bool t_log_rapid(void)
{
    for (int i = 0; i < 1000; i++)
    {
        csorma_logger_write(CSORMA_LOGGER_LEVEL_DEBUG, __FILE__, __LINE__, __func__,
                            "rapid %d", i);
    }
    return true;
}

/* ============================================================
 *  MAIN
 * ============================================================ */
int main(void)
{
    printf(C_BOLD "\n");
    printf("  ================================================================\n");
    printf("   CSORMA LOGGER EDGE CASE TESTS\n");
    printf("   Source: ../template/logger.c\n");
    printf("   NOTE: Logger output goes to stderr (expected)\n");
    printf("  ================================================================\n");
    printf(C_RESET "\n");

    /* Redirect stderr to /dev/null during tests to reduce noise */
    /* (uncomment if you want quiet output) */
    /* freopen("/dev/null", "w", stderr); */

    TEST_SUITE("Basic logging");
    RUN_TEST(t_log_all_levels);
    RUN_TEST(t_log_empty_format);
    RUN_TEST(t_log_no_args);
    RUN_TEST(t_log_with_int_args);
    RUN_TEST(t_log_with_string_args);
    SUITE_END();

    TEST_SUITE("Buffer overflow edge cases");
    RUN_TEST(t_log_very_long_message);
    RUN_TEST(t_log_exactly_buffer_size);
    RUN_TEST(t_log_one_over_buffer);
    SUITE_END();

    TEST_SUITE("Format string safety");
    RUN_TEST(t_log_format_specifiers_in_data);
    RUN_TEST(t_log_format_string_with_percent);
    SUITE_END();

    TEST_SUITE("NULL pointer handling");
    RUN_TEST(t_log_null_file);
    RUN_TEST(t_log_null_func);
    RUN_TEST(t_log_null_file_and_func);
    SUITE_END();

    TEST_SUITE("Special content");
    RUN_TEST(t_log_special_chars);
    RUN_TEST(t_log_unicode);
    SUITE_END();

    TEST_SUITE("Macro interface");
    RUN_TEST(t_log_macros);
    RUN_TEST(t_log_macro_with_args);
    SUITE_END();

    TEST_SUITE("Stress");
    RUN_TEST(t_log_concurrent);
    RUN_TEST(t_log_rapid);
    SUITE_END();

    return test_summary("Logger");
}

