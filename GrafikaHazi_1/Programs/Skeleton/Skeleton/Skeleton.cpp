//=============================================================================================
// Mintaprogram: Zöld háromszög. Ervenyes 2019. osztol.
//
// A beadott program csak ebben a fajlban lehet, a fajl 1 byte-os ASCII karaktereket tartalmazhat, BOM kihuzando.
// Tilos:
// - mast "beincludolni", illetve mas konyvtarat hasznalni
// - faljmuveleteket vegezni a printf-et kiveve
// - Mashonnan atvett programresszleteket forrasmegjeloles nelkul felhasznalni es
// - felesleges programsorokat a beadott programban hagyni!!!!!!! 
// - felesleges kommenteket a beadott programba irni a forrasmegjelolest kommentjeit kiveve
// ---------------------------------------------------------------------------------------------
// A feladatot ANSI C++ nyelvu forditoprogrammal ellenorizzuk, a Visual Studio-hoz kepesti elteresekrol
// es a leggyakoribb hibakrol (pl. ideiglenes objektumot nem lehet referencia tipusnak ertekul adni)
// a hazibeado portal ad egy osszefoglalot.
// ---------------------------------------------------------------------------------------------
// A feladatmegoldasokban csak olyan OpenGL fuggvenyek hasznalhatok, amelyek az oran a feladatkiadasig elhangzottak 
// A keretben nem szereplo GLUT fuggvenyek tiltottak.
//
// NYILATKOZAT
// ---------------------------------------------------------------------------------------------
// Nev    : Kerekes Adrienne
// Neptun : GFVHSO
// ---------------------------------------------------------------------------------------------
// ezennel kijelentem, hogy a feladatot magam keszitettem, es ha barmilyen segitseget igenybe vettem vagy
// mas szellemi termeket felhasznaltam, akkor a forrast es az atvett reszt kommentekben egyertelmuen jeloltem.
// A forrasmegjeloles kotelme vonatkozik az eloadas foliakat es a targy oktatoi, illetve a
// grafhazi doktor tanacsait kiveve barmilyen csatornan (szoban, irasban, Interneten, stb.) erkezo minden egyeb
// informaciora (keplet, program, algoritmus, stb.). Kijelentem, hogy a forrasmegjelolessel atvett reszeket is ertem,
// azok helyessegere matematikai bizonyitast tudok adni. Tisztaban vagyok azzal, hogy az atvett reszek nem szamitanak
// a sajat kontribucioba, igy a feladat elfogadasarol a tobbi resz mennyisege es minosege alapjan szuletik dontes.
// Tudomasul veszem, hogy a forrasmegjeloles kotelmenek megsertese eseten a hazifeladatra adhato pontokat
// negativ elojellel szamoljak el es ezzel parhuzamosan eljaras is indul velem szemben.
//=============================================================================================
#include "framework.h"

float sdot(const vec3& v1, const vec3& v2) { return (v1.x * v2.x + v1.y * v2.y + v1.z * v2.z * -1); }

float slength(const vec3& v) { return sqrtf(sdot(v, v)); }

vec3 snormalize(const vec3& v) { return v * (1 / slength(v)); }

vec3 pontotVetit(const vec3& v) {
	return vec3(v.x, v.y, sqrtf(((v.x * v.x) + (v.y * v.y) + 1)));
}

vec3 vektorVetit(const vec3& p, const vec3& v) {
	vec3 pont = pontotVetit(p);
	return snormalize(vec3(v.x, v.y, (v.x * p.x + v.y * p.y) / pont.z));
}

//1. Feladat: Egy irányra merõleges irány állítása. 
vec3 meroleges(const vec3& p, const vec3& v) {
	vec3 a = p;
	vec3 b = v;
	a.z *= -1;
	b.z *= -1;
	return snormalize(cross(a, b));
}

//2. Feladat: Adott pontból és sebesség vektorral induló pont helyének és sebesség vektorának számítása t idõvel késõbb. 
vec3 ujPont(const vec3& p, const vec3& v, float t) {
	return p * coshf(t) + v * sinhf(t);
}

vec3 ujIrany(const vec3& p, const vec3& v, float t) {
	return snormalize(p * sinhf(t) + v * coshf(t));
}

