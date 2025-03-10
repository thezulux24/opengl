//=============================================================================
// Sample Application: Lighting (Per Fragment Phong)
//=============================================================================

#include <GL/glew.h>
#include <GL/freeglut.h>
#include "glApplication.h"
#include "glutWindow.h"
#include <iostream>
#include "glsl.h"
#include <time.h>
#include "glm.h"
#include <FreeImage.h> //*** Para Textura: Incluir librer�a
#define NM 8
//-----------------------------------------------------------------------------

class myWindow : public cwc::glutWindow {
protected:
    cwc::glShaderManager SM;
    cwc::glShader* shader;
    cwc::glShader* shader1; // Para Textura: shader para objetos texturizados
    GLuint ProgramObject;
    clock_t time0, time1;
    float timer010;  // timer counting 0->1->0
    bool bUp;        // flag if counting up or down.
    float rotationAngle; // Rotaci�n global horizontal de la escena
    float cameraHeight;  // Altura de la c�mara
    float zoom;          // Distancia de la c�mara
    GLMmodel* objmodel_ptr[NM];
    GLMmodel* objmodel_ptr1[NM]; // Para Textura: objetos texturizados
    GLuint texid[NM]; // Usamos los primeros n �ndices para texturas

public:
    myWindow() : rotationAngle(0.0f), cameraHeight(1.0f), zoom(4.0f) {}

    void abrirMalla(char* nombre, int i, int text) {
        if (text == 0) {
            objmodel_ptr[i] = glmReadOBJ(nombre);
            if (!objmodel_ptr[i])
                exit(0);
            glmUnitize(objmodel_ptr[i]);
            glmFacetNormals(objmodel_ptr[i]);
            glmVertexNormals(objmodel_ptr[i], 90.0);
        }
        else {
            objmodel_ptr1[i] = glmReadOBJ(nombre);
            if (!objmodel_ptr1[i])
                exit(0);
            glmUnitize(objmodel_ptr1[i]);
            glmFacetNormals(objmodel_ptr1[i]);
            glmVertexNormals(objmodel_ptr1[i], 90.0);
        }
    }

    // Funci�n para dibujar utilizando (si text==1) el shader de texturas
    void DibujarMalla(int i, float x, float y, float z, int text, float scale, float angle) {
        if (text == 0) {
            if (shader) shader->begin();
            glPushMatrix();
            glTranslatef(x, y, z);
            glRotatef(angle, 0.0f, 1.0f, 0.0f);
            glScalef(scale, scale, scale);
            glmDraw(objmodel_ptr[i], GLM_SMOOTH | GLM_MATERIAL);
            glPopMatrix();
            if (shader) shader->end();
        }
        else {
            if (shader1) shader1->begin();
            glPushMatrix();
            glTranslatef(x, y, z);
            glRotatef(angle, 0.0f, 1.0f, 0.0f);
            glScalef(scale, scale, scale);
            glBindTexture(GL_TEXTURE_2D, texid[i]);
            glmDraw(objmodel_ptr1[i], GLM_SMOOTH | GLM_MATERIAL | GLM_TEXTURE);
            glPopMatrix();
            if (shader1) shader1->end();
        }
    }

    // Sobrecarga, usando �ngulo 0
    void DibujarMalla(int i, float x, float y, float z, int text, float scale) {
        DibujarMalla(i, x, y, z, text, scale, 0.0f);
    }

