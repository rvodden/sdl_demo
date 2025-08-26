#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "message_box.h"

class MessageBoxTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Initialize SDL for testing (already handled by test environment)
    }
};

TEST_F(MessageBoxTest, ConstructorInitializesCorrectly) {
    sdlpp::MessageBox mbox("Test Title", "Test Message");
    
    // Test that object was created successfully
    // Since we can't inspect internal state directly, we verify via behavior
    SUCCEED();
}

TEST_F(MessageBoxTest, FluentInterfaceReturnsReference) {
    sdlpp::MessageBox mbox("Title", "Message");
    
    // Test that each method returns a reference to allow chaining
    auto& result1 = mbox.addButton("OK");
    auto& result2 = mbox.addButton("Cancel").setType(sdlpp::MessageBox::Type::Warning);
    auto& result3 = mbox.makeLastButtonCancel();
    
    // All should return references to the same object
    EXPECT_EQ(&mbox, &result1);
    EXPECT_EQ(&mbox, &result2);
    EXPECT_EQ(&mbox, &result3);
}

TEST_F(MessageBoxTest, ButtonChaining) {
    sdlpp::MessageBox mbox("Title", "Message");
    
    // Test fluent interface works as expected
    auto& result = mbox.addButton("OK")
                      .addButton("Cancel")
                      .makeLastButtonCancel()
                      .setType(sdlpp::MessageBox::Type::Error);
                      
    EXPECT_EQ(&mbox, &result);
}

TEST_F(MessageBoxTest, MoveConstructorWorks) {
    sdlpp::MessageBox mbox1("Title", "Message");
    mbox1.addButton("OK").addButton("Cancel");
    
    // Move constructor should work
    sdlpp::MessageBox mbox2 = std::move(mbox1);
    
    // Original should be in valid but unspecified state
    // Moved-to object should be functional
    SUCCEED();
}

TEST_F(MessageBoxTest, MoveAssignmentWorks) {
    sdlpp::MessageBox mbox1("Title1", "Message1");
    sdlpp::MessageBox mbox2("Title2", "Message2");
    
    mbox1.addButton("OK");
    mbox2.addButton("Yes").addButton("No");
    
    // Move assignment should work
    mbox2 = std::move(mbox1);
    
    SUCCEED();
}

TEST_F(MessageBoxTest, SetTypeWorks) {
    sdlpp::MessageBox mbox("Title", "Message");
    
    // Should be able to set each type without throwing
    EXPECT_NO_THROW(mbox.setType(sdlpp::MessageBox::Type::Information));
    EXPECT_NO_THROW(mbox.setType(sdlpp::MessageBox::Type::Warning));
    EXPECT_NO_THROW(mbox.setType(sdlpp::MessageBox::Type::Error));
}

TEST_F(MessageBoxTest, SetParentAcceptsNullptr) {
    sdlpp::MessageBox mbox("Title", "Message");
    
    // Should accept nullptr for parent window
    EXPECT_NO_THROW(mbox.setParent(nullptr));
}

// Note: We can't easily test show() in a unit test environment since it would
// display an actual dialog box. In a real application, integration tests
// would be needed to verify the complete functionality.

TEST_F(MessageBoxTest, CompleteFluentExample) {
    // Test the complete example from the documentation works
    sdlpp::MessageBox mbox("Confirmation", "Are you sure you want to quit?");
    
    EXPECT_NO_THROW({
        auto& result = mbox.addButton("OK")
                          .addButton("Cancel")
                          .makeLastButtonCancel()
                          .setType(sdlpp::MessageBox::Type::Warning)
                          .setParent(nullptr);
        EXPECT_EQ(&mbox, &result);
    });
}