//3. Feladat: Egy ponthoz képest egy másik pont irányának és távolságának meghatározása. 
float tavolsag(const vec3& p, const vec3& q) {
	return acoshf(-sdot(p, q));
}

vec3 iranyAllit(const vec3& p, const vec3& q) {
	float t = tavolsag(p, q);
	return snormalize((q - p * coshf(t)) / sinh(t));
}

//4. Feladat:  Egy ponthoz képest adott irányban és távolságra lévõ pont elõállítása. 
vec3 eltol(const vec3& p, const vec3& v, float d) {
	vec3 n = snormalize(v);
	return p * coshf(d) + n * sinhf(d);
}

//5. Feladat: Egy pontban egy vektor elforgatása adott szöggel.
vec3 vektorForgat(const vec3& p, const vec3& v, float fi) {
	vec3 m = snormalize(meroleges(p, v));
	return snormalize(v * cosf(fi) + m * sinf(fi));
}

// vertex shader in GLSL: It is a Raw string (C++11) since it contains new line characters
const char* const vertexSource = R"(
	#version 330				// Shader 3.3
	precision highp float;		// normal floats, makes no difference on desktop computers
	layout(location = 0) in vec3 vp;	// Varying input: vp = vertex position is expected in attrib array 0
	void main() {
		gl_Position = vec4(vp.x/(vp.z + 1), vp.y/(vp.z + 1), 0, 1);		// transform vp from modeling space to normalized device space
	}
)";

// fragment shader in GLSL
const char* const fragmentSource = R"(
	#version 330			// Shader 3.3
	precision highp float;	// normal floats, makes no difference on desktop computers
	uniform vec3 color;		// uniform variable, the color of the primitive
	out vec4 outColor;		// computed color of the current pixel

	void main() {
		outColor = vec4(color, 1);	// computed color is the color of the primitive
	}
)";

GPUProgram gpuProgram; // vertex and fragment shaders
unsigned int vao;	   // virtual world on the GPU

class Kor {
	unsigned int vbo;
	vec3 szin;
	const static int nVertices = 100;

public:
	Kor(vec3 szin) :szin(szin) {
		glGenBuffers(1, &vbo);
	}

	void megjelenit() {
		vec3 kor[nVertices];
		for (int i = 0; i < nVertices; i++)
		{
			float phi = i * 2.0f * M_PI / nVertices;
			kor[i] = vec3(cosf(phi), sinf(phi), 0);
		}

		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vec3) * nVertices, kor, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		int location = glGetUniformLocation(gpuProgram.getId(), "color");
		glUniform3f(location, szin.x, szin.y, szin.z);
		glDrawArrays(GL_TRIANGLE_FAN, 0, nVertices);
	}
};

class Hami {
	unsigned int vbo;
	const static int nVertices = 100;
	vec3 szin;
	vec3 helyzet;
	vec3 irany;
	float r;

public:
	Hami(vec3 szin, vec3 hely, vec3 irany, float r) :szin(szin), helyzet(hely), irany(irany), r(r) {
		glGenBuffers(1, &vbo);
		helyzet = pontotVetit(helyzet);
		irany = vektorVetit(hely, irany);
	}

	void megjelenit() {
		vec3 hami[nVertices];
		for (int i = 0; i < nVertices; i++) {
			vec3 p = helyzet;
			vec3 v = irany;
			float phi = i * 2.0f * M_PI / nVertices;
			v = vektorForgat(p, v, phi);
			hami[i] = ujPont(p, v, r);
		}

		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vec3) * nVertices, hami, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		int location = glGetUniformLocation(gpuProgram.getId(), "color");
		glUniform3f(location, szin.x, szin.y, szin.z);
		glDrawArrays(GL_TRIANGLE_FAN, 0, nVertices);
	}

	vec3 getHelyzet() {
		return helyzet;
	}

	void setHelyzet(vec3 p) {
		helyzet = p;
	}

	vec3 getIrany() {
		return irany;
	}

	void setIrany(vec3 v) {
		irany = v;
	}

	float getR() {
		return r;
	}

	void setR(float rad) {
		r = rad;
	}
};


