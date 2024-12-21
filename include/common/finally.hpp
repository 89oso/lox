#pragma once

#include <functional>
#include <iostream>

namespace oso {
    class finally {
    public:
        using final_action_type = std::function<void()>;

        finally(final_action_type final_action) {
            _final_action = final_action;
        }

        ~finally() {
            _final_action();
        }

    private:
        final_action_type _final_action;
    };
} // namespace oso
