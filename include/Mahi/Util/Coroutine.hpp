#pragma once

#include <experimental/coroutine>
#include <memory>
#include <chrono>

namespace mahi {
namespace util {

//==============================================================================
// Forward Declarations / Typedefs
//==============================================================================

class Enumerator;
using SuspendAlways = std::experimental::suspend_always;
using SuspendNever  = std::experimental::suspend_never;

//==============================================================================
// YieldInsruction
//==============================================================================

/// Coroutine yield instruction base class
struct YieldInstruction {
    YieldInstruction();
    virtual ~YieldInstruction();
    virtual bool isOver();
};

/// Yield instruction which waits a certain duration in seconds
struct WaitForSeconds : public YieldInstruction {
    WaitForSeconds(float duration);
    bool isOver() override;
private:
    std::chrono::time_point<std::chrono::high_resolution_clock, std::chrono::milliseconds> m_endPoint;
};

//==============================================================================
// PromiseType
//==============================================================================

struct PromiseType {
    PromiseType();
    ~PromiseType();
    SuspendAlways initial_suspend();
    SuspendAlways final_suspend();
    Enumerator get_return_object();
    void unhandled_exception();
    SuspendNever  return_void();
    SuspendAlways yield_value(YieldInstruction* value);
    SuspendAlways yield_value(std::shared_ptr<YieldInstruction> value);
    std::shared_ptr<YieldInstruction> m_instruction;
};

//==============================================================================
// Coroutine
//==============================================================================

struct Coroutine : public YieldInstruction
{
    /// Destructor
    ~Coroutine();
    /// Stops the Coroutine
    void stop();
    /// Returns true if the Coroutine is over
    bool isOver() override;
    /// Move semantics
    Coroutine(Coroutine &&other);

private:

    friend class Object;
    friend class Enumerator;
    friend struct PromiseType;

    /// Constructor
    Coroutine(std::experimental::coroutine_handle<PromiseType> coroutine);

private:
    std::experimental::coroutine_handle<PromiseType> m_coroutine; ///< underlying handle
    bool m_stop;
};

//==============================================================================
// Enumerator
//==============================================================================

class Enumerator {
public:

    using promise_type = PromiseType;

    /// Destructor
    ~Enumerator();
    /// Move semantics
    Enumerator(Enumerator &&e);
    /// Advances Enumerator and returns true until completion
    bool moveNext();
    /// Gets the Coroutine
    std::shared_ptr<Coroutine> getCoroutine();

private:
    friend struct PromiseType;
    /// Constructor
    Enumerator(std::shared_ptr<Coroutine> h);
private:
    std::shared_ptr<Coroutine> m_ptr;
};

} // namespace util
} // namespace mahi
