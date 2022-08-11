#include "pch.h"
#include "CoreErrors.h"

namespace {

struct CoreErrorsCategory : std::error_category
{
    const char* name() const noexcept override;
    std::string message(int ev) const override;
};

const char* CoreErrorsCategory::name() const noexcept
{
    return "Core errors.";
}

std::string CoreErrorsCategory::message(int ev) const
{
    switch (static_cast<CoreErrors>(ev))
    {
    case CoreErrors::kSuccess: 
        return "Success.";
    case CoreErrors::kFailToLoadResource: 
        return "Fail to load resource.";
    case CoreErrors::kResourceIsInvalid: 
        return "Resource is invalid.";
    case CoreErrors::kInternalError: 
        return "Internal error.";
    default: 
        return "Unknown error.";
    }
}

const CoreErrorsCategory TheCoreErrorsCategory{};

} // anonymous namespace

std::error_code make_error_code(CoreErrors e) {
    return { static_cast<int>(e), TheCoreErrorsCategory };
}
