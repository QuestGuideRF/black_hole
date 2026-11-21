#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <iostream>
#include <cmath>
using namespace glm;

// Глобальные переменные
const int WIDTH = 800;
const int HEIGHT = 600;

// Функции

// Структуры и классы
class Engine{
public:
    // -- Отрисовка квадрата и текстуры
    GLFWwindow* window;
    GLuint quadVAO;
    GLuint texture;
    GLuint shaderProgram;

    Engine(){
        this->window = StartGLFW();
        this->shaderProgram = CreateShaderProgram();
        
        auto result = QuadVAO();
        this->quadVAO = result[0];
        this->texture = result[1];
    }
    GLFWwindow* StartGLFW(){
        if(!glfwInit()){
            std::cerr<<"Не удалось инициализировать GLFW"<<std::endl;
            return nullptr;
        }

        GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Трассировщик лучей", NULL, NULL);
        glfwMakeContextCurrent(window);
        
        glewExperimental = GL_TRUE;
        if (glewInit() != GLEW_OK) {
            std::cerr << "Не удалось инициализировать GLEW." << std::endl;
            glfwTerminate();
            return nullptr;
        }

        glViewport(0, 0, WIDTH, HEIGHT);
        return window;
    };
    GLuint CreateShaderProgram(){
        const char* vertexShaderSource = R"(
        #version 330 core
        layout (location = 0) in vec2 aPos;  // Изменено на vec2
        layout (location = 1) in vec2 aTexCoord;
        out vec2 TexCoord;
        void main() {
            gl_Position = vec4(aPos, 0.0, 1.0);  // Явное z=0
            TexCoord = aTexCoord;
        })";

        const char* fragmentShaderSource = R"(
        #version 330 core
        in vec2 TexCoord;
        out vec4 FragColor;
        uniform sampler2D screenTexture;
        void main() {
            FragColor = texture(screenTexture, TexCoord);
        })";

        // Вершинный шейдер
        GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
        glCompileShader(vertexShader);

        // Фрагментный шейдер
        GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
        glCompileShader(fragmentShader);

        GLuint shaderProgram = glCreateProgram();
        glAttachShader(shaderProgram, vertexShader);
        glAttachShader(shaderProgram, fragmentShader);
        glLinkProgram(shaderProgram);

        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);

        return shaderProgram;
    };

    std::vector<GLuint> QuadVAO(){
        float quadVertices[] = {
            // Позиции    // Текстурные координаты
            -1.0f,  1.0f,  0.0f, 1.0f,  // Верхний левый угол
            -1.0f, -1.0f,  0.0f, 0.0f,  // Нижний левый угол
            1.0f, -1.0f,  1.0f, 0.0f,  // Нижний правый угол

            -1.0f,  1.0f,  0.0f, 1.0f,  // Верхний левый угол
            1.0f, -1.0f,  1.0f, 0.0f,  // Нижний правый угол
            1.0f,  1.0f,  1.0f, 1.0f   // Верхний правый угол

        };
        
        GLuint VAO, VBO;
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
        glEnableVertexAttribArray(1);

        GLuint texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        std::vector<GLuint> VAOtexture = {VAO, texture};
        return VAOtexture;
    }
    void renderScene(std::vector<unsigned char> pixels) {
        // Обновить текстуру результатами трассировки лучей
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, WIDTH, HEIGHT, 0, GL_RGB, 
                    GL_UNSIGNED_BYTE, pixels.data());

        // Очистить экран и нарисовать текстурированный квадрат
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glUseProgram(shaderProgram);

        GLint textureLocation = glGetUniformLocation(shaderProgram, "screenTexture");
        glUniform1i(textureLocation, 0);

        glBindVertexArray(quadVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        glfwSwapBuffers(window);
        glfwPollEvents();
    };
};

struct Ray{
    vec3 direction;
    vec3 origin;
    Ray(vec3 o, vec3 d) : origin(o), direction(normalize(d)){}
};
struct Material{
    vec3 color;
    float specular;
    float emission;
    Material(vec3 c, float s, float e) : color(c), specular(s), emission(e) {}
};
struct Object{
    vec3 centre;
    float radius;
    Material material;

    Object(vec3 c, float r, Material m) : centre(c), radius(r), material(m) {}

