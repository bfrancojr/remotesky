#include <gphoto2/gphoto2-camera.h>
#include <linux/limits.h>

typedef struct {
	Camera *camera;
	GPContext *context;
	char photo_dir[PATH_MAX];
} camerad_photo_conn;

int camerad_photo_connect(
		camerad_photo_conn *conn, 
		char *photo_dir);
	
int camerad_photo_capture(
		camerad_photo_conn *conn, 
		char *photo_path,
		char *thumb_path);
	
int camerad_photo_close(camerad_photo_conn *conn);

