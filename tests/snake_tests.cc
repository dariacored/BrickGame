#include "test_includes.h"

// =============================================================================
// Main Test Runner
// =============================================================================

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}