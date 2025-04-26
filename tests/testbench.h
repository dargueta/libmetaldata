#pragma once

struct MDTestCase_;
typedef struct MDTestCase_ MDTestCase;

struct MDTestRunner_;
typedef struct MDTestRunner_ MDTestRunner;

typedef int (*mdt_test_case_fn)(MDTestCase *test);

struct MDTestCase_
{
    const char *name;
    mdt_test_case_fn function;
    void *userdata;
    MDTestRunner *runner;
};