class Nyal {
	unsigned int vbo;
	vec3 szin;
	std::vector<vec3> csik;

public:
	Nyal(vec3 szin, vec3 pkezdo) :szin(szin) {
		csik.push_back(pontotVetit(pkezdo));
	}

	void megjelenit() {
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vec3) * csik.size(), &csik[0], GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		int location = glGetUniformLocation(gpuProgram.getId(), "color");
		glUniform3f(location, szin.x, szin.y, szin.z);
		glDrawArrays(GL_LINE_STRIP, 0, csik.size());
	}

	void padd(vec3 pkov) {
		csik.push_back(pontotVetit(pkov));
	}
};

Kor* hatter;
Nyal* nyal;
Hami* hami;
Hami* szaj;
Hami* jszem;
Hami* bszem;
Hami* jgolyo;
Hami* bgolyo;

Nyal* anyal;
Hami* ahami;
Hami* aszaj;
Hami* ajszem;
Hami* abszem;
Hami* ajgolyo;
Hami* abgolyo;

// Initialization, create an OpenGL context
void onInitialization() {
	glViewport(0, 0, windowWidth, windowHeight);
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	hatter = new Kor(vec3(0, 0, 0));

	nyal = new Nyal(vec3(1, 1, 1), vec3(0, 0, 0));
	anyal = new Nyal(vec3(1, 1, 1), vec3(1, 0, 0));

	hami = new Hami(vec3(1, 0, 0), vec3(0, 0, 0), vec3(0, 1, 0), 0.25);
	szaj = new Hami(vec3(0, 0, 0), vec3(0, 0.25, 0), vec3(0, 1, 0), 0.1);
	jszem = new Hami(vec3(1, 1, 1), vec3(0, 0.25, 0), vec3(0, 1, 0), 0.05);
	vec3 vj(vektorVetit(hami->getHelyzet(), vektorForgat(hami->getHelyzet(), hami->getIrany(), M_PI / 5)));
	jszem->setHelyzet(ujPont(hami->getHelyzet(), vj, 0.25));
	jszem->setIrany(vektorVetit(jszem->getHelyzet(), vj));
	bszem = new Hami(vec3(1, 1, 1), vec3(0, 0.25, 0), vec3(0, 1, 0), 0.05);
	vec3 vb(vektorVetit(hami->getHelyzet(), vektorForgat(hami->getHelyzet(), hami->getIrany(), M_PI / 5 * -1)));
	bszem->setHelyzet(ujPont(hami->getHelyzet(), vb, 0.25));
	bszem->setIrany(vektorVetit(bszem->getHelyzet(), vb));
	jgolyo = new Hami(vec3(0, 0, 1), vec3(0, 0.25, 0), vec3(0, 1, 0), 0.025);
	bgolyo = new Hami(vec3(0, 0, 1), vec3(0, 0.25, 0), vec3(0, 1, 0), 0.025);

	ahami = new Hami(vec3(0, 1, 0), vec3(1, 0, 0), vec3(0, 1, 0), 0.25);
	aszaj = new Hami(vec3(0, 0, 0), vec3(0, 0.25, 0), vec3(0, 1, 0), 0.1);
	ajszem = new Hami(vec3(1, 1, 1), vec3(0, 0.25, 0), vec3(0, 1, 0), 0.05);
	vec3 avj(vektorVetit(ahami->getHelyzet(), vektorForgat(ahami->getHelyzet(), ahami->getIrany(), M_PI / 5)));
	ajszem->setHelyzet(ujPont(ahami->getHelyzet(), avj, 0.25));
	ajszem->setIrany(vektorVetit(ajszem->getHelyzet(), avj));
	abszem = new Hami(vec3(1, 1, 1), vec3(0, 0.25, 0), vec3(0, 1, 0), 0.05);
	vec3 avb(vektorVetit(ahami->getHelyzet(), vektorForgat(ahami->getHelyzet(), ahami->getIrany(), M_PI / 5 * -1)));
	abszem->setHelyzet(ujPont(ahami->getHelyzet(), avb, 0.25));
	abszem->setIrany(vektorVetit(abszem->getHelyzet(), avb));
	ajgolyo = new Hami(vec3(0, 0, 1), vec3(0, 0.25, 0), vec3(0, 1, 0), 0.025);
	abgolyo = new Hami(vec3(0, 0, 1), vec3(0, 0.25, 0), vec3(0, 1, 0), 0.025);

	gpuProgram.create(vertexSource, fragmentSource, "outColor");
}

