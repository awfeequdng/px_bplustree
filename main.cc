#include "bplustree.hh"
#include <iostream>
#include <cstdlib>
#include <unordered_map>
#include <map>

#define EXPECT_EQ(a, b) PXBPT_ASSERT((a) == (b), "")
#define EXPECT_NE(a, b) PXBPT_ASSERT((a) == (b), "")

class TestValue {
public:
    char val[50];
    bool operator==(const TestValue &tv) const {
        return strncmp(val, tv.val, 50);
    }
};

unsigned int globalseed = 9;

void BasicNodeInitPushBackTest() {
//    auto bplustree = new BPlusTree<int, TestValue>();
    BPlusTree<int, TestValue>::KeyNodePointerPair p1;
    BPlusTree<int, TestValue>::KeyNodePointerPair p2;

    int size = 10;
    int depth = 0;
    int item_count = 10;
    auto node = BPlusTree<int, TestValue>::ElasticNode<BPlusTree<int, TestValue>::KeyNodePointerPair>::Get(
            size, BPlusTree<int, TestValue>::NodeType::LeafType, depth, item_count, p1, p2
    );

    std::vector<BPlusTree<int, TestValue>::KeyNodePointerPair> values;
    for (int key = 0; key < item_count; key++) {
        BPlusTree<int, TestValue>::KeyNodePointerPair p1;
        p1.first = key;
        values.push_back(p1);
        node->PushBack(p1);
        EXPECT_EQ(node->GetSize(), key + 1);
    }

    using ElementType = BPlusTree<int, TestValue>::KeyNodePointerPair;

    unsigned key = 0;
    for (ElementType *element_p = node->Begin(); element_p != node->End(); element_p++) {
        EXPECT_EQ(element_p->first, key);
        key++;
    }
    EXPECT_EQ(
            reinterpret_cast<char *>(node) + sizeof(BPlusTree<int, TestValue>::ElasticNode<BPlusTree<int, TestValue>>),
            reinterpret_cast<char *>(node->Begin())
    );
    EXPECT_EQ(&(node->GetLowKeyPair()), node->GetElasticLowKeyPair());
    EXPECT_EQ(&(node->GetHighKeyPair()), node->GetElasticHighKeyPair());
    EXPECT_EQ(node->GetLowKeyPair().first, p1.first);
    EXPECT_EQ(node->GetHighKeyPair().first, p2.first);
    EXPECT_NE(&p1, &(node->GetLowKeyPair()));
    EXPECT_NE(&p2, &(node->GetHighKeyPair()));
}

/**
 * Works similar to the push back test above, but uses the Insert function instead of
 * directly using PushBack on the node.
 */
void InsertElementInNodeTest() {
    auto bplustree = new BPlusTree<int, TestValue>;
    BPlusTree<int, TestValue>::KeyNodePointerPair p1;
    BPlusTree<int, TestValue>::KeyNodePointerPair p2;

    // Get inner Node
    int size = 10;
    int depth = 0;
    int item_count = 10;  // Usually equal to size
    auto node = BPlusTree<int, TestValue>::ElasticNode<BPlusTree<int, TestValue>::KeyNodePointerPair>::Get(
            size, BPlusTree<int, TestValue>::NodeType::LeafType, depth, item_count, p1, p2);

    for (int key = 0; key < item_count; key++) {
        BPlusTree<int, TestValue>::KeyNodePointerPair p1;
        p1.first = key;
        EXPECT_EQ(node->InsertElementIfPossible(p1, node->Begin()), true);
        EXPECT_EQ(node->GetSize(), key + 1);
    }

    using ElementType = BPlusTree<int, TestValue>::KeyNodePointerPair;

    unsigned key = 9;
    for (ElementType *element_p = node->Begin(); element_p != node->End(); element_p++) {
        EXPECT_EQ(element_p->first, key);
        key--;
    }

    // To Check if we are inserting at the correct place
    EXPECT_EQ(
            reinterpret_cast<char *>(node) + sizeof(BPlusTree<int, TestValue>::ElasticNode<BPlusTree<int, TestValue>>),
            reinterpret_cast<char *>(node->Begin()));

    EXPECT_EQ(&(node->GetLowKeyPair()), node->GetElasticLowKeyPair());
    EXPECT_EQ(&(node->GetHighKeyPair()), node->GetElasticHighKeyPair());
    EXPECT_EQ(node->GetLowKeyPair().first, p1.first);
    EXPECT_EQ(node->GetHighKeyPair().first, p2.first);
    EXPECT_NE(&p1, &(node->GetLowKeyPair()));
    EXPECT_NE(&p2, &(node->GetHighKeyPair()));

    // Free the node - should not result in an ASAN
    node->FreeElasticNode();
    delete bplustree;
}

