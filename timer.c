#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <termios.h>
#include <unistd.h>
#include <sys/select.h>
#include <pthread.h>

// Define the moves
const char *moves[] = {"U", "U'", "U2", "D", "D'", "D2", "L", "L'", "L2", "R", "R'", "R2", "F", "F'", "F2", "B", "B'", "B2"};

// Function to generate a random move
const char *random_move() {
    return moves[rand() % 18];
}

void timer(int inspection);
void reset_terminal(struct termios original);
void configure_terminal(struct termios *original);
void *handle_input(void *arg);
void print_scramble(int length);

typedef struct {
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    int input_received;
} input_state_t;

int main(int argc, char *argv[]) {
    struct termios original;
    configure_terminal(&original);

	bool scramble = true;
	if (argc > 1) {
		if (strcmp(argv[1], "--noscramble") == 0) {
			scramble = false;
		}
	}

	printf("Press space to start inspection or 'q' to quit.\n");

	bool skip_scramble = false;
    while (true) {
		if (!skip_scramble) {
			print_scramble(20);
		}

		char input = getchar();
        if (input == ' ') {
			timer(1);
			skip_scramble = false;
        } else if (input == 'q') {
            break;
        } else {
			skip_scramble = true;
		}
    }

    reset_terminal(original);
    return 0;
}

void timer(int inspection) {
    struct timespec start, end;
    time_t sec;
    long ns;
    double elapsed;

    if (inspection) {
        input_state_t state;
        pthread_mutex_init(&state.mutex, NULL);
        pthread_cond_init(&state.cond, NULL);
        state.input_received = 0;

        pthread_t input_thread;
        pthread_create(&input_thread, NULL, handle_input, &state);

        printf("Inspection: ");
        for (int i = 15; i >= 0; i--) {
            printf("%d ", i);
            fflush(stdout);

            struct timespec ts;
            clock_gettime(CLOCK_REALTIME, &ts);
            ts.tv_sec += 1;

            pthread_mutex_lock(&state.mutex);
            int result = 0;
            while (!state.input_received && result == 0) {
                result = pthread_cond_timedwait(&state.cond, &state.mutex, &ts);
            }
            pthread_mutex_unlock(&state.mutex);

            if (state.input_received) {
                break;
            }

            // Move the cursor back and clear the previous number
			if (i < 10) {
                printf("\b\b \b");
            } else {
                printf("\b\b\b \b");
            }
        }

        pthread_cancel(input_thread);
        pthread_join(input_thread, NULL);
        pthread_mutex_destroy(&state.mutex);
        pthread_cond_destroy(&state.cond);
    }

    printf("Solving... ");
    fflush(stdout);
    clock_gettime(CLOCK_MONOTONIC, &start);

    while (1) {
        if (getchar()) {
            clock_gettime(CLOCK_MONOTONIC, &end);
            sec = end.tv_sec - start.tv_sec;
            ns = end.tv_nsec - start.tv_nsec;
            elapsed = sec + ns * 1e-9;

            printf("%.2fs\n", elapsed);
            break;
        }
    }
}


void reset_terminal(struct termios original) {
    tcsetattr(STDIN_FILENO, TCSANOW, &original);
}

void configure_terminal(struct termios *original) {
    tcgetattr(STDIN_FILENO, original);

    struct termios new_term = *original;
    new_term.c_lflag &= ~(ICANON | ECHO);
    new_term.c_cc[VMIN] = 1;
    new_term.c_cc[VTIME] = 0;

    tcsetattr(STDIN_FILENO, TCSANOW, &new_term);
}

void *handle_input(void *arg) {
    input_state_t *state = (input_state_t *)arg;

    while (1) {
        char input = getchar();
        if (input == ' ') {
            pthread_mutex_lock(&state->mutex);
            state->input_received = 1;
            pthread_cond_signal(&state->cond);
            pthread_mutex_unlock(&state->mutex);
            break;
        }
    }

    return NULL;
}

// Function to generate a Rubik's cube scramble
void print_scramble(int length) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    srand((unsigned int) ts.tv_nsec);

    const char *last_move = NULL;
    const char *current_move;

    for (int i = 0; i < length; i++) {
        do {
            current_move = random_move();
        } while (last_move != NULL && (last_move[0] == current_move[0] || (last_move[0] == 'U' && current_move[0] == 'D') || (last_move[0] == 'D' && current_move[0] == 'U') || (last_move[0] == 'L' && current_move[0] == 'R') || (last_move[0] == 'R' && current_move[0] == 'L') || (last_move[0] == 'F' && current_move[0] == 'B') || (last_move[0] == 'B' && current_move[0] == 'F')));

        printf("%s ", current_move);
        last_move = current_move;
    }
}