// Window has become invalid: Redraw
void onDisplay() {
	glClearColor(0.2, 0.2, 0.2, 0);
	glClear(GL_COLOR_BUFFER_BIT);

	hatter->megjelenit();

	nyal->megjelenit();
	anyal->megjelenit();

	hami->megjelenit();
	szaj->megjelenit();
	jszem->megjelenit();
	bszem->megjelenit();
	jgolyo->megjelenit();
	bgolyo->megjelenit();

	ahami->megjelenit();
	aszaj->megjelenit();
	ajszem->megjelenit();
	abszem->megjelenit();
	ajgolyo->megjelenit();
	abgolyo->megjelenit();

	glutSwapBuffers();
}

void szajValtozik(Hami* szaj, vec3 helyzet, vec3 irany) {
	szaj->setHelyzet(ujPont(pontotVetit(helyzet), vektorVetit(helyzet, irany), 0.25));
	szaj->setIrany(ujIrany(pontotVetit(helyzet), vektorVetit(helyzet, irany), 0.25));
}

void jszemValtozik(Hami* jszem, vec3 helyzet, vec3 irany) {
	jszem->setHelyzet(ujPont(pontotVetit(helyzet), vektorVetit(helyzet, irany), 0.25));
	jszem->setIrany(ujIrany(pontotVetit(helyzet), vektorVetit(helyzet, irany), 0.25));
	vec3 vj(vektorVetit(helyzet, vektorForgat(helyzet, irany, M_PI / 5)));
	jszem->setHelyzet(ujPont(helyzet, vj, 0.25));
	jszem->setIrany(vektorVetit(jszem->getHelyzet(), vj));
}

void bszemValtozik(Hami* bszem, vec3 helyzet, vec3 irany) {
	bszem->setHelyzet(ujPont(pontotVetit(helyzet), vektorVetit(helyzet, irany), 0.25));
	bszem->setIrany(ujIrany(pontotVetit(helyzet), vektorVetit(helyzet, irany), 0.25));
	vec3 vb(vektorVetit(helyzet, vektorForgat(helyzet, irany, M_PI / 5 * -1)));
	bszem->setHelyzet(ujPont(helyzet, vb, 0.25));
	bszem->setIrany(vektorVetit(bszem->getHelyzet(), vb));
}

void elore(Hami* hami, vec3 helyzet, vec3 irany, Nyal* nyal, float t) {
	hami->setHelyzet(pontotVetit(ujPont(hami->getHelyzet(), hami->getIrany(), t)));
	hami->setIrany(vektorVetit(hami->getHelyzet(), ujIrany(hami->getHelyzet(), hami->getIrany(), t)));

	nyal->padd(hami->getHelyzet());
}

void jobbra(Hami* hami, vec3 helyzet, vec3 irany) {
	hami->setIrany(vektorForgat(helyzet, irany, M_PI / 50));
}

void balra(Hami* hami, vec3 helyzet, vec3 irany) {
	hami->setIrany(vektorForgat(helyzet, irany, M_PI / 50 * -1));
}

void jgmozog(Hami* jszem, Hami* jgolyo, vec3 kovet) {
	vec3 jgpont = jszem->getHelyzet();
	vec3 jirany = vektorVetit(jgpont, iranyAllit(jgpont, pontotVetit(kovet)));
	jgolyo->setHelyzet(pontotVetit(ujPont(jgpont, jirany, 0.025)));
	jgolyo->setIrany(vektorVetit(jgolyo->getHelyzet(), jirany));
	jgolyo->megjelenit();
}

void bgmozog(Hami* bszem, Hami* bgolyo, vec3 kovet) {
	vec3 bgpont = bszem->getHelyzet();
	vec3 birany = vektorVetit(bgpont, iranyAllit(bgpont, pontotVetit(kovet)));
	bgolyo->setHelyzet(ujPont(bgpont, birany, 0.025));
	bgolyo->setIrany(vektorVetit(bgolyo->getHelyzet(), birany));
	bgolyo->megjelenit();
}

