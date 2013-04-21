#include <stdio.h>
#include <fcntl.h>
#include <sys/time.h>
#include <string.h>
#include <gphoto2/gphoto2-camera.h>
#include "camerad_photo.h"
#include "camerad_thumb.h"

static void errordumper(GPLogLevel level, const char *domain, const char *str, void *data) {
  struct timeval tv;
  gettimeofday (&tv, NULL);
  fprintf(stdout, "%d.%d: %s\n", (int) tv.tv_sec, (int) tv.tv_usec, str);
}


static void ctx_error_func (GPContext *context, const char *str, void *data) {
  fprintf(stderr, "\n*** Contexterror ***              \n%s\n",str);
  fflush(stderr);
}

static void ctx_status_func (GPContext *context, const char *str, void *data) {
  fprintf(stderr, "%s\n", str);
  fflush(stderr);
}

int camerad_photo_connect(
		camerad_photo_conn *conn, 
		char *photo_dir) {

	conn->context = gp_context_new();
  strncpy(conn->photo_dir, photo_dir, PATH_MAX);

  // set callbacks for camera messages
  gp_context_set_error_func(conn->context, ctx_error_func, NULL);
  gp_context_set_status_func(conn->context, ctx_status_func, NULL);
	gp_log_add_func(GP_LOG_ERROR, errordumper, NULL);

	gp_camera_new(&(conn->camera));

  printf("Camera init. Can take more than 10 seconds...\n");

  int ret = gp_camera_init(conn->camera, conn->context);

  if (ret < GP_OK) {
		fprintf(stderr, "No camera auto detected: %d\n", ret);
		return ret;
	}

	return 0;
}

int camerad_photo_capture(
		camerad_photo_conn *conn, 
		char *photo_filename,
		char *thumb_filename) {

  int fd, ret;
  CameraFile *file;
  CameraFilePath camera_file_path;

  // take a shot
  ret = gp_camera_capture(
		conn->camera, 
		GP_CAPTURE_IMAGE, 
		&camera_file_path, 
		conn->context);

	if (ret < GP_OK) {
		fprintf(stderr, "gp_camera_capture failed: %d\n", ret);
    return ret;
  }

  printf(
		"Pathname on the camera: %s/%s\n", 
		camera_file_path.folder, 
		camera_file_path.name
	);

	snprintf(photo_filename, PATH_MAX, "photo_%s", camera_file_path.name);

	snprintf(thumb_filename, PATH_MAX, "thumb_%s", camera_file_path.name);

  // save photo original
  char photo_path[PATH_MAX];
  snprintf(photo_path, PATH_MAX, "%s/%s", conn->photo_dir, photo_filename);
  fd = open(photo_path, O_CREAT | O_WRONLY, 0644);
  ret = gp_file_new_from_fd(&file, fd);

	if (ret < GP_OK) {
		fprintf(stderr, "gp_file_new_from_fd failed: %d\n", ret);
    return ret;
  }

  ret = gp_camera_file_get(
    conn->camera, 
    camera_file_path.folder, 
    camera_file_path.name,
    GP_FILE_TYPE_NORMAL, 
    file, 
    conn->context
  );

	if (ret < GP_OK) {
		fprintf(stderr, "gp_camera_file_get failed: %d\n", ret);
    return ret;
  }

  ret = gp_camera_file_delete(
    conn->camera, 
    camera_file_path.folder, 
    camera_file_path.name,
    conn->context
  );

	if (ret < GP_OK) {
		fprintf(stderr, "gp_camera_file_delete failed: %d\n", ret);
    return ret;
  }

  gp_file_free(file);

  // generate and save thumbnail
  char thumb_path[PATH_MAX];
  snprintf(thumb_path, PATH_MAX, "%s/%s", conn->photo_dir, thumb_filename);
  ret = camerad_thumb(photo_path, thumb_path);

	if (ret < GP_OK) {
		fprintf(stderr, "camerad_photo_resize failed: %d\n", ret);
  }

	return ret;
}

int camerad_photo_close(camerad_photo_conn *conn) {
  gp_camera_exit(conn->camera, conn->context);
  return 1;
}
