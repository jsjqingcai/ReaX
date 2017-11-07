#pragma once

#include "catch.hpp"

#define DONT_SET_USING_JUCE_NAMESPACE 1
#include "JuceHeader.h"

VARX_ENABLE_EXTRA_WARNINGS

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wheader-hygiene"
using namespace juce;
using namespace varx;
#pragma clang diagnostic pop


#define CONTEXT(desc) SECTION(std::string(" Context: ") + desc, "")
#define IT(desc) SECTION(std::string("       It ") + desc, "")

/// Subscribes to an Observable and collects all emitted values into a given Array.
#define varxCollectValues(__observable, __arrayName) \
    DisposeBag JUCE_JOIN_MACRO(__arrayName, JUCE_JOIN_MACRO(Subscription_, __LINE__)); \
    (__observable).subscribe([&__arrayName](const decltype(__arrayName.getFirst())& value) { __arrayName.add(value); }).disposedBy(JUCE_JOIN_MACRO(__arrayName, JUCE_JOIN_MACRO(Subscription_, __LINE__)));

/// REQUIREs that a given Array is equal to the given list of values.
#define varxRequireValues(__arrayName, ...) REQUIRE(__arrayName == decltype(__arrayName)({ __VA_ARGS__ }))

/// CHECKs that a given Array is equal to the given list of values.
#define varxCheckValues(__arrayName, ...) CHECK(__arrayName == decltype(__arrayName)({ __VA_ARGS__ }))


/// Runs the JUCE dispatch loop for a given time, to process async callbacks.
inline void varxRunDispatchLoop(int millisecondsToRunFor = 0)
{
    MessageManager::getInstance()->runDispatchLoopUntil(millisecondsToRunFor);
}

/// Runs the JUCE dispatch loop until a given condition is fulfilled.
#define varxRunDispatchLoopUntil(__condition) \
    { \
        const auto startTime = juce::Time::getMillisecondCounter(); \
        while (!(__condition) && Time::getMillisecondCounter() < startTime + 5 * 1000) { \
            varxRunDispatchLoop(5); \
        } \
    } \
    REQUIRE(__condition);

namespace juce {
inline std::ostream& operator<<(std::ostream& os, const var& v)
{
    os << v.toString();
    return os;
}
}


/// The app window for running the tests.
class TestWindow : public DocumentWindow, private DeletedAtShutdown
{
public:
    static TestWindow& getInstance()
    {
        static TestWindow* window = new TestWindow();
        return *window;
    }

    void addAndMakeVisible(Component& component)
    {
        getContentComponent()->addAndMakeVisible(component);
    }

private:
    TestWindow()
    : DocumentWindow("varx-Tests", Colours::white, DocumentWindow::TitleBarButtons::closeButton, true)
    {
        ScopedPointer<Component> component(new Component());
        component->setSize(1, 1);
        setUsingNativeTitleBar(true);
        setContentOwned(component.release(), true);
        setVisible(true);
    }
};

// Dummy struct that just counts copy and move constructions
struct CopyAndMoveConstructible
{
    struct Counters
    {
        int numCopyConstructions = 0;
        int numMoveConstructions = 0;
        int numCopyAssignments = 0;
        int numMoveAssignments = 0;
        bool printDebugMessages = false;
    };

    CopyAndMoveConstructible(Counters* counters)
    : counters(counters)
    {}

    // Copy Constructor
    CopyAndMoveConstructible(const CopyAndMoveConstructible& other)
    : counters(other.counters)
    {
        counters->numCopyConstructions++;
        if (counters->printDebugMessages)
            std::cout << "CopyAndMoveConstructible copy constructor" << std::endl;
    }

    // Move constructor
    CopyAndMoveConstructible(CopyAndMoveConstructible&& other)
    : counters(std::move(other.counters))
    {
        counters->numMoveConstructions++;
        if (counters->printDebugMessages)
            std::cout << "CopyAndMoveConstructible move constructor" << std::endl;
    }

    // Copy assignment
    CopyAndMoveConstructible& operator=(const CopyAndMoveConstructible& other)
    {
        counters = other.counters;
        counters->numCopyAssignments++;
        if (counters->printDebugMessages)
            std::cout << "CopyAndMoveConstructible copy assignment" << std::endl;

        return *this;
    }

    // Move assignment
    CopyAndMoveConstructible& operator=(CopyAndMoveConstructible&& other)
    {
        counters = std::move(other.counters);
        counters->numMoveAssignments++;
        if (counters->printDebugMessages)
            std::cout << "CopyAndMoveConstructible move assignment" << std::endl;

        return *this;
    }

    Counters* counters;
};
