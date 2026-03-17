#define NOB_UNSTRIP_PREFIX
#define NOB_IMPLEMENTATION
#include "nob.h"
#include "stb_image.h"
#include "stb_image_write.h"
#include <raylib.h>

#define LU_UNIMPLEMENTED()                                                  \
	do {                                                                \
		nob_log(NOB_ERROR, "This feature is not implemented yet."); \
		exit(1);                                                    \
	} while (false)

#define LU_MAX(a, b) (((a) > (b)) ? (a) : (b))

void lu_log_help(FILE* stream)
{
	fprintf(stream, "usage: lumina <task> [input_image]\n"
	                "tasks:\n"
	                "    lumina help                display this message\n"
	                "    lumina enhc <image>        enhance the image\n"
	                "    lumina blur <image>        blur the image\n"
	                "    lumina armf <image>        apply Arithmetic Mean Filter to the image\n"
	                "    lumina gemf <image>        apply Geometric Mean Filter to the image\n"
	                "    lumina glpf <image>        apply Gaussian Lowpass Filter to the image\n"
	                "    lumina blpf <image>        apply Butterworth Lowpass Filter to the image\n"
	                "    lumina ghpf <image>        apply Gaussian Highpass Filter to the image\n"
	                "    lumina bhpf <image>        apply Butterworth Highpass Filter to the image\n"
	                "    lumina admf <image>        apply Adaptive Median Filter to the image\n");
}

bool lu_str_eq(const char* a, const char* b)
{
	return strcmp(a, b) == 0;
}

typedef enum {
	TASK_ENHC,
	TASK_BLUR,
	TASK_ARMF,
	TASK_GEMF,
	TASK_GLPF,
	TASK_BLPF,
	TASK_GHPF,
	TASK_BHPF,
	TASK_ADMF,
} LuTask;

typedef struct {
	const char* path;
	int width;
	int height;
	int channels;
	stbi_uc* data;
} LuImage;

char* lu_add_ext(const char* path)
{
	const char* ext = ".jpg";
	size_t old_len  = strlen(path);
	size_t new_len  = old_len + strlen(ext);
	char* buff      = calloc(new_len + 1, sizeof(char));
	if (buff == nullptr) {
		nob_log(NOB_ERROR, "Could not allocate %zu bytes: %s", new_len + 1, strerror(errno));
		exit(1);
	}

	strcpy(buff, path);
	strcat(buff, ext);

	return buff;
}

LuImage lu_enhance(LuImage input)
{
	if (input.channels != 3) {
		nob_log(NOB_ERROR, "We currently only support 3-channel RGB image enchancing");
		exit(1);
	}

	size_t n       = input.width * input.height * input.channels;
	LuImage output = {
		.path     = lu_add_ext(input.path),
		.width    = input.width,
		.height   = input.height,
		.channels = input.channels,
		.data     = calloc(n, sizeof(input.data[0])),
	};

	if (output.data == nullptr) {
		nob_log(NOB_ERROR, "Could not allocate %zu bytes: %s", n, strerror(errno));
		exit(1);
	}

	nob_log(NOB_INFO, "Enhancing %s", input.path);

	for (size_t i = 0; i < n; ++i) {
		// Normalize to [-0.5, 0.5]
		float norm = (float)input.data[i] / 255.0f - 0.5f;

		// scale and clip
		float scale = 1.5f;
		norm *= scale; // [-scale * 0.5, scale * 0.5];
		if (norm < -0.5f) norm = -0.5f;
		if (norm > +0.5f) norm = +0.5f;

		// denormalize and clip
		int byte = (norm + 0.5f) * 255.0f;
		if (byte < 0) byte = 0;
		if (byte > 255) byte = 255;

		// write
		output.data[i] = (stbi_uc)byte;
	}

	return output;
}

void lu_log_image_info(LuImage image)
{
	nob_log(NOB_INFO, "width = %d", image.width);
	nob_log(NOB_INFO, "height = %d", image.height);
	nob_log(NOB_INFO, "channels = %d", image.channels);
}

