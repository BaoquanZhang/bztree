#include <glog/logging.h>
#include <gtest/gtest.h>

#include "bztree.h"

TEST(LeafNode, Insert) {
  bztree::LeafNode *node = (bztree::LeafNode *) malloc(bztree::LeafNode::kNodeSize);
  memset(node, 0, bztree::LeafNode::kNodeSize);
  new(node) bztree::LeafNode;
  node->Dump();

  pmwcas::InitLibrary(pmwcas::TlsAllocator::Create,
                      pmwcas::TlsAllocator::Destroy,
                      pmwcas::LinuxEnvironment::Create,
                      pmwcas::LinuxEnvironment::Destroy);

  pmwcas::DescriptorPool *pool =
      (pmwcas::DescriptorPool *) pmwcas::Allocator::Get()->Allocate(sizeof(pmwcas::DescriptorPool));
  new(pool) pmwcas::DescriptorPool(1000, 1, nullptr, false);

  pool->GetEpoch()->Protect();

  ASSERT_TRUE(node->Insert(0, "def", 3, 100, pool));
  ASSERT_TRUE(node->Insert(0, "bdef", 4, 100, pool));
  ASSERT_TRUE(node->Insert(0, "abc", 3, 100, pool));

  node->Dump();

  auto *new_node = node->Consolidate(pool);
  new_node->Dump();

  pool->GetEpoch()->Unprotect();
}

TEST(LeafNode, duplicate_insert) {
  auto *node = (bztree::LeafNode *) malloc(bztree::LeafNode::kNodeSize);
  memset((void *) node, 0, bztree::LeafNode::kNodeSize);
  new(node) bztree::LeafNode();
  node->Dump();

  pmwcas::InitLibrary(pmwcas::TlsAllocator::Create,
                      pmwcas::TlsAllocator::Destroy,
                      pmwcas::LinuxEnvironment::Create,
                      pmwcas::LinuxEnvironment::Destroy);
  auto *pool = (pmwcas::DescriptorPool *) pmwcas::Allocator::Get()->Allocate(sizeof(pmwcas::DescriptorPool));
  new(pool) pmwcas::DescriptorPool(1000, 1, nullptr, false);

  pool->GetEpoch()->Protect();

  ASSERT_TRUE(node->Insert(0, (char *) "abc", 2, 100, pool));
  ASSERT_TRUE(node->Insert(0, (char *) "bdef", 4, 100, pool));
  ASSERT_TRUE(node->Insert(0, (char *) "abc", 3, 100, pool));

  node->Dump();

  auto *new_node = node->Consolidate(pool);
  new_node->Dump();

  pool->GetEpoch()->Unprotect();
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