    bool Intersect(Ray &ray, float &t){
        vec3 oc = ray.origin - centre;
        float a = glm::dot(ray.direction, ray.direction); // Масштаб направления луча по t
        float b = 2.0f * glm::dot(oc, ray.direction);     // 
        float c = glm::dot(oc, oc) - radius * radius;     // Поправка на радиус сферы
        double discriminant = b*b - 4*a*c;
        if(discriminant < 0){return false;}          // Нет пересечения со сферой

        float intercept = (-b - sqrt(discriminant)) / (2.0f*a);
        if(intercept < 0){
            intercept = (-b + sqrt(discriminant)) / (2.0f*a);
            if(intercept<0){return false;}           // Пересечение находится позади начала луча
        }
        t = intercept;
        return true;
    };

    vec3 getNormal(vec3 &point) const{
        return normalize(point - centre);
    }
};

class Scene {
public:
    std::vector<Object> objs;
    vec3 lightPos;
    Scene() : lightPos(5.0f, 5.0f, 5.0f) {}

    vec3 trace(Ray &ray){
        float closest = INFINITY;
        const Object* hitObj = nullptr;

        for(auto& obj : objs){
            float t;                    // Расстояние до пересечения
            if(obj.Intersect(ray, t)){
                if(t < closest) {
                    closest = t;
                    hitObj = &obj;
                }
            }
        };
        if(hitObj){
            vec3 hitPoint = ray.origin + ray.direction * closest;     // Точка объекта, в которую попал луч
            vec3 normal = hitObj->getNormal(hitPoint);
            vec3 lightDir = normalize(lightPos - hitPoint);          // Направление от источника света к точке попадания

            float diff = std::max(glm::dot(normal, lightDir), 0.0f); // Диффузное освещение

            Ray shadowRay(hitPoint + normal * 0.001f, lightDir); // Небольшой сдвиг, чтобы избежать артефактов
            // Проверить, находится ли точка в тени
            bool inShadow = false;
            
            // Проверить тени: не перекрывает ли какой-либо объект источник света
            for(auto& obj : objs) {
                float t;
                if(obj.Intersect(shadowRay, t)) {
                    inShadow = true;
                    break;
                }
            }

            vec3 color = hitObj->material.color;
            float ambient = 0.1f; // Минимальный уровень освещения

            if (inShadow) {
                return color * ambient;
            }

            return color * (ambient + diff * 0.9f);
        }

        return vec3(0.0f, 0.0f, 0.1f); 
    }
};


// --- Главный цикл ---- //
int main(){
    Engine engine;
    Scene scene;

    scene.objs = {
        Object(vec3(0.0f, 0.0f, -5.0f), 2.0f, Material(vec3(1.0f, 0.2f, 0.2f), 0.5f, 0.0f)),   // Сдвинут дальше и увеличен
        Object(vec3(3.0f, 0.0f, -7.0f), 1.5f, Material(vec3(0.2f, 1.0f, 0.2f), 0.5f, 0.0f))    // Скорректированы положение и размер
    };
    // -- Цикл -- //
    std::vector<unsigned char> pixels(WIDTH * HEIGHT * 3);
    while(!glfwWindowShouldClose(engine.window)){
        glClear(GL_COLOR_BUFFER_BIT);

        // Отрисовать текстуру по пикселям
        for(int y = 0; y < HEIGHT; ++y){
            for(int x = 0; x < WIDTH; ++x){
                float aspectRatio = float(WIDTH) / float(HEIGHT);
                float u = float(x) / float(WIDTH);
                float v = float(y) / float(HEIGHT);

                // Направление луча через камеру
                vec3 direction(
                    (2.0f * u - 1.0f) * aspectRatio,
                    -(2.0f * v - 1.0f),  // Перевернуто для правильной ориентации
                    -1.0f  // Направление вперед по отрицательной оси z
                );
                Ray ray(vec3(0.0f, 0.0f, 0.0f), normalize(direction));
                vec3 color = scene.trace(ray);

                int index = (y * WIDTH + x) * 3;
                pixels[index + 0] = static_cast<unsigned char>(color.r * 255);
                pixels[index + 1] = static_cast<unsigned char>(color.g * 255);
                pixels[index + 2] = static_cast<unsigned char>(color.b * 255);
            }
        }
        
        engine.renderScene(pixels);
    }

    glfwTerminate();
}


// Объявления функций



