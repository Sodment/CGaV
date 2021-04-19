#include "shaderprogram.h"



//Procedura wczytuje plik do tablicy znaków.
char* ShaderProgram::readFile(const char* fileName) {
	int filesize;
	FILE* plik;
	char* result;

#pragma warning(disable : 4996) //Wy³¹czenie b³êdu w Visual Studio wynikaj¹ce z nietrzymania siê standardów przez Microsoft.
	plik = fopen(fileName, "rb");
	if (plik != NULL) {
		fseek(plik, 0, SEEK_END);
		filesize = ftell(plik);
		fseek(plik, 0, SEEK_SET);
		result = new char[filesize + 1];
#pragma warning(suppress : 6386) //Wy³¹czenie b³êdu w Visual Studio wynikaj¹cego z b³êdnej analizy statycznej kodu.
		int readsize = fread(result, 1, filesize, plik);
		result[filesize] = 0;
		fclose(plik);

		return result;
	}

	return NULL;

}

//Metoda wczytuje i kompiluje shader, a nastêpnie zwraca jego uchwyt
GLuint ShaderProgram::loadShader(GLenum shaderType, const char* fileName) {
	//Wygeneruj uchwyt na shader
	GLuint shader = glCreateShader(shaderType);//shaderType to GL_VERTEX_SHADER, GL_GEOMETRY_SHADER lub GL_FRAGMENT_SHADER
	//Wczytaj plik ze Ÿród³em shadera do tablicy znaków
	const GLchar* shaderSource = readFile(fileName);
	//Powi¹¿ Ÿród³o z uchwytem shadera
	glShaderSource(shader, 1, &shaderSource, NULL);
	//Skompiluj Ÿród³o
	glCompileShader(shader);
	//Usuñ Ÿród³o shadera z pamiêci (nie bêdzie ju¿ potrzebne)
	delete[]shaderSource;

	//Pobierz log b³êdów kompilacji i wyœwietl
	int infologLength = 0;
	int charsWritten = 0;
	char* infoLog;

	glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infologLength);

	if (infologLength > 1) {
		infoLog = new char[infologLength];
		glGetShaderInfoLog(shader, infologLength, &charsWritten, infoLog);
		printf("%s\n", infoLog);
		delete[]infoLog;
	}

	//Zwróæ uchwyt wygenerowanego shadera
	return shader;
}

ShaderProgram::ShaderProgram(const char* vertexShaderFile, const char* geometryShaderFile, const char* fragmentShaderFile) {
	//Wczytaj vertex shader
	printf("Loading vertex shader...\n");
	vertexShader = loadShader(GL_VERTEX_SHADER, vertexShaderFile);

	//Wczytaj geometry shader
	if (geometryShaderFile != NULL) {
		printf("Loading geometry shader...\n");
		geometryShader = loadShader(GL_GEOMETRY_SHADER, geometryShaderFile);
	}
	else {
		geometryShader = 0;
	}

	//Wczytaj fragment shader
	printf("Loading fragment shader...\n");
	fragmentShader = loadShader(GL_FRAGMENT_SHADER, fragmentShaderFile);

	//Wygeneruj uchwyt programu cieniuj¹cego
	shaderProgram = glCreateProgram();

	//Pod³¹cz do niego shadery i zlinkuj program
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	if (geometryShaderFile != NULL) glAttachShader(shaderProgram, geometryShader);
	glLinkProgram(shaderProgram);

	//Pobierz log b³êdów linkowania i wyœwietl
	int infologLength = 0;
	int charsWritten = 0;
	char* infoLog;

	glGetProgramiv(shaderProgram, GL_INFO_LOG_LENGTH, &infologLength);

	if (infologLength > 1)
	{
		infoLog = new char[infologLength];
		glGetProgramInfoLog(shaderProgram, infologLength, &charsWritten, infoLog);
		printf("%s\n", infoLog);
		delete[]infoLog;
	}

	printf("Shader program created \n");
}

ShaderProgram::~ShaderProgram() {
	//Od³¹cz shadery od programu
	glDetachShader(shaderProgram, vertexShader);
	if (geometryShader != 0) glDetachShader(shaderProgram, geometryShader);
	glDetachShader(shaderProgram, fragmentShader);

	//Wykasuj shadery
	glDeleteShader(vertexShader);
	if (geometryShader != 0) glDeleteShader(geometryShader);
	glDeleteShader(fragmentShader);

	//Wykasuj program
	glDeleteProgram(shaderProgram);
}


//W³¹cz u¿ywanie programu cieniuj¹cego reprezentowanego przez aktualny obiekt
void ShaderProgram::use() {
	glUseProgram(shaderProgram);
}

//Pobierz numer slotu odpowiadaj¹cego zmiennej jednorodnej o nazwie variableName
GLuint ShaderProgram::u(const char* variableName) {
	return glGetUniformLocation(shaderProgram, variableName);
}

//Pobierz numer slotu odpowiadaj¹cego atrybutowi o nazwie variableName
GLuint ShaderProgram::a(const char* variableName) {
	return glGetAttribLocation(shaderProgram, variableName);
}
