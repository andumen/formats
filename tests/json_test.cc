#include "json_test.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

FormatsTest* FormatsTest::last_ = NULL;

// The windows compiler doesn't like to use `strdup`, and claims it's a
// deprecated name.
// For simplicity just implement it ourselves.
static char* Strdup(const char* str)
{
  size_t len    = strlen(str);
  char*  result = reinterpret_cast<char*>(malloc(len + 1));
  memcpy(result, str, len + 1);
  return result;
}

FormatsTest::FormatsTest(TestFunction* callback,
                         const char*   test_file,
                         const char*   test_name,
                         const char*   test_dependency,
                         bool          test_is_enabled)
    : callback_(callback)
    , name_(test_name)
    , dependency_(test_dependency)
    , prev_(last_)
{
  // Find the base name of this test (const_cast required on Windows).
  char* basename = strrchr(const_cast<char*>(test_file), '/');
  if (!basename) { basename = strrchr(const_cast<char*>(test_file), '\\'); }
  if (!basename) { basename = Strdup(test_file); }
  else { basename = Strdup(basename + 1); }
  // Drop the extension, if there is one.
  char* extension = strrchr(basename, '.');
  if (extension) *extension = 0;
  // Install this test in the list of tests
  file_    = basename;
  enabled_ = test_is_enabled;
  prev_    = last_;
  last_    = this;
}

static void PrintTestList(FormatsTest* current)
{
  if (current == NULL) return;
  PrintTestList(current->prev());
  if (current->dependency() != NULL)
  {
    printf("%s/%s<%s\n", current->file(), current->name(), current->dependency());
  }
  else { printf("%s/%s<\n", current->file(), current->name()); }
}

int main(int argc, char* argv[])
{
  build_custom_data_once();

  int  tests_run       = 0;
  bool print_run_count = true;
  if (argc == 1)
  {
    // Just run all the tests.
    FormatsTest* test = FormatsTest::last();
    while (test != NULL)
    {
      if (test->enabled())
      {
        test->Run();
        tests_run++;
      }
      test = test->prev();
    }
  }
  for (int i = 1; i < argc; i++)
  {
    char* arg = argv[i];
    if (strcmp(arg, "--list") == 0)
    {
      PrintTestList(FormatsTest::last());
      print_run_count = false;
    }
    else
    {
      char* arg_copy = Strdup(arg);
      char* testname = strchr(arg_copy, '/');
      if (testname)
      {
        // Split the string in two by nulling the slash and then run
        // exact matches.
        *testname         = 0;
        char*        file = arg_copy;
        char*        name = testname + 1;
        FormatsTest* test = FormatsTest::last();
        while (test != NULL)
        {
          if (test->enabled() && strcmp(test->file(), file) == 0 && strcmp(test->name(), name) == 0)
          {
            test->Run();
            tests_run++;
          }
          test = test->prev();
        }
      }
      else
      {
        // Run all tests with the specified file or test name.
        char*        file_or_name = arg_copy;
        FormatsTest* test         = FormatsTest::last();
        while (test != NULL)
        {
          if (test->enabled() &&
              (strcmp(test->file(), file_or_name) == 0 || strcmp(test->name(), file_or_name) == 0))
          {
            test->Run();
            tests_run++;
          }
          test = test->prev();
        }
      }
      free(arg_copy);
    }
  }
  if (print_run_count && tests_run != 1) printf("Ran %i tests.\n", tests_run);
  return 0;
}
