#define _USE_MATH_DEFINES
#include <iostream>
#include <stdio.h>
#include<cmath>

// Nagłówki
#include <GL/glew.h>
#include <SFML/Window.hpp>



//Ponieważ każdy wierzchołek ma współrzędne 3D 
//tworzymy zmienną wejściową vec3 o nazwie aPos. 
//okreslamy lokalizacje na 0 

// Kody shaderów
const GLchar* vertexSource = R"glsl(
#version 150 core
in vec3 position;
in vec3 color;
out vec3 Color;
void main(){
Color = color;

gl_Position = vec4(position, 1.0);

}
)glsl";

const GLchar* fragmentSource = R"glsl(
#version 150 core
in vec3 Color;
out vec4 outColor;
void main()
{
outColor = vec4(Color, 1.0);
}
)glsl";
int main()
{
    //context settings
    sf::ContextSettings settings;
    settings.depthBits = 24;
    settings.stencilBits = 8;
    //Okno rendering w SFML
    sf::Window window(sf::VideoMode(600,600,32),"New Window",sf::Style::Titlebar | sf::Style::Close,settings);

    //Inicjalizacja GLEW
	glewExperimental = GL_TRUE;
	glewInit();

    // Utworzenie VAO (Vertex Array Object)
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

    //teraz chce wyslac do potoku fraficznego vertex shader
    //zarzadzamy tym poprzez obiekty bufora VBO <- przechowuja wierzcholki
    //generowanie tego bufora
    //TWORZENIE BUFFORA
    unsigned int VBO;
    glGenBuffers(1, &VBO);

    //Vertex Input (te wierzcholki) uklad kartezjanski w 2D
    int points = 36;
	GLfloat *vertices = new GLfloat[points * 6];

	float angle = 2 * M_PI / points;
	float alfa = 0;
	float R = 1;


	for (int i = 0; i < points * 6; i += 6)
	{
		//zmienne xyz
		vertices[i] = R * cos(alfa);
		vertices[i + 1] = R * sin(alfa);
		vertices[i + 2] = 0;
		//-1/1 
		vertices[i + 3] = (sin(alfa)+1) / 1.5;
		vertices[i + 4] = (cos(alfa)+1);
		vertices[i + 5] = (cos(alfa) + 1) / 2.0;
		alfa += angle;	
	}

    

    //typ bufora wierzcholkow to GL_ARRAY_BUFFER
    //Powiazemy te buffer yza pomoca glBlindBuffer
    //NADAWANIE MU TYPU
    glBindBuffer(GL_ARRAY_BUFFER, VBO);  
    //teraz wszystkie operacje wykonane na gl_array_buffer zwaizane sa z vbo
    //teraz kopiujemy wczesniej zdefiniowane dane wierzcholkow do bifera
    //KOPIOWANIE DANYCH DO NIEGO
    //glBufferData( typ, rozmiar danych, dane, w jaki sposob ma zarzadzac danymi)
    //GL_STATIC_DRAW: dane są ustawiane tylko raz i używane wielokrotnie.
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*points*6, vertices, GL_STATIC_DRAW);
    //Od teraz przechowuje ane w pamieci karty graficznej zarzadzanje przez obiekt bufora
    //wierzcholkow o nazwie VBO 
    //MAM BUROZ Z ZAPISANYMI DANYMI WIERZCHOLKOW


    unsigned int vertexShader;
    //poniewaz tworzymy vertex shader pzekazujemy GL_VERTEX_SHADER
    vertexShader = glCreateShader(GL_VERTEX_SHADER);


    //Funkcja glShaderSource jako pierwszy argument przyjmuje obiekt modułu cieniującego do kompilacji. 
    //Drugi argument określa, ile ciągów przekazujemy jako kod źródłowy, czyli tylko jeden. Trzeci parametr 
    //to rzeczywisty kod źródłowy Vertex Shadera i możemy zostawić 4 parametr na NULL.
    glShaderSource(vertexShader, 1, &vertexSource, NULL);
    glCompileShader(vertexShader);

    //sprawdzenie czy wszystko poszlo ok
    int  success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if(!success)
    {
    glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
    std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    else
    {
        std::cout<<"SHADER::VERTEX: OK"<<std::endl;
    }

    //Tworzenie vertex fragment
    GLuint fragmentShader;
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
	glCompileShader(fragmentShader);

    //Czy wszystko ok 
    int  successf;
    char infoLogf[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &successf);
    if(!successf)
    {
    glGetShaderInfoLog(vertexShader, 512, NULL, infoLogf);
    std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLogf << std::endl;
    }
    else
    {
        std::cout<<"SHADER::FRAGMENT: OK"<<std::endl;
    }

    //Shader Program
    //Funkcja glCreateProgram tworzy program i zwraca odniesienie ID do nowo utworzonego obiektu programu. Teraz
    //musimy dołączyć wcześniej skompilowane shadery do obiektu programu
    //a następnie połączyć je z glLinkProgram:
    unsigned int shaderProgram;
    shaderProgram = glCreateProgram();

    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
	glUseProgram(shaderProgram);

    //Sprawdzam czy ok
    int  successp;
    char infoLogp[512];
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &successp);
    if(!successp) {
    glGetProgramInfoLog(shaderProgram, 512, NULL, infoLogp);
    std::cout << "ERROR::SHADER::PROGRAM::COMPILATION_FAILED\n" << infoLogp << std::endl;
    }
    else
    {
        std::cout<<"SHADER::PROGRAM: OK"<<std::endl;
    }

    

    // Specifikacja formatu danych wierzchołkowych
	GLint posAttrib = glGetAttribLocation(shaderProgram, "position");
	glEnableVertexAttribArray(posAttrib);
	glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), 0);
	GLint colAttrib = glGetAttribLocation(shaderProgram, "color");
	glEnableVertexAttribArray(colAttrib);
	glVertexAttribPointer(colAttrib, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));




    //Pretla glowna
    bool running = true;
    while(running)
    {
        
        //stworzenie zmiennej na event
        sf::Event windowEvent;
        //Spawdzam czy okno zostalo zamkniete
        while(window.pollEvent(windowEvent))
        {
            if(windowEvent.type == sf::Event::Closed)
            {
                running = false;
            }
        }
        // Nadanie scenie koloru czarnego
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		// Narysowanie trójkąta na podstawie 3 wierzchołków
		glDrawArrays(GL_TRIANGLE_FAN, 0, points);
		// Wymiana buforów tylni/przedni
		window.display();


    }

    // Kasowanie programu i czyszczenie buforów
	glDeleteProgram(shaderProgram);
	glDeleteShader(fragmentShader);
	glDeleteShader(vertexShader);
	glDeleteBuffers(1, &VBO);
	glDeleteVertexArrays(1, &vao);
	// Zamknięcie okna renderingu
	window.close();
    
    return 0;
}