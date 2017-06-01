#include <stdio.h>
#include <stdlib.h>

#include <math.h>
#include <memory.h>
#include <assert.h>

#define NUM_VERTICES 8
#define NUM_LINKS    3

float SPACING = 0.005f;

// vertice states
enum tagSTATE {
	LINK = 0, BROKEN = NUM_VERTICES
};

float gt = 0.0f;

typedef struct tagGRAPH {
	float _position[3];
	float projection;
	tagSTATE state;
	int _link[3];
} GRAPH;

// initial description
GRAPH _initial[NUM_VERTICES] = {
	0.5f, 0.5f, 0.5f, 0.0f, LINK, 1, 3, 7,
		-.5f, 0.5f, 0.5f, 0.0f, LINK, 2, 0, 6,
		-.5f, 0.5f, -.5f, 0.0f, LINK, 3, 1, 5,
		0.5f, 0.5f, -.5f, 0.0f, LINK, 0, 2, 4,
		0.5f, -.5f, -.5f, 0.0f, LINK, 5, 7, 3,
		-.5f, -.5f, -.5f, 0.0f, LINK, 6, 4, 2,
		-.5f, -.5f, 0.5f, 0.0f, LINK, 7, 5, 1,
		0.5f, -.5f, 0.5f, 0.0f, LINK, 4, 6, 0
};

typedef struct _tagENVIRONMENT {
	GRAPH projection[NUM_VERTICES];

	int init[NUM_VERTICES];
	int end[1];
	int entries;
} ENVIRONMENT;

ENVIRONMENT env;

void eye(GRAPH * _graph, float _eye[3]) {
	int i, v;

	memcpy(env.projection, _initial, sizeof(GRAPH) * NUM_VERTICES);
	memset(env.init, 0, sizeof(int) * NUM_VERTICES);

	env.end[0] = 0;

	env.entries = 0;

	for (i = 0; i < NUM_VERTICES; i++) {
		for (v = 0; v < 3; v++) {
			_graph[i].projection += _graph[i]._position[v] * _eye[v];
		}

		// erro na compara��o entre pontos flutuantes
		if (_graph[i].projection == _graph[env.init[0]].projection) {
			env.init[env.entries] = i;

			env.entries++;
		}
		// invers�o na ordem de desenho
		else if (_graph[i].projection > _graph[env.init[0]].projection) {
			env.init[0] = i;

			env.entries = 1;
		}

		if (_graph[i].projection < _graph[env.end[0]].projection) {
			env.end[0] = i;
		}
	}
}

void reorder(GRAPH * _graph) {
	int v, l;
	int links[NUM_LINKS + (NUM_LINKS - 1)];

	for (v = 0; v < NUM_VERTICES; v++) {
		memcpy(links, _graph[v]._link, sizeof(int) * NUM_LINKS);

		memcpy(&links[NUM_LINKS], _graph[v]._link, sizeof(int) * 
			(NUM_LINKS - 1));

		for (l = 1; l >= 0; l--) { 
			if (_graph[v]._link[l] != BROKEN) {
				if (_graph[v].projection < _graph[_graph[v]._link[l]].projection) {
					memcpy(_graph[v]._link, &links[l + 1], sizeof(int) * 
						NUM_LINKS);
				}
			}
		}
	}
}

void reshape(GRAPH * _graph) {
	int l, r;

	for (l = 0; l < NUM_VERTICES; l++) {
		for (r = 0; r < NUM_VERTICES; r++) {
			if (_graph[l].projection <= _graph[r].projection) {
				for (int v = 0; v < NUM_LINKS; v++) {
					if (_graph[l]._link[v] == r) {
						_graph[l]._link[v] = BROKEN;
					}
				}
			}
		}
	}

}

void interpolation(GRAPH * _graph, int a, int b, float t) {
	float at, point[3];
	int p;

	at = (float) (fabs(t - _graph[a].projection) / 
		fabs(_graph[b].projection - _graph[a].projection));

	for (p = 0; p < 3; p++) {
		point[p] = (1.0f - at) * _graph[a]._position[p] + at * 
			_graph[b]._position[p];
	}

	glTexCoord3f(1.0f - (point[0] + 0.5f), point[1] + 0.5f, point[2] + 0.5f);
	glVertex3fv(point);
}

void cross(GRAPH * _graph, int vertice, float t) {
	int l;

	for (l = 0; l < NUM_LINKS; l++) {
		if (_graph[vertice]._link[l] != BROKEN) {
			if (_graph[_graph[vertice]._link[l]].projection < t) {
				interpolation(_graph, vertice, _graph[vertice]._link[l], t);
			}
			else {
				cross(_graph, _graph[vertice]._link[l], t);
			}
		}
	}
}

void draw(GRAPH * _graph) {
	int e;
	float t, init, end;

	init = _graph[env.init[0]].projection - SPACING;
	end = _graph[env.end[0]].projection + SPACING;

	for (t = init; t >= end; t -= SPACING) {
		glBegin(GL_POLYGON);

		for (e = 0; e < env.entries; e++) {
			cross(_graph, env.init[e], t);
		}

		glEnd();
	}
}

void spacing_between_planes(float space)
{
	SPACING = space;
}

void plot(float _eye[3]) {
	eye(env.projection, _eye);
	reorder(env.projection);
	reshape(env.projection);

	draw(env.projection);
}