/**
 * Randomly insert elements in a node.
 */
void InsertElementInNodeRandomTest() {
    auto bplustree = new BPlusTree<int, TestValue>;
    BPlusTree<int, TestValue>::KeyNodePointerPair p1;
    BPlusTree<int, TestValue>::KeyNodePointerPair p2;

    // Get inner Node
    int size = 10;
    int depth = 0;
    int item_count = 10;  // Usually equal to size
    auto node = BPlusTree<int, TestValue>::ElasticNode<BPlusTree<int, TestValue>::KeyNodePointerPair>::Get(
            size, BPlusTree<int, TestValue>::NodeType::LeafType, depth, item_count, p1, p2);

    std::map<int, int> positions;
    for (int key = 0; key < item_count; key++) {
        BPlusTree<int, TestValue>::KeyNodePointerPair p1;
        p1.first = key;
        int k;
        k = rand_r(&globalseed) % (node->GetSize() + 1);
        while (positions.find(k) != positions.end()) k = (k + 1) % (node->GetSize() + 1);
        EXPECT_EQ(node->InsertElementIfPossible(p1, node->Begin() + k), true);
        positions[k] = key;
        EXPECT_EQ(node->GetSize(), key + 1);
    }

    using ElementType = BPlusTree<int, TestValue>::KeyNodePointerPair;

    unsigned key = 0;
    for (ElementType *element_p = node->Begin(); element_p != node->End(); element_p++) {
        EXPECT_EQ(element_p->first, positions[key]);
        key++;
    }

    // To Check if we are inserting at the correct place
    EXPECT_EQ(
            reinterpret_cast<char *>(node) + sizeof(BPlusTree<int, TestValue>::ElasticNode<BPlusTree<int, TestValue>>),
            reinterpret_cast<char *>(node->Begin()));

    EXPECT_EQ(&(node->GetLowKeyPair()), node->GetElasticLowKeyPair());
    EXPECT_EQ(&(node->GetHighKeyPair()), node->GetElasticHighKeyPair());
    EXPECT_EQ(node->GetLowKeyPair().first, p1.first);
    EXPECT_EQ(node->GetHighKeyPair().first, p2.first);
    EXPECT_NE(&p1, &(node->GetLowKeyPair()));
    EXPECT_NE(&p2, &(node->GetHighKeyPair()));

    // Free the node - should not result in an ASAN
    node->FreeElasticNode();
    delete bplustree;
}


/**
 * Explicitly verifies splitting of a node
 */
void SplitNodeTest() {
    auto bplustree = new BPlusTree<int, TestValue>;
    BPlusTree<int, TestValue>::KeyNodePointerPair p1;
    BPlusTree<int, TestValue>::KeyNodePointerPair p2;

    // Get inner Node
    int size = 10;
    int depth = 0;
    int item_count = 10;  // Usually equal to size
    auto node = BPlusTree<int, TestValue>::ElasticNode<BPlusTree<int, TestValue>::KeyNodePointerPair>::Get(
            size, BPlusTree<int, TestValue>::NodeType::LeafType, depth, item_count, p1, p2);

    for (int key = 0; key < item_count; key++) {
        BPlusTree<int, TestValue>::KeyNodePointerPair p1;
        p1.first = key;
        EXPECT_EQ(node->InsertElementIfPossible(p1, node->End()), true);
        EXPECT_EQ(node->GetSize(), key + 1);
    }

    auto newnode = node->SplitNode();

    using ElementType = BPlusTree<int, TestValue>::KeyNodePointerPair;

    unsigned key = 0;
    for (ElementType *element_p = node->Begin(); element_p != node->End(); element_p++) {
        EXPECT_EQ(element_p->first, key);
        key++;
    }

    EXPECT_EQ(key, 5);

    for (ElementType *element_p = newnode->Begin(); element_p != newnode->End(); element_p++) {
        EXPECT_EQ(element_p->first, key);
        key++;
    }

    EXPECT_EQ(key, 10);

    EXPECT_EQ(&(node->GetLowKeyPair()), node->GetElasticLowKeyPair());
    EXPECT_EQ(&(node->GetHighKeyPair()), node->GetElasticHighKeyPair());
    EXPECT_EQ(node->GetLowKeyPair().first, p1.first);
    EXPECT_EQ(node->GetHighKeyPair().first, p2.first);
    EXPECT_NE(&p1, &(node->GetLowKeyPair()));
    EXPECT_NE(&p2, &(node->GetHighKeyPair()));

    EXPECT_EQ(&(newnode->GetLowKeyPair()), newnode->GetElasticLowKeyPair());
    EXPECT_EQ(&(newnode->GetHighKeyPair()), newnode->GetElasticHighKeyPair());
    EXPECT_EQ(newnode->GetLowKeyPair().first, p1.first);
    EXPECT_EQ(newnode->GetHighKeyPair().first, p2.first);
    EXPECT_NE(&p1, &(newnode->GetLowKeyPair()));
    EXPECT_NE(&p2, &(newnode->GetHighKeyPair()));

    // Free the node - should not result in an ASAN
    node->FreeElasticNode();
    newnode->FreeElasticNode();
    delete bplustree;
}

