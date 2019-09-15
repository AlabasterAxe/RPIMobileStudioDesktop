/*******************************************************************
* Copyright (C) 2011 Rensselaer Polytechnic Institute              *
* This work is licensed under the FreeBSD License.                 *
* Please see the included LICENSE file in the top level directory. *
*******************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <math.h>

#include <pthread.h>
#include <GL/glut.h>
#include <fftw3.h>

#include "RPIMS.h"

#define SAMPLES 4096
#define CHANNELS 1

double samples[SAMPLES];
double spectrum[SAMPLES];
double window[SAMPLES];

pthread_mutex_t mutex;
struct ioboard_device *dev;

int quitting = 0;
int paused = 0;

int ch1 = 1;
int ch2 = 0;

double ch1_pos = 0.0;
double ch2_pos = 0.0;

double ch1_scale = 1.0;
double ch2_scale = 1.0;

float freq = 1000.0;
int trigger = 2500;

fftw_complex *in, *out;
fftw_plan p;

double A_0 = 0.35875;
double A_1 = 0.48829;
double A_2 = 0.14128;
double A_3 = 0.01168;

int nWidth;
int nHeight;

void
init_window(void)
{
	int i;
	double n;
	double N = (double) SAMPLES;

	for (i = 0; i < SAMPLES; i++) {
		n = ((double) i);
		window[i] = A_0
			- (A_1 * cos((2.0 * M_PI * n) / (N - 1.0)))
			+ (A_2 * cos((4.0 * M_PI * n) / (N - 1.0)))
			- (A_3 * cos((6.0 * M_PI * n) / (N - 1.0)));
	}
}


void *
retrieval_thread(void *arg)
{
	struct IOBoard *iob = NULL;
	struct AnalogStream *as = NULL;
	int i;
	int err;

	RPIMS_Init();

	iob = IOBoard_Probe(-1);
	if (!iob) {
		fprintf(stderr, "No IOBoard device found\n");
		goto exit;
	}

	as = AnalogStream_New(iob);
	if (!as) {
		fprintf(stderr, "Failed to create AnalogStream object\n");
		goto exit;
	}

	AnalogStream_SetChannelGain(as, STREAM_CHANNEL_A, 0);
	AnalogStream_SetChannelGain(as, STREAM_CHANNEL_B, 0);

	AnalogStream_SetTriggerChannel(as, STREAM_CHANNEL_A);
	AnalogStream_SetTriggerMode(as, STREAM_TRIGGER_MODE_OFF);
	AnalogStream_SetTriggerSource(as, STREAM_TRIGGER_SOURCE_INTERNAL);
	AnalogStream_SetTriggerSlope(as, STREAM_TRIGGER_SLOPE_RISING);
	AnalogStream_SetTriggerLevel(as, 0.0);

	/* Start Scope and Retrieve Data */
	fprintf(stderr, "Starting Capture...\n");

	err = AnalogStream_Start(as);
	if (err < 0) {
		fprintf(stderr, "Failed to start analog stream\n");
		goto exit;
	}

	init_window();

	in = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * SAMPLES);
	out = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * SAMPLES);
	p = fftw_plan_dft_1d(SAMPLES, in, out, FFTW_FORWARD, FFTW_ESTIMATE);

	while (1) {
		if (quitting)
			break;

		if (!paused) {

			//pthread_mutex_lock(&mutex);

			err = AnalogStream_GetData(as, samples, SAMPLES, STREAM_CHANNEL_A);
			if (err < 0) {
				fprintf(stderr, "Failed to get samples\n");
				goto exit;
			}

			for (i = 0; i < SAMPLES; i++) {
				in[i][0] = samples[i] * window[i];
				in[i][1] = 0;
			}
			fftw_execute(p);
			for (i = 0; i < SAMPLES; i++) {
				spectrum[i] = sqrt(out[i][0] * out[i][0] + out[i][1] * out[i][1]);
			}

			//pthread_mutex_unlock(&mutex);

			glutPostRedisplay();
		}
	}

 exit:
	return NULL;
}





void
reshape(int width, int height)
{
	nWidth = width;
	nHeight = height;

	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-20.0, 20.0, -2.0, 20.0, 0.0, 1.0);
	glMatrixMode(GL_MODELVIEW);
	glDisable(GL_DEPTH_TEST);
}