bool s = false;
bool e = false;
bool f = false;
// Key of ASCII code pressed
void onKeyboard(unsigned char key, int pX, int pY) {
	if (key == 's') {
		s = true;
	}
	if (key == 'e') {
		e = true;
	}
	if (key == 'f') {
		f = true;
	}
}

// Key of ASCII code released
void onKeyboardUp(unsigned char key, int pX, int pY) {
	if (key == 's') {
		s = false;
	}
	if (key == 'e') {
		e = false;
	}
	if (key == 'f') {
		f = false;
	}
}

// Move mouse with key pressed
void onMouseMotion(int pX, int pY) {}

// Mouse click event
void onMouse(int button, int state, int pX, int pY) {}

long eonIdle = 0;
float fi = 0;
// Idle event indicating that some time elapsed: do animation here
void onIdle() {
	long time = glutGet(GLUT_ELAPSED_TIME);
	float r = 0.1;
	long eltelt = time - eonIdle;
	long idokoz = 1000 / 90;

	if (time - idokoz > eonIdle) {
		int it = eltelt * 90 / 1000;
		for (int i = 0; i < it; ++i) {
			fi += 0.1;
			szaj->setR(r * sinf(fi));
			aszaj->setR(r * sinf(fi));

			if (s) {
				vec3 helyzet = pontotVetit(hami->getHelyzet());
				vec3 irany = vektorVetit(helyzet, hami->getIrany());

				jobbra(hami, helyzet, irany);

				helyzet = pontotVetit(hami->getHelyzet());
				irany = vektorVetit(helyzet, hami->getIrany());

				szajValtozik(szaj, helyzet, irany);
				jszemValtozik(jszem, helyzet, irany);
				bszemValtozik(bszem, helyzet, irany);
			}
			if (e) {
				vec3 helyzet = pontotVetit(hami->getHelyzet());
				vec3 irany = vektorVetit(helyzet, hami->getIrany());

				elore(hami, helyzet, irany, nyal, 0.05);

				helyzet = pontotVetit(hami->getHelyzet());
				irany = vektorVetit(helyzet, hami->getIrany());

				szajValtozik(szaj, helyzet, irany);
				jszemValtozik(jszem, helyzet, irany);
				bszemValtozik(bszem, helyzet, irany);
			}
			if (f) {
				vec3 helyzet = pontotVetit(hami->getHelyzet());
				vec3 irany = vektorVetit(helyzet, hami->getIrany());

				balra(hami, helyzet, irany);

				helyzet = pontotVetit(hami->getHelyzet());
				irany = vektorVetit(helyzet, hami->getIrany());

				szajValtozik(szaj, helyzet, irany);
				jszemValtozik(jszem, helyzet, irany);
				bszemValtozik(bszem, helyzet, irany);
			}

			jgmozog(jszem, jgolyo, pontotVetit(ahami->getHelyzet()));
			bgmozog(bszem, bgolyo, pontotVetit(ahami->getHelyzet()));


			vec3 ahelyzet = pontotVetit(ahami->getHelyzet());
			vec3 airany = vektorVetit(ahelyzet, ahami->getIrany());

			elore(ahami, ahelyzet, airany, anyal, 0.03);

			ahelyzet = pontotVetit(ahami->getHelyzet());
			airany = vektorVetit(ahelyzet, ahami->getIrany());

			vec3 phelyzet = ahami->getHelyzet();
			vec3 pirany = ahami->getIrany();

			szajValtozik(aszaj, ahelyzet, airany);
			jszemValtozik(ajszem, ahelyzet, airany);
			bszemValtozik(abszem, ahelyzet, airany);
			jgmozog(ajszem, ajgolyo, pontotVetit(hami->getHelyzet()));
			bgmozog(abszem, abgolyo, pontotVetit(hami->getHelyzet()));
			jobbra(ahami, ahami->getHelyzet(), ahami->getIrany());
		}

	}

	eonIdle = glutGet(GLUT_ELAPSED_TIME);
	glutPostRedisplay();
}
