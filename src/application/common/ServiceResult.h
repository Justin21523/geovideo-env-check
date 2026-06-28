#pragma once

#include <QString>

#include <utility>

namespace geovideo::application {

class OperationResult final {
public:
    static OperationResult success()
    {
        OperationResult result;
        result.success_ = true;
        return result;
    }

    static OperationResult failure(QString errorMessage)
    {
        OperationResult result;
        result.success_ = false;
        result.errorMessage_ = std::move(errorMessage);
        return result;
    }

    bool isSuccess() const
    {
        return success_;
    }

    bool isFailure() const
    {
        return !success_;
    }

    QString errorMessage() const
    {
        return errorMessage_;
    }

private:
    bool success_ = false;
    QString errorMessage_;
};

template <typename T>
class ServiceResult final {
public:
    static ServiceResult success(T value)
    {
        ServiceResult result;
        result.success_ = true;
        result.value_ = std::move(value);
        return result;
    }

    static ServiceResult failure(QString errorMessage)
    {
        ServiceResult result;
        result.success_ = false;
        result.errorMessage_ = std::move(errorMessage);
        return result;
    }

    bool isSuccess() const
    {
        return success_;
    }

    bool isFailure() const
    {
        return !success_;
    }

    const T& value() const
    {
        return value_;
    }

    T& value()
    {
        return value_;
    }

    QString errorMessage() const
    {
        return errorMessage_;
    }

private:
    bool success_ = false;
    T value_{};
    QString errorMessage_;
};

} // namespace geovideo::application