/**
 * Tests whether the FindLocation function correctly fetches the location of a key in the node.
 */
void FindLocationTest() {
    auto bplustree = new BPlusTree<int, TestValue>;
    BPlusTree<int, TestValue>::KeyNodePointerPair p1;
    BPlusTree<int, TestValue>::KeyNodePointerPair p2;

    // Get inner Node
    auto size = 10;
    auto depth = 0;
    auto item_count = 10;  // Usually equal to size
    auto node = BPlusTree<int, TestValue>::ElasticNode<BPlusTree<int, TestValue>::KeyNodePointerPair>::Get(
            size, BPlusTree<int, TestValue>::NodeType::LeafType, depth, item_count, p1, p2);

    std::set<unsigned> s;
    while (node->GetSize() < node->GetItemCount()) {
        int k = rand_r(&globalseed);
        while (s.find(k) != s.end()) k++;
        s.insert(k);
        BPlusTree<int, TestValue>::KeyNodePointerPair p;
        p.first = k;
        EXPECT_EQ(node->InsertElementIfPossible(
                p, static_cast<BPlusTree<int, TestValue>::InnerNode *>(node)->FindLocation(k, bplustree)),
                  true);
    }
    auto iter = node->Begin();
    for (auto &elem : s) {
        EXPECT_EQ(iter->first, elem);
        iter++;
    }

    // To Check if we are inserting at the correct place
    EXPECT_EQ(
            reinterpret_cast<char *>(node) + sizeof(BPlusTree<int, TestValue>::ElasticNode<BPlusTree<int, TestValue>>),
            reinterpret_cast<char *>(node->Begin()));

    EXPECT_EQ(&(node->GetLowKeyPair()), node->GetElasticLowKeyPair());
    EXPECT_EQ(&(node->GetHighKeyPair()), node->GetElasticHighKeyPair());
    EXPECT_EQ(node->GetLowKeyPair().first, p1.first);
    EXPECT_EQ(node->GetHighKeyPair().first, p2.first);
    EXPECT_NE(&p1, &(node->GetLowKeyPair()));
    EXPECT_NE(&p2, &(node->GetHighKeyPair()));

    // Free the node - should not result in an ASAN
    node->FreeElasticNode();
    delete bplustree;
}

/**
 * Tests the PopBegin() function in a node.
 */
