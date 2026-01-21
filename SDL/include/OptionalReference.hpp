#pragma once
#include <optional>

template <typename Type>
using OptionalReference = std::optional<std::reference_wrapper<Type>>;