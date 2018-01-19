/**
 * Copyright (C) 2018 Xueyun Jiang
*/

#include <re.h>
#include <rem.h>
#include <baresip.h>
#include <pthread.h>


static struct vidsrc_cb g_cb;

void set_vidsrc_cb(struct vidsrc_cb cb) {
	g_cb = cb;
}

struct vidsrc_cb get_vidsrc_cb() {
	return g_cb;
}

struct vidsrc_st {
	const struct vidsrc *vs;  /* inheritance */
	int fps;
	struct vidsrc_prm prm;
	struct vidsz size;
	bool run;
	pthread_t thread;
	vidsrc_frame_h *frameh;
	void *arg;
};


static struct vidsrc *vidsrc;

static void destructor(void *arg)
{
	struct vidsrc_st *st = arg;

	(void)st;

	if (st->run) {
		st->run = false;
		pthread_join(st->thread, NULL);
	}
}

static void process(struct vidsrc_st *st)
{
	struct vidframe f;
	int size = st->size.w * st->size.h * 4;
	char *buf = (char *)malloc(size);
	static int color = 0;
	
	color = (color + 4) % 255;
	memset(buf, color, size);

	vidframe_init_buf(&f, VID_FMT_RGB32, &st->size, buf);
	st->frameh(&f, st->arg);

	info("vidsrc_cb: read");

	free(buf);
}

static void *read_thread(void *arg)
{
	struct vidsrc_st *st = arg;
	uint64_t ts = 0;

	while (st->run) {
		sys_msleep(1000 / st->prm.fps);
		process(st);

		ts += 1000/st->prm.fps;
	}

	return NULL;
}

static int alloc(struct vidsrc_st **stp, const struct vidsrc *vs,
		 struct media_ctx **ctx, struct vidsrc_prm *prm,
		 const struct vidsz *size, const char *fmt,
		 const char *dev, vidsrc_frame_h *frameh,
		 vidsrc_error_h *errorh, void *arg)
{
	struct vidsrc_st *st;
	int err = 0;

	warning("vidsrc_cb alloc\n");

	(void)ctx;
	(void)fmt;
	(void)dev;
	(void)errorh;

	if (g_cb.alloc) {
		g_cb.alloc(stp, vs, ctx, prm, size, fmt, dev, frameh, errorh, arg);
	}

	if (!stp || !prm || !size || !frameh)
		return EINVAL;

	st = mem_zalloc(sizeof(*st), destructor);
	if (!st)
		return ENOMEM;

	st->vs     = vs;
	st->size   = *size;
	st->fps    = prm->fps;
	st->frameh = frameh;
	st->arg    = arg;
	st->prm    = *prm;
	st->run    = true;

	err = pthread_create(&st->thread, NULL, read_thread, st);


 out:
	if (err)
		mem_deref(st);
	else
		*stp = st;

	return err;
}


static int module_init(void)
{
	return vidsrc_register(&vidsrc, "vidsrc_cb", alloc, NULL);
}


static int module_close(void)
{
	vidsrc = mem_deref(vidsrc);
	return 0;
}


EXPORT_SYM const struct mod_export DECL_EXPORTS(vidsrc_cb) = {
	"vidsrc_cb",
	"vidsrc",
	module_init,
	module_close
};