LuImage lu_image_load(const char* image_path)
{
	nob_log(NOB_INFO, "Loading %s", image_path);
	FILE* image_file = fopen(image_path, "rb");
	if (image_file == nullptr) {
		nob_log(NOB_ERROR, "Could not open file %s for reading: %s", image_path, strerror(errno));
		exit(1);
	}

	LuImage image = { 0 };
	image.path    = image_path;
	image.data    = stbi_load_from_file(image_file, &image.width, &image.height, &image.channels, 3);
	fclose(image_file);

	if (image.data == nullptr) {
		nob_log(NOB_INFO, "stb_image could not load image %s: %s", image_path, stbi_failure_reason());
		exit(1);
	}

	lu_log_image_info(image);
	return image;
}

void lu_image_write(LuImage image)
{
	nob_log(NOB_INFO, "Writing %s", image.path);
	lu_log_image_info(image);
	int result = stbi_write_jpg(image.path, image.width, image.height, image.channels, image.data, 100);
	if (result == 0) {
		nob_log(NOB_ERROR, "stb_image_write could not write file %s", image.path);
		exit(1);
	}
}

int main(int argc, char* argv[argc])
{
	const char* program_path = nob_shift(argv, argc);
	NOB_UNUSED(program_path);

	// Checking if user provided task
	if (argc < 1) {
		nob_log(NOB_ERROR, "No task provided");
		lu_log_help(stderr);
		return 1;
	}

	// Parsing task
	const char* task_str = nob_shift(argv, argc);
	nob_log(NOB_INFO, "Task provided: %s", task_str);
	LuTask task;
	if (lu_str_eq(task_str, "help")) {
		lu_log_help(stdout);
		return 0;
	} else if (lu_str_eq(task_str, "enhc")) {
		task = TASK_ENHC;
	} else if (lu_str_eq(task_str, "blur")) {
		task = TASK_BLUR;
	} else if (lu_str_eq(task_str, "armf")) {
		task = TASK_ARMF;
	} else if (lu_str_eq(task_str, "gemf")) {
		task = TASK_GEMF;
	} else if (lu_str_eq(task_str, "glpf")) {
		task = TASK_GLPF;
	} else if (lu_str_eq(task_str, "blpf")) {
		task = TASK_BLPF;
	} else if (lu_str_eq(task_str, "ghpf")) {
		task = TASK_GHPF;
	} else if (lu_str_eq(task_str, "bhpf")) {
		task = TASK_BHPF;
	} else if (lu_str_eq(task_str, "admf")) {
		task = TASK_ADMF;
	} else {
		nob_log(NOB_ERROR, "Unknown task `%s`", task_str);
		lu_log_help(stderr);
		return 1;
	}

	// Checking if user provided input image path
	if (argc < 1) {
		nob_log(NOB_ERROR, "No input image provided");
		lu_log_help(stderr);
		return 1;
	}

	// Loading input image
	const char* input_path = nob_shift(argv, argc);
	nob_log(NOB_INFO, "Input image: %s", input_path);
	LuImage input = lu_image_load(input_path);

	// Performing the task
	LuImage output = { 0 };
	switch (task) {
	case TASK_ENHC: output = lu_enhance(input); break;
	case TASK_BLUR: LU_UNIMPLEMENTED(); break;
	case TASK_ARMF: LU_UNIMPLEMENTED(); break;
	case TASK_GEMF: LU_UNIMPLEMENTED(); break;
	case TASK_GLPF: LU_UNIMPLEMENTED(); break;
	case TASK_BLPF: LU_UNIMPLEMENTED(); break;
	case TASK_GHPF: LU_UNIMPLEMENTED(); break;
	case TASK_BHPF: LU_UNIMPLEMENTED(); break;
	case TASK_ADMF: LU_UNIMPLEMENTED(); break;
	default: NOB_UNREACHABLE("Unreachable task"); break;
	}

	// Writing output image
	lu_image_write(output);

	// Raylib visualization
	Image rl_input  = LoadImage(input.path);
	Image rl_output = LoadImage(output.path);

	int window_width  = rl_input.width + rl_output.width;
	int window_height = LU_MAX(rl_input.height, rl_output.height);

	InitWindow(window_width, window_height, "lumina");
	Texture2D rl_input_texture  = LoadTextureFromImage(rl_input);
	Texture2D rl_output_texture = LoadTextureFromImage(rl_output);

	while (!WindowShouldClose()) {
		BeginDrawing();
		{
			ClearBackground(BLACK);
			DrawTexture(rl_input_texture, 0, 0, WHITE);
			DrawTexture(rl_output_texture, rl_input.width, 0, WHITE);
		}
		EndDrawing();
	}

	CloseWindow();
	return 0;
}
