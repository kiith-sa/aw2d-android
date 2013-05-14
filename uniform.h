#ifndef UNIFORM_H
#define UNIFORM_H

#include <QGLShaderProgram>

/// Convenience wrapper for a GLSL uniform variable or array.
///
/// Allows to only reupload the uniform when it's modified.
///
/// valueCount should be 1 (default) for plain uniforms, array size for array 
/// uniforms.
template<class T, int valueCount = 1> class Uniform
{
    /// Value of the uniform variable/array.
    T value_[valueCount];
    /// Handle to the uniform in a QGLShaderProgram.
    int handle_;
    /// Do we need to reupload the uniform? (e.g. after modification).
    bool needReupload_;

public:
    /// Construct a Uniform with specified handle.
    ///
    /// The handle must be returned by a QGLShaderProgram.
    Uniform(const int handle)
        : handle_(handle)
        , needReupload_(true)
    {
        Q_ASSERT_X(valueCount >= 1, "Uniform::Uniform",
                   "Zero-value uniform arrays not supported");
        memset(value_, 0, sizeof(T) * valueCount);
    }

    /// Set the uniform's value (triggers reupload if changed).
    ///
    /// @param rhs   Value to set.
    /// @param index Index of the value in the value array (if this is an array uniform).
    void setValue(const T& rhs, const unsigned index = 0)
    {
        Q_ASSERT_X(index < valueCount, "Uniform::setValue", "value index out of bounds");
        if(value_[index] != rhs) {needReupload_ = true;}
        value_[index] = rhs;
    }

    /// Get the uniform's value.
    ///
    /// @param index Index of the value to get (if this is an array uniform).
    const T& value(const unsigned index = 0) const
    {
        return value_[index];
    }

    /// Return handle to the uniform.
    int handle() const 
    {
        return handle_;
    }

    /// Force the uniform to be uploaded before the next draw.
    ///
    /// Should be called after a shader is bound to ensure the uniforms are uploaded.
    void reset()
    {
        needReupload_ = true;
    }

    /// Upload the uniform to passed shader if its value has changed or it's been reset.
    ///
    /// @param shaderProgram Shader program this uniform belongs to. Must be the program
    ///                      that was used to determine the uniform's handle.
    void uploadIfNeeded(QGLShaderProgram& shaderProgram)
    {
        if(!needReupload_) {return;}

        if(valueCount == 1)
        {
            shaderProgram.setUniformValue(handle_, value_[0]);
        }
        else
        {
            setUniformArray(shaderProgram, handle_, value_);
        }
    }

private:
    /// Wrapper around QGLShaderProgram::setUniformValueArray for all types except float.
    template<class U>
    static void setUniformArray
        (QGLShaderProgram& program, const int handle, const U* values)
    {
        program.setUniformValueArray(handle, values, valueCount);
    }

    /// Wrapper around QGLShaderProgram::setUniformValueArray for floats.
    static void setUniformArray
        (QGLShaderProgram& program, const int handle, const float* values)
    {
        program.setUniformValueArray(handle, values, valueCount, 1);
    }
};

#endif // UNIFORM_H
