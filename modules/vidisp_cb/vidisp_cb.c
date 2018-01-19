/**
 * Copyright (C) 2018 Xueyun Jiang.
 */

#include <re.h>
#include <rem.h>
#include <baresip.h>

static struct vidisp *vid;
static struct vidisp_cb g_cb;

struct vidisp_st {
	const struct vidisp *vd;        /**< Inheritance (1st)     */
};


void set_vidisp_cb(struct vidisp_cb cb) {
	g_cb = cb;
}

struct vidisp_cb get_vidisp_cb() {
	return g_cb;
}


static void destructor(void *arg)
{
	struct vidisp_st *st = arg;
	(void)st;
}


static int alloc(struct vidisp_st **stp, const struct vidisp *vd,
		 struct vidisp_prm *prm, const char *dev,
		 vidisp_resize_h *resizeh, void *arg)
{
	struct vidisp_st *st;
	int err = 0;

	warning("vidisp_cb alloc\n");

	/* Not used by SDL */
	(void)prm;
	(void)dev;
	(void)resizeh;
	(void)arg;
	(void)err;

	st = mem_zalloc(sizeof(*st), destructor);
	if (!st)
		return ENOMEM;

	st->vd = vd;

	if (g_cb.alloc) {
		g_cb.alloc(stp, vd, prm, dev, resizeh, arg);
	}

	*stp = st;
	return 0;
}


static int display(struct vidisp_st *st, const char *title,
		   const struct vidframe *frame)
{
	if (g_cb.disp) {
		g_cb.disp(st, title, frame);
	}

	info("vidisp_cb: display\n");

	return 0;
}


static void hide(struct vidisp_st *st)
{
	if (g_cb.hide) {
		g_cb.hide(st);
	}

	info("vidisp_cb: hide\n");
}


static int module_init(void)
{
	int err;

	err = vidisp_register(&vid, "vidisp_cb", alloc, NULL, display, hide);

	if (err)
		return err;

	return 0;
}


static int module_close(void)
{
	vid = mem_deref(vid);

	return 0;
}


EXPORT_SYM const struct mod_export DECL_EXPORTS(vidisp_cb) = {
	"vidisp_cb",
	"vidisp",
	module_init,
	module_close,
};
