#include "context.h"	
#include "image.h"

ContextUPtr Context::Create() {
    auto context = ContextUPtr(new Context());
    if (!context->Init())
        return nullptr;
    return std::move(context);
}

bool Context::Init() {

    float vertices[] = {                                    //[x, y, z, r, g, b, s, t]
        0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
        0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
        -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
        -0.5f, 0.5f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
    };
        uint32_t indices[] = {  // note that we start from 0! 점의 인덱스 표현
        0, 1, 3,                // first triangle
        1, 2, 3,                // second triangle
    };

    m_vertexLayout = VertexLayout::Create();                                       //array obj -> 버퍼 obj 순서중요

    m_vertexBuffer = Buffer::CreateWithData(GL_ARRAY_BUFFER,
        GL_STATIC_DRAW, vertices, sizeof(float) * 32);                             //용도는 "STATIC | DYNAMIC | STREAM", "DRAW | COPY | READ"의 조합
  	
    //m_vertexLayout->SetAttrib(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0);   //점의 위치 묘사 n | size | type | normailzed | stride | offset
    m_vertexLayout->SetAttrib(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, 0);
    m_vertexLayout->SetAttrib(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, sizeof(float) * 3);
    m_vertexLayout->SetAttrib(2, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 8, sizeof(float) * 6);

    m_indexBuffer = Buffer::CreateWithData(GL_ELEMENT_ARRAY_BUFFER,
        GL_STATIC_DRAW, indices, sizeof(uint32_t) * 6);
   
    ShaderPtr vertShader = Shader::CreateFromFile("./shader/texture.vs", GL_VERTEX_SHADER);
    ShaderPtr fragShader = Shader::CreateFromFile("./shader/texture.fs", GL_FRAGMENT_SHADER);
    if (!vertShader || !fragShader)
        return false;
    SPDLOG_INFO("vertex shader id: {}", vertShader->Get());
    SPDLOG_INFO("fragment shader id: {}", fragShader->Get());

    m_program = Program::Create({fragShader, vertShader});
    if (!m_program)
        return false;
    SPDLOG_INFO("program id: {}", m_program->Get());

    glClearColor(0.0f, 0.5f, 1.0f, 0.0f);
    
    auto image = Image::Load("./image/container.jpg");
    if (!image) 
        return false;
    SPDLOG_INFO("image: {}x{}, {} channels", 
        image->GetWidth(), image->GetHeight(), image->GetChannelCount());
	
    m_texture = Texture::CreateFromImage(image.get());        

    auto image2 = Image::Load("./image/awesomeface.png");
    m_texture2 = Texture::CreateFromImage(image2.get());

    glActiveTexture(GL_TEXTURE0);                                       //지금 건드릴려 하는 슬롯 번호
    glBindTexture(GL_TEXTURE_2D, m_texture->Get());                     //0슬롯 image
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, m_texture2->Get());                    //1슬롯 image2

    m_program->Use();
    glUniform1i(glGetUniformLocation(m_program->Get(), "tex"), 0);      
    glUniform1i(glGetUniformLocation(m_program->Get(), "tex2"), 1);
                 
    return true;
}

void Context::Render() {
    glClear(GL_COLOR_BUFFER_BIT);

    m_program->Use();
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    //m_program->Use();
    //glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);        //primitive | 인덱스의 개수 | 데이터타입 |pointer/offset
} 