void PopBeginTest() {
    auto bplustree = new BPlusTree<int, TestValue>;
    BPlusTree<int, TestValue>::KeyNodePointerPair p1;
    BPlusTree<int, TestValue>::KeyNodePointerPair p2;

    // Get inner Node
    int size = 10;
    int depth = 0;
    int item_count = 10;  // Usually equal to size
    auto node = BPlusTree<int, TestValue>::ElasticNode<BPlusTree<int, TestValue>::KeyNodePointerPair>::Get(
            size, BPlusTree<int, TestValue>::NodeType::LeafType, depth, item_count, p1, p2);

    // To check if we can read what we inserted
    std::vector<BPlusTree<int, TestValue>::KeyNodePointerPair> values;
    for (int key = 0; key < item_count; key++) {
        BPlusTree<int, TestValue>::KeyNodePointerPair p1;
        p1.first = key;
        node->PushBack(p1);
        EXPECT_EQ(node->GetSize(), key + 1);
    }

    using ElementType = BPlusTree<int, TestValue>::KeyNodePointerPair;
    unsigned i = 0;
    while (node->PopBegin()) {
        i++;
        unsigned key = i;
        for (ElementType *element_p = node->Begin(); element_p != node->End(); element_p++) {
            EXPECT_EQ(element_p->first, key);
            key++;
        }
        EXPECT_EQ(key, 10);
    }

    EXPECT_EQ(i, 10);

    // To Check if we are inserting at the correct place
    EXPECT_EQ(
            reinterpret_cast<char *>(node) + sizeof(BPlusTree<int, TestValue>::ElasticNode<BPlusTree<int, TestValue>>),
            reinterpret_cast<char *>(node->Begin()));

    EXPECT_EQ(&(node->GetLowKeyPair()), node->GetElasticLowKeyPair());
    EXPECT_EQ(&(node->GetHighKeyPair()), node->GetElasticHighKeyPair());
    EXPECT_EQ(node->GetLowKeyPair().first, p1.first);
    EXPECT_EQ(node->GetHighKeyPair().first, p2.first);
    EXPECT_NE(&p1, &(node->GetLowKeyPair()));
    EXPECT_NE(&p2, &(node->GetHighKeyPair()));

    // Free the node - should not result in an ASAN
    node->FreeElasticNode();
    delete bplustree;
}

/**
 * Tests the PopEnd() function in a node.
 */
void PopEndTest() {
    auto bplustree = new BPlusTree<int, TestValue>;
    BPlusTree<int, TestValue>::KeyNodePointerPair p1;
    BPlusTree<int, TestValue>::KeyNodePointerPair p2;

    // Get inner Node
    int size = 10;
    int depth = 0;
    int item_count = 10;  // Usually equal to size
    auto node = BPlusTree<int, TestValue>::ElasticNode<BPlusTree<int, TestValue>::KeyNodePointerPair>::Get(
            size, BPlusTree<int, TestValue>::NodeType::LeafType, depth, item_count, p1, p2);

    // To check if we can read what we inserted
    for (int key = 0; key < item_count; key++) {
        BPlusTree<int, TestValue>::KeyNodePointerPair p1;
        p1.first = key;
        node->PushBack(p1);
        EXPECT_EQ(node->GetSize(), key + 1);
    }

    // using ElementType = BPlusTree<int, TestValue>::KeyNodePointerPair;
    unsigned key = 9;
    while (node->PopEnd()) {
        if (node->GetSize() <= 0) break;
        key--;
        auto last = node->RBegin();
        EXPECT_EQ(last->first, key);
    }

    EXPECT_EQ(key, 0);

    // To Check if we are inserting at the correct place
    EXPECT_EQ(
            reinterpret_cast<char *>(node) + sizeof(BPlusTree<int, TestValue>::ElasticNode<BPlusTree<int, TestValue>>),
            reinterpret_cast<char *>(node->Begin()));

    EXPECT_EQ(&(node->GetLowKeyPair()), node->GetElasticLowKeyPair());
    EXPECT_EQ(&(node->GetHighKeyPair()), node->GetElasticHighKeyPair());
    EXPECT_EQ(node->GetLowKeyPair().first, p1.first);
    EXPECT_EQ(node->GetHighKeyPair().first, p2.first);
    EXPECT_NE(&p1, &(node->GetLowKeyPair()));
    EXPECT_NE(&p2, &(node->GetHighKeyPair()));

    // Free the node - should not result in an ASAN
    node->FreeElasticNode();
    delete bplustree;
}


/**
 * Verify that erasing an element from a node works correctly.
 */