    //*** M�todo para cargar texturas (se cargan en �ndices 0, 1, 2 y 3)
    void initialize_textures(int i, const char* filename) {
        glGenTextures(1, &texid[i]);
        glBindTexture(GL_TEXTURE_2D, texid[i]);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

        FIBITMAP* bitmap = FreeImage_Load(FreeImage_GetFileType(filename, 0), filename);
        if (!bitmap) {
            std::cerr << "Error loading texture: " << filename << std::endl;
            return;
        }
        FIBITMAP* pImage = FreeImage_ConvertTo32Bits(bitmap);
        int nWidth = FreeImage_GetWidth(pImage);
        int nHeight = FreeImage_GetHeight(pImage);
        GLenum format = GL_BGRA;
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, nWidth, nHeight, 0, format, GL_UNSIGNED_BYTE, (void*)FreeImage_GetBits(pImage));
        FreeImage_Unload(pImage);
        glEnable(GL_TEXTURE_2D);
    }

    virtual void OnRender(void) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        gluLookAt(0.0f, cameraHeight, zoom,
            0.0f, 0.0f, -1.0f,
            0.0f, 1.0f, 0.0f);

        glPushMatrix();
        glRotatef(rotationAngle, 0.0f, 1.0f, 0.0f);
        // Dibujar modelos texturizados:
        // Granjas (�ndice 0), silla (�ndice 1), pinocchio (�ndice 2) y cadena (�ndice 3)
        DibujarMalla(0, 0.0f, 0.0f, 0.0f, 1, 2.0f, 90.0f);
        DibujarMalla(1, 0.0f, -0.2f, 1.0f, 1, 2.5f, 90.0f);
        DibujarMalla(2, 0.0f, -0.08f, 1.03f, 1, 2.0f, 90.0f);
        DibujarMalla(3, 0.01f, -0.08f, 1.01f, 1, 2.0f, 90.0f);
		DibujarMalla(4, 0.8f, -0.2f, 0.6f, 1, 1.3f, 90.0f);
		DibujarMalla(0, 0.8f, -0.26f, 1.1f, 0, 2.0f, 0.0f);
		DibujarMalla(1, -1.0f, 0.0f, 1.0f, 0, 0.4f, 0.0f);
        glPopMatrix();
        glutSwapBuffers();
        UpdateTimer();
        Repaint();
    }

    virtual void OnIdle(void) {}

    virtual void OnInit(void) {
        // Inicializa FreeImage
        FreeImage_Initialise();

        glClearColor(0.5f, 0.5f, 1.0f, 0.0f);
        glShadeModel(GL_SMOOTH);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_COLOR_MATERIAL);
        glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

        shader = SM.loadfromFile("vertexshader.txt", "fragmentshader.txt");
        if (shader == 0)
            std::cout << "Error loading, compiling or linking shader\n";
        else
            ProgramObject = shader->GetProgramObject();

        shader1 = SM.loadfromFile("vertexshaderT.txt", "fragmentshaderT.txt");
        if (shader1 == 0)
            std::cout << "Error loading, compiling or linking shader\n";
        else
            ProgramObject = shader1->GetProgramObject();

        time0 = clock();
        timer010 = 0.0f;
        bUp = true;
        rotationAngle = 0.0f;
        cameraHeight = 1.0f;
        zoom = 4.0f;

        // Abrir mallas texturizadas (se pasan "1" para indicar textura)
        abrirMalla("./Mallas/granja.obj", 0, 1);
        abrirMalla("./Mallas/silla.obj", 1, 1);
        abrirMalla("./Mallas/pinocchio.obj", 2, 1);
        abrirMalla("./Mallas/cadena.obj", 3, 1);
        abrirMalla("./Mallas/casaperro.obj", 4, 1);
		abrirMalla("./Mallas/pollo.obj", 0, 0);
		abrirMalla("./Mallas/arbol.obj", 1, 0);

        // Cargar las texturas correspondientes
        initialize_textures(0, "./Mallas/granja.jpeg");
        initialize_textures(1, "./Mallas/silla.png");
        initialize_textures(2, "./Mallas/pinocchio.png");
        initialize_textures(3, "./Mallas/cadena.jpg");
		initialize_textures(4, "./Mallas/casaperro.png");
        

        DemoLight();
    }

    virtual void OnResize(int w, int h) {
        if (h == 0)
            h = 1;
        float ratio = 1.0f * w / h;
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glViewport(0, 0, w, h);
        gluPerspective(45, ratio, 1, 100);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
    }

    virtual void OnClose(void) {
        // Finaliza FreeImage (opcional)
        FreeImage_DeInitialise();
    }

    virtual void OnMouseDown(int button, int x, int y) {}
    virtual void OnMouseUp(int button, int x, int y) {}
    virtual void OnMouseWheel(int nWheelNumber, int nDirection, int x, int y) {
        if (nDirection > 0)
            zoom -= 0.5f; // Acercar
        else
            zoom += 0.5f; // Alejar
        if (zoom < 1.0f)
            zoom = 1.0f;
    }

    virtual void OnKeyDown(int nKey, char cAscii) {
        if (cAscii == 27)
            this->Close();
        if (nKey == GLUT_KEY_LEFT)
            rotationAngle -= 5.0f;
        if (nKey == GLUT_KEY_RIGHT)
            rotationAngle += 5.0f;
        if (nKey == GLUT_KEY_UP)
            cameraHeight += 0.1f;
        if (nKey == GLUT_KEY_DOWN)
            cameraHeight -= 0.1f;
    }

    virtual void OnKeyUp(int nKey, char cAscii) {
        if (cAscii == 's')
            shader->enable();
        else if (cAscii == 'f')
            shader->disable();
    }

    void UpdateTimer(void) {
        time1 = clock();
        float delta = static_cast<float>(static_cast<double>(time1 - time0) / static_cast<double>(CLOCKS_PER_SEC));
        delta = delta / 4;
        if (delta > 0.00005f) {
            time0 = clock();
            if (bUp) {
                timer010 += delta;
                if (timer010 >= 1.0f) {
                    timer010 = 1.0f;
                    bUp = false;
                }
            }
            else {
                timer010 -= delta;
                if (timer010 <= 0.0f) {
                    timer010 = 0.0f;
                    bUp = true;
                }
            }
        }
    }

    // Configuraci�n b�sica de la iluminaci�n
    void DemoLight(void) {
        glEnable(GL_LIGHTING);
        glEnable(GL_LIGHT0);
        glEnable(GL_NORMALIZE);
        GLfloat lmKa[] = { 0.2f, 0.2f, 0.2f, 1.0f };
        glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmKa);
        glLightModelf(GL_LIGHT_MODEL_LOCAL_VIEWER, 1.0f);
        glLightModelf(GL_LIGHT_MODEL_TWO_SIDE, 0.0f);
        GLfloat spot_direction[] = { 0.0f, -1.0f, -1.0f };
        GLint spot_exponent = 30;
        GLint spot_cutoff = 180;
        glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, spot_direction);
        glLighti(GL_LIGHT0, GL_SPOT_EXPONENT, spot_exponent);
        glLighti(GL_LIGHT0, GL_SPOT_CUTOFF, spot_cutoff);
        GLfloat Kc = 1.0f, Kl = 0.0f, Kq = 0.0f;
        glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, Kc);
        glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, Kl);
        glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, Kq);
        GLfloat light_pos[] = { 0.0f, 5.0f, 5.0f, 1.0f };
        GLfloat light_Ka[] = { 0.5f, 0.5f, 0.5f, 1.0f };
        GLfloat light_Kd[] = { 0.8f, 0.8f, 0.8f, 1.0f };
        GLfloat light_Ks[] = { 1.0f, 1.0f, 1.0f, 1.0f };
        glLightfv(GL_LIGHT0, GL_POSITION, light_pos);
        glLightfv(GL_LIGHT0, GL_AMBIENT, light_Ka);
        glLightfv(GL_LIGHT0, GL_DIFFUSE, light_Kd);
        glLightfv(GL_LIGHT0, GL_SPECULAR, light_Ks);
        GLfloat material_Ka[] = { 0.5f, 0.5f, 0.5f, 1.0f };
        GLfloat material_Kd[] = { 0.6f, 0.6f, 0.6f, 1.0f };
        GLfloat material_Ks[] = { 0.9f, 0.9f, 0.9f, 1.0f };
        GLfloat material_Ke[] = { 0.0f, 0.0f, 0.0f, 0.0f };
        GLfloat material_Se = 50.0f;
        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, material_Ka);
        glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, material_Kd);
        glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, material_Ks);
        glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, material_Ke);
        glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, material_Se);
    }
};

//-----------------------------------------------------------------------------

class myApplication : public cwc::glApplication {
public:
    virtual void OnInit(void) { std::cout << "Hello World!\n"; }
};

//-----------------------------------------------------------------------------

int main(void) {
    myApplication* pApp = new myApplication;
    myWindow* myWin = new myWindow();
    pApp->run();
    delete pApp;
    return 0;
}