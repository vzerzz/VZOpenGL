#include "Shader.h"
#include <string>
#include <string_view>
#include <fstream>
#include <sstream>
#include <iostream>

#include <glad/glad.h>
//---------------------------------------------------------------------------------------
// 1. 从文件路径中获取顶点/片段着色器
//---------------------------------------------------------------------------------------
Shader::Shader(std::string_view file_path)
:id_{0}{
    std::ifstream fs{};
    // 保证ifstream对象可以抛出异常
    fs.exceptions(std::ifstream::failbit|std::ifstream::badbit);

    try{
        // 打开文件
        fs.open(std::string{file_path});
        std::stringstream ss{};
        // 读取文件的缓冲内容到数据流中
        ss << fs.rdbuf();
        // 关闭文件处理器
        fs.close();
        // 转换数据流到string
        source_ = ss.str();
    }
    catch(std::ifstream::failure e){
        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ" << std::endl;        
    }
}

Shader::~Shader(){
    //删除着色器对象
    if(id_ != 0)
        glDeleteShader(id_);
}
//---------------------------------------------------------------------------------------
// 2. 编译着色器
//---------------------------------------------------------------------------------------
VertexShader::VertexShader(std::string_view file_path)
:Shader{file_path}{
    //创建一个ID引用的顶点着色器对象
    id_ = glCreateShader(GL_VERTEX_SHADER);
    
    auto source_str = source_.c_str();
    //把着色器源码附加到着色器对象上，然后编译
    glShaderSource(id_, 1, &source_str, NULL);//第二参数指定了传递的源码字符串数量，这里只有一个
    glCompileShader(id_);
    //检测编译时错误
    int success;
    char infoLog[512];
    glGetShaderiv(id_, GL_COMPILE_STATUS, &success);
    //获取错误消息
    if(!success){
        glGetShaderInfoLog(id_, 512, NULL, infoLog);
        std::cout<<"ERROR::SHADER::VERTEX::COMPILATION_FAILED\n"<<infoLog<<std::endl;
    }
}

FragmentShader::FragmentShader(std::string_view file_path)
:Shader{file_path}{
    //创建一个ID引用的片段着色器对象
    id_ = glCreateShader(GL_FRAGMENT_SHADER);
    auto source_str = source_.c_str();
    glShaderSource(id_, 1, &source_str, NULL);
    glCompileShader(id_);
    //检测编译时错误
    int success;
    char infoLog[512];
    glGetShaderiv(id_, GL_COMPILE_STATUS, &success);
    //获取错误消息
    if(!success){
        glGetShaderInfoLog(id_, 512, NULL, infoLog);
        std::cout<<"ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n"<<infoLog<<std::endl;
    }
}

GeometryShader::GeometryShader(std::string_view file_path)
:Shader{file_path}{
    //创建一个ID引用的片段着色器对象
    id_ = glCreateShader(GL_GEOMETRY_SHADER);
    auto source_str = source_.c_str();
    glShaderSource(id_, 1, &source_str, NULL);
    glCompileShader(id_);
    //检测编译时错误
    int success;
    char infoLog[512];
    glGetShaderiv(id_, GL_COMPILE_STATUS, &success);
    //获取错误消息
    if(!success){
        glGetShaderInfoLog(id_, 512, NULL, infoLog);
        std::cout<<"ERROR::SHADER::GEOMETRY::COMPILATION_FAILED\n"<<infoLog<<std::endl;
    }
}
//---------------------------------------------------------------------------------------
// 3. 着色器程序
//---------------------------------------------------------------------------------------
ShaderProgram::ShaderProgram(std::string_view vertex_shader, std::string_view fragment_shader)
:id_{0}{
    
    VertexShader vertexShader{vertex_shader};
    FragmentShader fragmentShader{fragment_shader};

    id_ = glCreateProgram();
    //把之前编译的着色器附加到程序对象上，然后链接它们
    glAttachShader(id_, vertexShader.get_id());
    glAttachShader(id_, fragmentShader.get_id());
    glLinkProgram(id_);
    //检测链接时错误
    int success;
    char infoLog[512] = "\0";
    glGetProgramiv(id_, GL_LINK_STATUS, &success);
    //获取错误消息
    if(!success){
        glGetShaderInfoLog(id_, 512, NULL, infoLog);
        std::cout<<"ERROR::SHADER::PROGRAM::LINKING_FAILED\n"<<infoLog<<std::endl;
    }
}

ShaderProgram::ShaderProgram(std::string_view vertex_shader, std::string_view fragment_shader, std::string_view geometry_shader)
:id_{0}{
    
    VertexShader vertexShader{vertex_shader};
    FragmentShader fragmentShader{fragment_shader};
    GeometryShader geometryShader{geometry_shader};

    id_ = glCreateProgram();
    //把之前编译的着色器附加到程序对象上，然后链接它们
    glAttachShader(id_, vertexShader.get_id());
    glAttachShader(id_, geometryShader.get_id());
    glAttachShader(id_, fragmentShader.get_id());
    glLinkProgram(id_);
    //检测链接时错误
    int success;
    char infoLog[512] = "\0";
    glGetProgramiv(id_, GL_LINK_STATUS, &success);
    //获取错误消息
    if(!success){
        glGetShaderInfoLog(id_, 512, NULL, infoLog);
        std::cout<<"ERROR::SHADER::PROGRAM::LINKING_FAILED\n"<<infoLog<<std::endl;
    }
}

ShaderProgram::~ShaderProgram(){
    if(id_ != 0)
        glDeleteProgram(id_);
}

void ShaderProgram::set_uniform(std::string_view name, bool value) const noexcept{
    glUniform1i(glGetUniformLocation(id_, name.data()), static_cast<int>(value));
}
void ShaderProgram::set_uniform(std::string_view name, int value) const noexcept{
    glUniform1i(glGetUniformLocation(id_, name.data()), value);
}
void ShaderProgram::set_uniform(std::string_view name, float value) const noexcept{
    glUniform1f(glGetUniformLocation(id_, name.data()), value);
}

void ShaderProgram::set_uniform(std::string_view name, float v0, float v1, float v2, float v3) const noexcept{
    glUniform4f(glGetUniformLocation(id_, name.data()), v0, v1, v2, v3);
}
void ShaderProgram::set_uniform(std::string_view name, float v0, float v1, float v2) const noexcept{
    glUniform3f(glGetUniformLocation(id_, name.data()), v0, v1, v2);
}
void ShaderProgram::set_uniform(std::string_view name, GLsizei count, GLboolean transpose, GLfloat* value) const noexcept{
    glUniformMatrix4fv(glGetUniformLocation(id_, name.data()), count, transpose, value);//第一个参数是uniform的位置值。第二个参数告诉OpenGL要发送多少个矩阵。第三个参数是否希望对矩阵进行转置。OpenGL通常使用列主序布局。GLM的默认布局就是列主序，所以并不需要转置矩阵。最后一个参数是真正的矩阵数据，但是GLM并不是把它们的矩阵储存为OpenGL所希望接受的那种，因此我们要先用GLM的自带的函数value_ptr来变换这些数据。
}
void ShaderProgram::use() const noexcept{
    glUseProgram(id_);
}