void
display()
{
    	double x, y;
	double val;
	int i;

	glClear(GL_COLOR_BUFFER_BIT);
	glLoadIdentity();


	glBegin(GL_LINES);
	glLineWidth(1.0);
        glColor3f(0.0,0.5,0.0);

	/* Horizontal Axis */
	glVertex2f(-20.0, 0.0);
	glVertex2f(20.0, 0.0);

	/* Vertical Axis */
	glVertex2f(0.0, -20.0);
	glVertex2f(0.0, 20.0);

	/* Horizontal Div */
	for(x = 0.0; x <= 20.0; x += 4.0) {
		glVertex2f(x, -0.5);
		glVertex2f(x, 0.5);
		glVertex2f(-1.0 * x, -0.5);
		glVertex2f(-1.0 * x, 0.5);
	}

	/* Vertical Div */
	for(y = 0.0; y <= 20.0; y += 4.0) {
		glVertex2f(-0.5, y);
		glVertex2f(0.5, y);
		glVertex2f(-0.5, -1.0 * y);
		glVertex2f(0.5, -1.0 * y);
	}
	glEnd();

	//pthread_mutex_lock(&mutex);

	/* Plot Data */
	if (ch1) {
		glLineWidth(1.0);
		glBegin(GL_LINE_STRIP);
		glColor3f(0.0, 1.0, 0.0);
		for (i = 0; i < SAMPLES / 2; i++) {
			x = -20.0 + 40.0 * ((double) i) / (((double) SAMPLES) / 2.0);
			val = spectrum[i];
			y = val * ch1_scale + ch1_pos;
			glVertex2f(x, y);
			if (CHANNELS > 1) i += 1;
		}
		glEnd();
	}

	if (ch2) {
		glLineWidth(1.0);
		glBegin(GL_LINE_STRIP);
		glColor3f(0.0, 1.0, 0.0);
		for (i = 0; i < SAMPLES / 2; i++) {
			if (CHANNELS > 1) i += 1;
			x = -20.0 + 40.0 * ((double) i) / (((double) SAMPLES) / 2.0);
			val = spectrum[i];
			y = val * ch2_scale + ch2_pos;
			glVertex2f(x, y);
		}
		glEnd();
	}

	//pthread_mutex_unlock(&mutex);

	glFlush();
	glutSwapBuffers();
}


void
keyboard(unsigned char key, int x, int y)
{
	switch (key) {

	case 'w':
		ch1_pos += 1.0;
		glutPostRedisplay();
		break;
	case 's':
		ch1_pos -= 1.0;
		glutPostRedisplay();
		break;
	case 'e':
		ch1_scale *= 2.0;
		glutPostRedisplay();
		break;
	case 'd':
		ch1_scale /= 2.0;
		glutPostRedisplay();
		break;


	case 'i':
		ch2_pos += 1.0;
		glutPostRedisplay();
		break;
	case 'k':
		ch2_pos -= 1.0;
		glutPostRedisplay();
		break;
	case 'o':
		ch2_scale *= 2.0;
		glutPostRedisplay();
		break;
	case 'l':
		ch2_scale /= 2.0;
		glutPostRedisplay();
		break;

	case '1':
		ch1 = !ch1;
		display();
		break;
	case '2':
		ch2 = !ch2;
		display();
		break;

	case 'p':
		paused = !paused;
		break;

	case 'q':
		quitting = 1;
		sleep(1);
		exit(0);

	default:
		break;
	}
}

void
mouse(int button, int state, int x, int y)
{
	if (state == GLUT_DOWN)
		fprintf(stderr, "Bin: %d / %d\n", ((x * SAMPLES) / (2 * nWidth)), SAMPLES);
}

int
main (int argc, char** argv)
{
	pthread_t tid;

	//usb_set_debug(0);

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
	glutInitWindowSize(640, 480);
	glutInitWindowPosition(0,0);
	glutCreateWindow("Mobile Studio Spectrum Analyzer");

	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutMouseFunc(mouse);
	glutDisplayFunc(display);
	//glutIdleFunc(display);

	if (pthread_mutex_init(&mutex, NULL)) {
		fprintf(stderr, "Mutex init failed.\n");
		exit(1);
	}

	if (pthread_create(&tid, NULL, retrieval_thread, NULL)) {
		fprintf(stderr, "Thread create failed.\n");
		exit(1);
	}

	glutMainLoop();

	return 0;
} 
