#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

MODULE_INFO(vermagic, VERMAGIC_STRING);

struct module __this_module
__attribute__((section(".gnu.linkonce.this_module"))) = {
 .name = KBUILD_MODNAME,
 .init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
 .exit = cleanup_module,
#endif
 .arch = MODULE_ARCH_INIT,
};

static const struct modversion_info ____versions[]
__attribute_used__
__attribute__((section("__versions"))) = {
	{ 0x3d2a5125, "struct_module" },
	{ 0x2f287f0d, "copy_to_user" },
	{ 0x4292364c, "schedule" },
	{ 0x9cda0123, "per_cpu__current_task" },
	{ 0x996993bf, "force_sig" },
	{ 0x46a20f1b, "find_task_by_pid" },
	{ 0x6dd5de5e, "wake_up_process" },
	{ 0x37e74642, "get_jiffies_64" },
	{ 0x529c3dd6, "kmem_cache_alloc" },
	{ 0xeb1de92, "malloc_sizes" },
	{ 0xdb23d1ed, "misc_register" },
	{ 0x52fb8df, "misc_deregister" },
	{ 0x37a0cba, "kfree" },
	{ 0x3afac169, "ptr2time_monitor_dump" },
	{ 0x1b7d4074, "printk" },
	{ 0x96807677, "ptr2time_monitor_exception_nmi" },
	{ 0xfe044103, "ptr2time_monitor_softirq" },
	{ 0x11709eac, "ptr2time_monitor_exception" },
	{ 0x4ce252a5, "ptr2time_monitor_interrupt" },
	{ 0xde14142c, "ptr2time_monitor" },
};

static const char __module_depends[]
__attribute_used__
__attribute__((section(".modinfo"))) =
"depends=";


MODULE_INFO(srcversion, "4B049F8A21AEC5F7FAD23A1");
