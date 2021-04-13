#pragma once

struct Queue {
	int size;
	int items[1024];
	int itemc;
};

void initialise_queue(int s, struct Queue * q);

int push(int n, struct Queue * q);

int pop(struct Queue * q);