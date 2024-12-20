#include <controller.h>
#include <gtest/gtest.h>
#include <fstream>
#include <filesystem>
#include <thread>

#include "tools.h"
#include "worker.h"
#include "controller.h"



TEST(insertNode, test)
{
    auto id = 1;
    std::shared_ptr<Node> root = nullptr;

    auto ok = InsertNode(root, id, 1234);

    ASSERT_TRUE(ok);
    ASSERT_EQ(root->id, id);

    auto ok1 = InsertNode(root, id + 1, 5678);

    ASSERT_TRUE(ok1);
    ASSERT_FALSE(root->right == nullptr);
    ASSERT_EQ(root->right->id, id + 1);

    ASSERT_TRUE(root->left == nullptr);

    auto bad = InsertNode(root, id, 1234);
    ASSERT_FALSE(bad);
}

TEST(findNode, test)
{
    auto id = 1;
    auto pid = 234;
    std::shared_ptr<Node> root = nullptr;

    InsertNode(root, id, pid);
    InsertNode(root, id + 1, pid);

    auto first = FindNode(root, id);
    auto second = FindNode(root, id + 1);
    auto bad = FindNode(root, id - 1);

    ASSERT_EQ(first->id, id);
    ASSERT_EQ(second->id, id + 1);
    ASSERT_TRUE(bad == nullptr);
}

TEST(ToolsTest, InsertAndFindNode) {
    std::shared_ptr<Node> root = nullptr;
    
    
    bool inserted = InsertNode(root, 10, 100);
    ASSERT_TRUE(inserted);
    ASSERT_NE(root, nullptr);
    EXPECT_EQ(root->id, 10);
    EXPECT_EQ(root->pid, 100);
    
    
    inserted = InsertNode(root, 5, 101);
    ASSERT_TRUE(inserted);
    auto node = FindNode(root, 5);
    ASSERT_NE(node, nullptr);
    EXPECT_EQ(node->id, 5);
    EXPECT_EQ(node->pid, 101);
    
    
    inserted = InsertNode(root, 15, 102);
    ASSERT_TRUE(inserted);
    node = FindNode(root, 15);
    ASSERT_NE(node, nullptr);
    EXPECT_EQ(node->id, 15);
    EXPECT_EQ(node->pid, 102);
    
    
    inserted = InsertNode(root, 10, 103);
    EXPECT_FALSE(inserted);
}



TEST(worker, test)
{
    const std::string msg_set = "exec MyVar 7";
    const std::string msg_get = "exec MyVar";
    const std::string resp_set = "Ok:1";
    const std::string resp_get = "Ok:1: 7";
    auto id = 1;
    std::shared_ptr<Node> root = nullptr;

    InsertNode(root, id, 1234);

    zmq::message_t message_set(msg_set);
    root->socket.send(message_set, zmq::send_flags::none);

    zmq::message_t reply_set;
    auto ok_set = root->socket.recv(reply_set, zmq::recv_flags::none);

    EXPECT_TRUE(ok_set);
    EXPECT_EQ(reply_set.to_string(), resp_set);

    zmq::message_t message_get(msg_get);
    root->socket.send(message_get, zmq::send_flags::none);

    zmq::message_t reply_get;
    auto ok_get = root->socket.recv(reply_get, zmq::recv_flags::none);

    EXPECT_TRUE(ok_get);
    EXPECT_EQ(reply_get.to_string(), resp_get);

    TerminateNodes(root);
}


int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
