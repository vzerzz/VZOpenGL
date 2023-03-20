#pragma once
#include <string>

#include <glad/glad.h>
class Shader
{
public:
    explicit Shader(std::string_view file_path);

    //禁用拷贝构造函数
    Shader(const Shader&) = delete;
    Shader &operator=(const Shader &) = delete;

    Shader(Shader &&other) noexcept = default;
    Shader &operator=(Shader &&other) noexcept = default;

    ~Shader();

    constexpr unsigned get_id() const noexcept { return id_; }

protected:
    unsigned id_;
    std::string source_;
};

class VertexShader : public Shader
{
public:
    explicit VertexShader(std::string_view file_path);
};

class FragmentShader : public Shader
{
public:
    explicit FragmentShader(std::string_view file_path);
};

class GeometryShader : public Shader
{
public:
    explicit GeometryShader(std::string_view file_path);
};

class ShaderProgram
{
public:
    ShaderProgram(std::string_view vertex_shader, std::string_view fragment_shader);
    ShaderProgram(std::string_view vertex_shader, std::string_view fragment_shader, std::string_view geometry_shader);

    ~ShaderProgram();

    void use() const noexcept;

    void set_uniform(std::string_view name, bool value) const noexcept;
    void set_uniform(std::string_view name, int value) const noexcept;
    void set_uniform(std::string_view name, float value) const noexcept;
    void set_uniform(std::string_view name, float v0, float v1, float v2, float v3) const noexcept;
    void set_uniform(std::string_view name, float v0, float v1, float v2) const noexcept;
    void set_uniform(std::string_view name, GLsizei count, GLboolean transpose, GLfloat* value) const noexcept;

    constexpr unsigned get_id() const noexcept { return id_; }
private:
    unsigned id_;
};