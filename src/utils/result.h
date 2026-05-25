#ifndef RESULT_H
#define RESULT_H

#include <QString>

template<typename T>
class Result
{
public:
    static Result<T> ok(T value)
    {
        Result<T> r;
        r.m_value = value;
        r.m_isOk = true;
        return r;
    }

    static Result<T> error(QString message)
    {
        Result<T> r;
        r.m_error = message;
        r.m_isOk = false;
        return r;
    }

    bool isOk() const { return m_isOk; }
    bool isError() const { return !m_isOk; }

    T value() const { return m_value; }

    QString errorMessage() const { return m_error; }

private:
    Result() : m_isOk(false) {}

    T m_value;
    QString m_error;
    bool m_isOk;
};

// Void specialization for operations that return nothing
template<>
class Result<void>
{
public:
    static Result<void> ok()
    {
        Result<void> r;
        r.m_isOk = true;
        return r;
    }

    static Result<void> error(QString message)
    {
        Result<void> r;
        r.m_error = message;
        r.m_isOk = false;
        return r;
    }

    bool isOk() const { return m_isOk; }
    bool isError() const { return !m_isOk; }
    QString errorMessage() const { return m_error; }

private:
    Result() : m_isOk(false) {}

    QString m_error;
    bool m_isOk;
};

#endif // RESULT_H