void NodeElementEraseTest() {
    auto bplustree = new BPlusTree<int, TestValue>;
    BPlusTree<int, TestValue>::KeyNodePointerPair p1;
    BPlusTree<int, TestValue>::KeyNodePointerPair p2;

    // Get inner Node
    int size = 10;
    int depth = 0;
    int item_count = 10;  // Usually equal to size
    auto node = BPlusTree<int, TestValue>::ElasticNode<BPlusTree<int, TestValue>::KeyNodePointerPair>::Get(
            size, BPlusTree<int, TestValue>::NodeType::LeafType, depth, item_count, p1, p2);

    // To check if we can read what we inserted
    for (int key = 0; key < item_count; key++) {
        BPlusTree<int, TestValue>::KeyNodePointerPair p1;
        p1.first = key;
        node->PushBack(p1);
        EXPECT_EQ(node->GetSize(), key + 1);
    }

    // using ElementType = BPlusTree<int, TestValue>::KeyNodePointerPair;
    int key = 9;
    while (node->Erase(key)) {
        if (node->GetSize() <= 0) break;
        key--;
        auto last = node->RBegin();
        EXPECT_EQ(last->first, key);
    }

    EXPECT_EQ(key, 0);

    // To Check if we are inserting at the correct place
    EXPECT_EQ(
            reinterpret_cast<char *>(node) + sizeof(BPlusTree<int, TestValue>::ElasticNode<BPlusTree<int, TestValue>>),
            reinterpret_cast<char *>(node->Begin()));

    EXPECT_EQ(&(node->GetLowKeyPair()), node->GetElasticLowKeyPair());
    EXPECT_EQ(&(node->GetHighKeyPair()), node->GetElasticHighKeyPair());
    EXPECT_EQ(node->GetLowKeyPair().first, p1.first);
    EXPECT_EQ(node->GetHighKeyPair().first, p2.first);
    EXPECT_NE(&p1, &(node->GetLowKeyPair()));
    EXPECT_NE(&p2, &(node->GetHighKeyPair()));

    // Free the node - should not result in an ASAN
    node->FreeElasticNode();
    delete bplustree;
}


/**
 * Test merging by explicitly calling MergeNode on two nodes
 */
void NodeMergeTest() {
    auto bplustree = new BPlusTree<int, TestValue>;
    BPlusTree<int, TestValue>::KeyNodePointerPair p1;
    BPlusTree<int, TestValue>::KeyNodePointerPair p2;

    // Get inner Node
    int size = 10;
    int depth = 0;
    int item_count = 10;  // Usually equal to size
    auto node = BPlusTree<int, TestValue>::ElasticNode<BPlusTree<int, TestValue>::KeyNodePointerPair>::Get(
            size, BPlusTree<int, TestValue>::NodeType::LeafType, depth, item_count, p1, p2);
    auto next_node = BPlusTree<int, TestValue>::ElasticNode<BPlusTree<int, TestValue>::KeyNodePointerPair>::Get(
            size, BPlusTree<int, TestValue>::NodeType::LeafType, depth, item_count, p1, p2);

    for (int i = 0; i < (item_count / 2); i++) {
        BPlusTree<int, TestValue>::KeyNodePointerPair p1;
        BPlusTree<int, TestValue>::KeyNodePointerPair p2;
        p1.first = i;
        p2.first = i + 5;
        EXPECT_EQ(node->InsertElementIfPossible(p1, node->End()), true);
        EXPECT_EQ(next_node->InsertElementIfPossible(p2, next_node->End()), true);
        EXPECT_EQ(node->GetSize(), i + 1);
        EXPECT_EQ(next_node->GetSize(), i + 1);
    }

    using ElementType = BPlusTree<int, TestValue>::KeyNodePointerPair;
    EXPECT_EQ(node->MergeNode(next_node), true);

    unsigned key = 0;
    for (ElementType *element_p = node->Begin(); element_p != node->End(); element_p++) {
        EXPECT_EQ(element_p->first, key);
        key++;
    }
    EXPECT_EQ(key, 10);

    // To Check if we are inserting at the correct place
    EXPECT_EQ(
            reinterpret_cast<char *>(node) + sizeof(BPlusTree<int, TestValue>::ElasticNode<BPlusTree<int, TestValue>>),
            reinterpret_cast<char *>(node->Begin()));

    EXPECT_EQ(&(node->GetLowKeyPair()), node->GetElasticLowKeyPair());
    EXPECT_EQ(&(node->GetHighKeyPair()), node->GetElasticHighKeyPair());
    EXPECT_EQ(node->GetLowKeyPair().first, p1.first);
    EXPECT_EQ(node->GetHighKeyPair().first, p2.first);
    EXPECT_NE(&p1, &(node->GetLowKeyPair()));
    EXPECT_NE(&p2, &(node->GetHighKeyPair()));

    // Free the node - should not result in an ASAN
    node->FreeElasticNode();
    next_node->FreeElasticNode();
    delete bplustree;
}

// NOLINTNEXTLINE
int main() {
    BasicNodeInitPushBackTest();
    InsertElementInNodeTest();
    InsertElementInNodeRandomTest();
    SplitNodeTest();
    FindLocationTest();
    PopBeginTest();
    PopEndTest();
    NodeElementEraseTest();
    NodeMergeTest();
    return 0;
}
